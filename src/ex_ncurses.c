#include <erl_nif.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

ERL_NIF_TERM key_to_elixir(ErlNifEnv *env, int code);

struct ex_window {
    WINDOW *win;
};

struct ex_ncurses_priv {
    ERL_NIF_TERM atom_ok;
    ERL_NIF_TERM atom_undefined;

    ErlNifResourceType *rt;
    ErlNifResourceType *window_rt;

    void *resource;

    bool using_real_stdin;

    int stdin_fd;
    FILE *stdin_fp;

    int pipe_to_nowhere[2];

    bool ncurses_initialized;
    bool polling;
};

// TODO: rename this to something
static void rt_dtor(ErlNifEnv *env, void *obj)
{
    //enif_fprintf(stderr, "rt_dtor called\r\n");
}

static void rt_stop(ErlNifEnv *env, void *obj, int fd, int is_direct_call)
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    //enif_fprintf(stderr, "rt_stop called %s, polling=%d\r\n", (is_direct_call ? "DIRECT" : "LATER"), data->polling);
    if (data->polling && is_direct_call) {
        data->polling = false;
        enif_select(env, STDIN_FILENO, ERL_NIF_SELECT_STOP, obj, NULL, data->atom_undefined);
    }
}

static void rt_down(ErlNifEnv *env, void *obj, ErlNifPid *pid, ErlNifMonitor *monitor)
{
    //enif_fprintf(stderr, "rt_down called\r\n");

    struct ex_ncurses_priv *data = enif_priv_data(env);
    if (data->polling) {
        data->polling = false;
        enif_select(env, data->stdin_fd, ERL_NIF_SELECT_STOP, obj, NULL, data->atom_undefined);
    }
}

static ErlNifResourceTypeInit rt_init = {rt_dtor, rt_stop, rt_down};

static void window_rt_dtor(ErlNifEnv *env, void *obj)
{
    enif_fprintf(stderr, "window_rt_dtor called\r\n");
}

static void window_rt_stop(ErlNifEnv *env, void *obj, int fd, int is_direct_call)
{
    enif_fprintf(stderr, "window_rt_stop called %s\r\n", (is_direct_call ? "DIRECT" : "LATER"));
}

static void window_rt_down(ErlNifEnv *env, void *obj, ErlNifPid *pid, ErlNifMonitor *monitor)
{
    enif_fprintf(stderr, "window_rt_down called\r\n");
}

static ErlNifResourceTypeInit window_rt_init = {window_rt_dtor, window_rt_stop, window_rt_down};

static int load(ErlNifEnv *env, void **priv, ERL_NIF_TERM info)
{
    struct ex_ncurses_priv *data = enif_alloc(sizeof(struct ex_ncurses_priv));
    if (!data)
        return 1;

    data->atom_ok = enif_make_atom(env, "ok");
    data->atom_undefined = enif_make_atom(env, "undefined");

    data->rt = enif_open_resource_type_x(env, "monitor", &rt_init, ERL_NIF_RT_CREATE, NULL);
    data->window_rt = enif_open_resource_type_x(env, "window", &window_rt_init, ERL_NIF_RT_CREATE, NULL);

    data->resource = enif_alloc_resource(data->rt, sizeof(int));

    data->ncurses_initialized = false;
    data->using_real_stdin = false;

    data->polling = false;

    data->stdin_fd = -1;
    data->stdin_fp = NULL;

    *priv = (void *) data;

    return 0;
}

static void unload(ErlNifEnv *env, void *priv)
{
    enif_free(priv);
}

static ERL_NIF_TERM make_error(ErlNifEnv *env, const char *error)
{
    return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, error));
}

static ERL_NIF_TERM
done(ErlNifEnv *env, int code)
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    if (code == OK)
        return data->atom_ok;
    else
        return enif_make_tuple2(env,
                                enif_make_atom(env, "error"),
                                enif_make_int(env, code));
}

static ERL_NIF_TERM
done_with_value(ErlNifEnv *env, int code)
{
    return enif_make_int(env, code);
}

