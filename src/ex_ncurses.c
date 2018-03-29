#include <erl_nif.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

struct ex_ncurses_priv {
    ERL_NIF_TERM atom_ok;
    ERL_NIF_TERM atom_undefined;

    ErlNifResourceType *rt;
    void *resource;

    int ncurses_initialized;
};

static char *
alloc_and_copy_to_cstring(ErlNifBinary *string)
{
    char *str = (char *) enif_alloc(string->size + 1);
    strncpy(str, (char *)string->data, string->size);
    str[string->size] = 0;
    return str;
}

static void free_cstring(char *str)
{
    enif_free(str);
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
    if (!data->ncurses_initialized) {
        initscr();
        data->ncurses_initialized = 1;
    }

    return data->atom_ok;
}

static ERL_NIF_TERM
ex_endwin(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    int code = 0;
    if (data->ncurses_initialized) {
        code = endwin(); /* End curses mode  */
        data->ncurses_initialized = 0;
    }

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
ex_printw(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary string;

    if (!enif_inspect_binary(env, argv[0], &string))
        return enif_make_badarg(env);

    char *str = alloc_and_copy_to_cstring(&string);
    int code  = printw("%s", str);
    free_cstring(str);

    return done(env, code);
}

static ERL_NIF_TERM
ex_mvprintw(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    unsigned int x, y;
    ErlNifBinary string;

    if (!enif_get_uint(env, argv[0], &y))
        return enif_make_badarg(env);

    if (!enif_get_uint(env, argv[1], &x))
        return enif_make_badarg(env);

    if (!enif_inspect_binary(env, argv[2], &string))
        return enif_make_badarg(env);

    char *str = alloc_and_copy_to_cstring(&string);

    int code  = mvprintw(y, x, "%s", str);
    free_cstring(str);

    return done(env, code);
}

static ERL_NIF_TERM
ex_refresh(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int code = refresh();
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
    // keypad() => keypad(stdscr, TRUE);
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

    if (! enif_get_int(env, argv[0], &pair))
        return enif_make_badarg(env);

    if (! enif_get_int(env, argv[1], &f))
        return enif_make_badarg(env);

    if (! enif_get_int(env, argv[2], &b))
        return enif_make_badarg(env);

    int code = init_pair(pair, f, b);

    return done(env, code);
}

static ERL_NIF_TERM
ex_attron(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int pair;
    if (!enif_get_int(env, argv[0], &pair))
        return enif_make_badarg(env);

    int code = attron(COLOR_PAIR(pair));

    return done(env, code);
}

static ERL_NIF_TERM
ex_attroff(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    int pair;
    if (!enif_get_int(env, argv[0], &pair))
        return enif_make_badarg(env);

    int code = attroff(COLOR_PAIR(pair));

    return done(env, code);
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

static void rt_dtor(ErlNifEnv *env, void *obj)
{
    enif_fprintf(stderr, "rt_dtor called\r\n");
}

static void rt_stop(ErlNifEnv *env, void *obj, int fd, int is_direct_call)
{
    enif_fprintf(stderr, "rt_stop called %s\r\n", (is_direct_call ? "DIRECT" : "LATER"));
}

static void rt_down(ErlNifEnv *env, void *obj, ErlNifPid *pid, ErlNifMonitor *monitor)
{
    enif_fprintf(stderr, "rt_down called\r\n");

    ERL_NIF_TERM undefined;
    enif_make_existing_atom(env, "undefined", &undefined, ERL_NIF_LATIN1);
    enif_select(env, STDIN_FILENO, ERL_NIF_SELECT_STOP, obj, NULL, undefined);
}

static ErlNifResourceTypeInit rt_init = {rt_dtor, rt_stop, rt_down};

static int load(ErlNifEnv *env, void **priv, ERL_NIF_TERM info)
{
    struct ex_ncurses_priv *data = enif_alloc(sizeof(struct ex_ncurses_priv));
    if (!data)
        return 1;

    data->atom_ok = enif_make_atom(env, "ok");
    data->atom_undefined = enif_make_atom(env, "undefined");

    data->rt = enif_open_resource_type_x(env, "monitor", &rt_init, ERL_NIF_RT_CREATE, NULL);
    data->resource = enif_alloc_resource(data->rt, sizeof(int));

    data->ncurses_initialized = 0;

    *priv = (void *) data;

    return 0;
}

static void unload(ErlNifEnv *env, void *priv)
{
    enif_free(priv);
}

static ERL_NIF_TERM
ex_poll(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    if (!data->ncurses_initialized)
        return make_error(env, "uninitialized");

    int rc = enif_select(env, STDIN_FILENO, ERL_NIF_SELECT_READ, data->resource, NULL,
                         data->atom_undefined);
    return rc >= 0 ? data->atom_ok : make_error(env, "enif_select");
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

    int rc = enif_select(env, STDIN_FILENO, ERL_NIF_SELECT_READ, data->resource, NULL,
                         data->atom_undefined);
    return rc >= 0 ? enif_make_int(env, code) : make_error(env, "enif_select");
}

static ERL_NIF_TERM
ex_stop(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    struct ex_ncurses_priv *data = enif_priv_data(env);
    if (!data->ncurses_initialized)
        return make_error(env, "uninitialized");

    int rc = enif_select(env, STDIN_FILENO, ERL_NIF_SELECT_STOP, data->resource, NULL,
                         data->atom_undefined);
    return rc >= 0 ? data->atom_ok : make_error(env, "enif_select");
}

static ErlNifFunc invoke_funcs[] = {
    {"initscr",      0, ex_initscr,    0},
    {"endwin",       0, ex_endwin,     0},

    {"clear",        0, ex_clear,      0},
    {"refresh",      0, ex_refresh,    0},

    {"raw",          0, ex_raw,        0},

    {"cbreak",       0, ex_cbreak,     0},
    {"nocbreak",     0, ex_nocbreak,   0},

    {"noecho",       0, ex_noecho,     0},

    {"printw",       1, ex_printw,     0},
    {"mvprintw",     3, ex_mvprintw,   0},

    {"cols",         0, ex_cols,       0},
    {"lines",        0, ex_lines,      0},
    {"getx",         0, ex_getx,       0},
    {"gety",         0, ex_gety,       0},

    {"flushinp",     0, ex_flushinp,   0},
    {"keypad",       0, ex_keypad,     0},
    {"getch",        0, ex_getch,      0},

    {"start_color",  0, ex_start_color, 0},
    {"has_colors",   0, ex_has_colors, 0},
    {"init_pair",    3, ex_init_pair,  0},
    {"attron",       1, ex_attron,     0},
    {"attroff",      1, ex_attroff,    0}
};

static ERL_NIF_TERM
ex_invoke(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    if (argc != 2)
        return enif_make_badarg(env);

    char name[16];
    if (enif_get_atom(env, argv[0], name, sizeof(name), ERL_NIF_LATIN1) <= 0)
        return enif_make_badarg(env);

    int arity;
    const ERL_NIF_TERM *array;
    if (!enif_get_tuple(env, argv[1], &arity, &array))
        return enif_make_badarg(env);

    for (size_t i = 0; i < sizeof(invoke_funcs) / sizeof(ErlNifFunc); i++) {
        if (strcmp(invoke_funcs[i].name, name) == 0 &&
                invoke_funcs[i].arity == (unsigned) arity) {
            return invoke_funcs[i].fptr(env, arity, array);
        }
    }

    return enif_make_badarg(env);
}

static ErlNifFunc nif_funcs[] = {
    {"poll",      0, ex_poll,    0},
    {"read",      0, ex_read,  0},
    {"invoke",    2, ex_invoke,  0},
    {"stop",      0, ex_stop,  0},
};

ERL_NIF_INIT(Elixir.ExNcurses.Nif, nif_funcs,
             load, NULL, NULL, unload)