static ERL_NIF_TERM
ex_initscr(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    if (data->ncurses_initialized)
        return data->atom_ok;

    ErlNifBinary string;
    if (!enif_inspect_binary(env, argv[0], &string))
        return enif_make_badarg(env);

    if (string.size == 0 || (string.size == 8 && memcmp(string.data, "/dev/tty", 8) == 0)) {
        // The user wants to take over Erlang's terminal

        // To avoid interfering with Erlang's tty_sl driver, we
        // duplicate the stdin filehandle and use that one instead.
        // Since we don't want Erlang to handle input anymore, we
        // replace STDIN_FILENO with a pipe to nowhere.
        data->stdin_fd = dup(STDIN_FILENO);
        if (data->stdin_fd <= 0) {
            enif_fprintf(stderr, "Error dup'ing stdin\n");
            return make_error(env, "dup");
        }
        if (pipe(data->pipe_to_nowhere) < 0) {
            enif_fprintf(stderr, "Error creating pipe to nowhere\n");
            close(data->stdin_fd);
            return make_error(env, "pipe");
        }
        // NOTE: dup2 automatically closes STDIN_FILENO before replacing it
        //       so no need to call close. Unfortunately, if STDIN_FILENO is
        //       in another Erlang thread's call to select(2), select will
        //       return an error on the handle which will then get printed
        //       to the console. It would be nice if initscr could be
        //       guaranteed to be called from the thread servicing the
        //       console, but I don't know how to do that. :(
        int rc = dup2(data->pipe_to_nowhere[0], STDIN_FILENO);
        if (rc != STDIN_FILENO) {
            enif_fprintf(stderr, "Error replacing stdin\n");
            return make_error(env, "dup2");
        }

        data->stdin_fp = fdopen(data->stdin_fd, "r");

        // initscr invokes newterm insternally. Update it with
        // our new "stdin"
        newterm(getenv("TERM"), stdout, data->stdin_fp);

        data->using_real_stdin = true;
    } else {
        // Use the user-supplied terminal instead
        char termname[string.size + 1];
        memcpy(termname, string.data, string.size);
        termname[string.size] = 0;

        data->stdin_fp = fopen(termname, "r+e");
        if (data->stdin_fp == NULL)
            return make_error(env, "enoent");
        data->stdin_fd = fileno(data->stdin_fp);

        newterm(getenv("TERM"), data->stdin_fp, data->stdin_fp);

        data->using_real_stdin = false;
    }
    data->ncurses_initialized = true;

    return data->atom_ok;
}

static ERL_NIF_TERM
ex_endwin(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    int code = endwin();

    if (data->polling) {
        data->polling = false;
        int rc = enif_select(env, data->stdin_fd, ERL_NIF_SELECT_STOP, data->resource, NULL,
                             data->atom_undefined);
        if (rc < 0)
            enif_fprintf(stderr, "enif_select(STOP) return error");
    }

    if (data->using_real_stdin) {
        // Undo our manipulation of filehandles from initscr
        // by dup'ing the real stdin back to STDIN_FILENO.
        dup2(data->stdin_fd, STDIN_FILENO);

        close(data->pipe_to_nowhere[0]);
        close(data->pipe_to_nowhere[1]);
    }

    fclose(data->stdin_fp); // This also closes data->stdin_fd
    data->stdin_fd = -1;
    data->stdin_fp = NULL;

    data->ncurses_initialized = false;

    return done(env, code);
}

static ERL_NIF_TERM
ex_clear(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = clear();
    return done(env, code);
}

static ERL_NIF_TERM
ex_raw(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = raw();
    return done(env, code);
}

static ERL_NIF_TERM
ex_beep(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    return done(env, beep());
}

static ERL_NIF_TERM
ex_cbreak(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = cbreak();
    return done(env, code);
}

static ERL_NIF_TERM
ex_nocbreak(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = nocbreak();
    return done(env, code);
}

static ERL_NIF_TERM
ex_noecho(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = noecho();
    return done(env, code);
}

static ERL_NIF_TERM
ex_setscrreg(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    unsigned int top, bottom;
    if (!enif_get_uint(env, argv[0], &top) ||
            !enif_get_uint(env, argv[1], &bottom))
        return enif_make_badarg(env);

    int code = setscrreg(top, bottom);
    return done(env, code);
}

static ERL_NIF_TERM
ex_printw(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary string;

    if (!enif_inspect_binary(env, argv[0], &string))
        return enif_make_badarg(env);

    // printw is the same as addstr since there's no way of passing
    // printf-formatted strings from Elixir
    int code = addnstr((const char *) string.data, string.size);
    return done(env, code);
}

static ERL_NIF_TERM
ex_mvprintw(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    unsigned int x, y;
    ErlNifBinary string;

    if (!enif_get_uint(env, argv[0], &y) ||
            !enif_get_uint(env, argv[1], &x) ||
            !enif_inspect_binary(env, argv[2], &string))
        return enif_make_badarg(env);

    int code = mvaddnstr(y, x, (const char *) string.data, string.size);
    return done(env, code);
}

static ERL_NIF_TERM
ex_refresh(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = refresh();
    return done(env, code);
}

static ERL_NIF_TERM
ex_wrefresh(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    struct ex_window *obj;
    if (!enif_get_resource(env, argv[0], data->window_rt, (void**) &obj))
        return enif_make_badarg(env);

    int code = wrefresh(obj->win);
    return done(env, code);
}

static ERL_NIF_TERM
ex_flushinp(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = flushinp();
    return done(env, code);
}

static ERL_NIF_TERM
ex_getch(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = getch();
    return done_with_value(env, code);
}

static ERL_NIF_TERM
ex_keypad(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = keypad(stdscr, TRUE);
    return done(env, code);
}

static ERL_NIF_TERM
ex_start_color(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = start_color();
    return done(env, code);
}

static ERL_NIF_TERM
ex_has_colors(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    if (has_colors())
        return enif_make_atom(env, "true");
    else
        return enif_make_atom(env, "false");
}

static ERL_NIF_TERM
ex_init_pair(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int pair, f, b;
    if (!enif_get_int(env, argv[0], &pair) ||
            !enif_get_int(env, argv[1], &f) ||
            !enif_get_int(env, argv[2], &b))
        return enif_make_badarg(env);

    int code = init_pair(pair, f, b);

    return done(env, code);
}

static ERL_NIF_TERM
ex_mvcur(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int oldrow, oldcol, newrow, newcol;
    if (!enif_get_int(env, argv[0], &oldrow) ||
            !enif_get_int(env, argv[1], &oldcol) ||
            !enif_get_int(env, argv[1], &newrow) ||
            !enif_get_int(env, argv[2], &newcol))
        return enif_make_badarg(env);

    int code = mvcur(oldrow, oldcol, newrow, newcol);

    return done(env, code);
}

static ERL_NIF_TERM
ex_attron(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int attr;
    if (!enif_get_int(env, argv[0], &attr))
        return enif_make_badarg(env);

    int code = attron(attr);

    return done(env, code);
}

static ERL_NIF_TERM
ex_attroff(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int attr;
    if (!enif_get_int(env, argv[0], &attr))
        return enif_make_badarg(env);

    int code = attroff(attr);

    return done(env, code);
}

static ERL_NIF_TERM
ex_border(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    // Use the defaults
    chtype ls = 0;
    chtype rs = 0;
    chtype ts = 0;
    chtype bs = 0;
    chtype tl = 0;
    chtype tr = 0;
    chtype bl = 0;
    chtype br = 0;
    return done(env, border(ls, rs, ts, bs, tl, tr, bl, br));
}

static ERL_NIF_TERM
ex_wborder(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    struct ex_window *obj;
    if (!enif_get_resource(env, argv[0], data->window_rt, (void**) &obj))
        return enif_make_badarg(env);

    // Use the defaults
    chtype ls = 0;
    chtype rs = 0;
    chtype ts = 0;
    chtype bs = 0;
    chtype tl = 0;
    chtype tr = 0;
    chtype bl = 0;
    chtype br = 0;
    return done(env, wborder(obj->win, ls, rs, ts, bs, tl, tr, bl, br));
}

static ERL_NIF_TERM
ex_wclear(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    struct ex_window *obj;
    if (!enif_get_resource(env, argv[0], data->window_rt, (void**) &obj))
        return enif_make_badarg(env);

    return done(env, wclear(obj->win));
}

static ERL_NIF_TERM
ex_gety(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int y = getcury(stdscr);
    return enif_make_int(env, y);
}

static ERL_NIF_TERM
ex_getx(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int x = getcurx(stdscr);
    return enif_make_int(env, x);
}

static ERL_NIF_TERM
ex_cols(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    return enif_make_int(env, COLS);
}

static ERL_NIF_TERM
ex_lines(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    return enif_make_int(env, LINES);
}

static ERL_NIF_TERM
ex_newwin(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int nlines, ncols, begin_y, begin_x;
    if (!enif_get_int(env, argv[0], &nlines) ||
            !enif_get_int(env, argv[1], &ncols) ||
            !enif_get_int(env, argv[2], &begin_y) ||
            !enif_get_int(env, argv[3], &begin_x))
        return enif_make_badarg(env);

    WINDOW *win = newwin(nlines, ncols, begin_y, begin_x);
    if (win == NULL)
        return make_error(env, "newwin");

    struct ex_ncurses_priv *data = enif_priv_data(env);
    struct ex_window *obj = enif_alloc_resource(data->window_rt, sizeof(struct ex_window));
    obj->win = win;

    ERL_NIF_TERM rc = enif_make_resource(env, obj);
    enif_release_resource(obj); // Let Erlang manage this resource now.
    return rc;
}

static ERL_NIF_TERM
ex_delwin(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    struct ex_window *obj;
    if (!enif_get_resource(env, argv[0], data->window_rt, (void**) &obj))
        return enif_make_badarg(env);

    return done(env, delwin(obj->win));
}

static int
get_boolean(ErlNifEnv *env, ERL_NIF_TERM term, bool *v)
{
    char buffer[16];
    if (enif_get_atom(env, term, buffer, sizeof(buffer), ERL_NIF_LATIN1) <= 0)
        return false;

    if (strcmp("false", buffer) == 0)
        *v = false;
    else
        *v = true;

    return true;
}

static ERL_NIF_TERM
ex_scrollok(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    WINDOW *win = stdscr;
    bool is_scrollok = true;
    if (argc == 2) {
        struct ex_window *obj;
        if (!enif_get_resource(env, argv[0], data->window_rt, (void**) &obj) ||
                !get_boolean(env, argv[1], &is_scrollok))
            return enif_make_badarg(env);
        win = obj->win;
    }
    return done(env, scrollok(win, is_scrollok));
}

static ERL_NIF_TERM
ex_waddstr(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    struct ex_window *obj;
    ErlNifBinary string;
    if (!enif_get_resource(env, argv[0], data->window_rt, (void**) &obj) ||
            !enif_inspect_binary(env, argv[1], &string))
        return enif_make_badarg(env);

    // printw is the same as addstr since there's no way of passing
    // printf-formatted strings from Elixir
    int code = waddnstr(obj->win, (const char *) string.data, string.size);
    return done(env, code);
}

static ERL_NIF_TERM
ex_wmove(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    struct ex_window *obj;
    int y, x;
    if (!enif_get_resource(env, argv[0], data->window_rt, (void**) &obj) ||
            !enif_get_int(env, argv[1], &y)||
            !enif_get_int(env, argv[2], &x))
        return enif_make_badarg(env);

    int code = wmove(obj->win, y, x);
    return done(env, code);
}

static ERL_NIF_TERM
ex_move(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int y, x;
    if (!enif_get_int(env, argv[0], &y) ||
            !enif_get_int(env, argv[1], &x))
        return enif_make_badarg(env);

    int code = move(y, x);
    return done(env, code);
}

static ERL_NIF_TERM
ex_curs_set(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int visibility;
    if (!enif_get_int(env, argv[0], &visibility))
        return enif_make_badarg(env);

    return done(env, curs_set(visibility));
}

static ERL_NIF_TERM
ex_poll(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    if (!data->ncurses_initialized)
        return make_error(env, "uninitialized");

    if (data->polling)
        return data->atom_ok;

    data->polling = true;
    int rc = enif_select(env, data->stdin_fd, ERL_NIF_SELECT_READ, data->resource, NULL,
                         data->atom_undefined);
    if (rc >= 0) {
        return data->atom_ok;
    } else {
        data->polling = false;
        return make_error(env, "enif_select");
    }
}

static ERL_NIF_TERM
ex_read(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    if (!data->ncurses_initialized)
        return make_error(env, "uninitialized");

    int code = getch();
    if (code < 0) {
        enif_fprintf(stderr, "Unexpected error from getch() -> %d!\n", code);
        return make_error(env, "getch");
    }

    if (data->polling) {
        int rc = enif_select(env, data->stdin_fd, ERL_NIF_SELECT_READ, data->resource, NULL,
                             data->atom_undefined);
        if (rc < 0) {
            data->polling = false;
            return make_error(env, "enif_select");
        }
    }

    return key_to_elixir(env, code);
}

static ErlNifFunc invoke_funcs[] = {
    {"addstr",       1, ex_printw,     0},
    {"attron",       1, ex_attron,     0},
    {"attroff",      1, ex_attroff,    0},
    {"beep",         0, ex_beep,       0},
    {"border",       0, ex_border,     0},
    {"cbreak",       0, ex_cbreak,     0},
    {"nocbreak",     0, ex_nocbreak,   0},
    {"clear",        0, ex_clear,      0},
    {"cols",         0, ex_cols,       0},
    {"curs_set",     1, ex_curs_set,   0},
    {"delwin",       1, ex_delwin,     0},
    {"noecho",       0, ex_noecho,     0},
    {"flushinp",     0, ex_flushinp,   0},
    {"getch",        0, ex_getch,      0},
    {"getx",         0, ex_getx,       0},
    {"gety",         0, ex_gety,       0},
    {"has_colors",   0, ex_has_colors, 0},
    {"init_pair",    3, ex_init_pair,  0},
    {"keypad",       0, ex_keypad,     0},
    {"lines",        0, ex_lines,      0},
    {"move",         2, ex_move,       0},
    {"mvaddstr",     3, ex_mvprintw,   0},
    {"mvcur",        4, ex_mvcur,      0},
    {"mvprintw",     3, ex_mvprintw,   0},
    {"newwin",       4, ex_newwin,     0},
    {"printw",       1, ex_printw,     0},
    {"raw",          0, ex_raw,        0},
    {"refresh",      0, ex_refresh,    0},
    {"scrollok",     0, ex_scrollok,   0},
    {"setscrreg",    2, ex_setscrreg,  0},
    {"start_color",  0, ex_start_color, 0},
    {"waddstr",      1, ex_waddstr,    0},
    {"wborder",      1, ex_wborder,    0},
    {"wclear",       1, ex_wclear,     0},
    {"wmove",        3, ex_wmove,      0},
    {"wrefresh",     1, ex_wrefresh,   0}
};

static ERL_NIF_TERM
ex_invoke(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    char name[16];
    const ERL_NIF_TERM *array;
    int arity;

    if (argc != 2 ||
            enif_get_atom(env, argv[0], name, sizeof(name), ERL_NIF_LATIN1) <= 0 ||
            !enif_get_tuple(env, argv[1], &arity, &array))
        return enif_make_badarg(env);

    // Check that initscr has been called
    struct ex_ncurses_priv *data = enif_priv_data(env);
    if (!data->ncurses_initialized)
        return make_error(env, "uninitialized");

    size_t i;
    for (i = 0; i < sizeof(invoke_funcs) / sizeof(ErlNifFunc); i++) {
        if (strcmp(invoke_funcs[i].name, name) == 0 &&
                invoke_funcs[i].arity <= (unsigned) arity) {
            return invoke_funcs[i].fptr(env, arity, array);
        }
    }

    return enif_make_badarg(env);
}

static ErlNifFunc nif_funcs[] = {
    {"endwin",    0, ex_endwin,  0},
    {"initscr",   1, ex_initscr, 0},
    {"invoke",    2, ex_invoke,  0},
    {"poll",      0, ex_poll,    0},
    {"read",      0, ex_read,    0}
};

ERL_NIF_INIT(Elixir.ExNcurses.Nif, nif_funcs,
             load, NULL, NULL, unload)
