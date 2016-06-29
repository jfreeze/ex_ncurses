#include <string.h>
#include "erl_nif.h"
#include <ncurses.h>
#include <unistd.h>  /* for sleep only */

char *
alloc_and_copy_to_cstring(ErlNifBinary *string)
{
    char *str = (char *) enif_alloc(string->size + 1);
    strncpy(str, (char *)string->data, string->size);
    str[string->size] = 0;
    return str;
}

void free_cstring(char * str) {
    enif_free(str);
}

ERL_NIF_TERM mk_atom(ErlNifEnv* env, const char* atom)
{
    ERL_NIF_TERM ret;

    if (!enif_make_existing_atom(env, atom, &ret, ERL_NIF_LATIN1))
    {
        return enif_make_atom(env, atom);
    }

    return ret;
}

static ERL_NIF_TERM
done(ErlNifEnv* env, int code)
{
    if(code == OK)
    {
        return mk_atom(env, "ok");
    }
    else
    {
        return enif_make_tuple2(env,
               enif_make_atom(env, "error"),
               enif_make_int(env, code));
    }
}

static ERL_NIF_TERM
  done_with_value(ErlNifEnv* env, int code)
{
    return enif_make_int(env, code);
}

ERL_NIF_TERM mk_error(ErlNifEnv* env, const char* mesg)
{
    return enif_make_tuple2(env, mk_atom(env, "error"), mk_atom(env, mesg));
}

static ERL_NIF_TERM
ex_initscr(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    initscr();
    return enif_make_int(env, 0);
}

static ERL_NIF_TERM
ex_endwin(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int code = endwin(); /* End curses mode  */
    return done(env, code);
}

static ERL_NIF_TERM
ex_clear(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int code = clear();
    return done(env, code);
}

static ERL_NIF_TERM
ex_raw(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
	  int code = raw();
    return done(env, code);
}

static ERL_NIF_TERM
ex_cbreak(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
	  int code = cbreak();
    return done(env, code);
}

static ERL_NIF_TERM
ex_nocbreak(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
	  int code = nocbreak();
    return done(env, code);
}

static ERL_NIF_TERM
ex_noecho(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
	  int code = noecho();
    return done(env, code);
}

static ERL_NIF_TERM
ex_printw(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary string;

    if (argc != 1 || ! enif_inspect_binary(env, argv[0], &string))
      return enif_make_badarg(env);

    char *str = alloc_and_copy_to_cstring(&string);
    int code  = printw(str);
    free_cstring(str);

    return done(env, code);
}

static ERL_NIF_TERM
ex_mvprintw(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    unsigned int x, y;
    ErlNifBinary string;

    if (argc != 3)
      return enif_make_badarg(env);

    if (! enif_get_uint(env, argv[0], &y))
      return enif_make_badarg(env);

    if (! enif_get_uint(env, argv[1], &x))
      return enif_make_badarg(env);

    if (! enif_inspect_binary(env, argv[2], &string))
      return enif_make_badarg(env);

    char *str = alloc_and_copy_to_cstring(&string);

    int code  = mvprintw(y, x, str);
    free_cstring(str);

    return done(env, code);
}

static ERL_NIF_TERM
ex_refresh(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int code = refresh();
    return done(env, code);
}

static ERL_NIF_TERM
ex_flushinp(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int code = flushinp();
    return done(env, code);
}

static ERL_NIF_TERM
ex_getch(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    timeout(1);
    int code = getch();

    return done_with_value(env, code);
}

/*
  // This is implemented in ex_ncurses.ex
static ERL_NIF_TERM
ex_getstr(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
}
*/

static ERL_NIF_TERM
ex_keypad(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    // keypad() => keypad(stdscr, TRUE);
    if (argc != 0 )
        return enif_make_badarg(env);

	  int code = keypad(stdscr, TRUE);
    return done(env, code);
}

static ERL_NIF_TERM
ex_start_color(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if (argc != 0 )
        return enif_make_badarg(env);

	  int code = start_color();
    return done(env, code);
}

static ERL_NIF_TERM
ex_has_colors(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    if (argc != 0 )
        return enif_make_badarg(env);

    if (has_colors())
        return mk_atom(env, "true");
    else
        return mk_atom(env, "false");
}

static ERL_NIF_TERM
ex_init_pair(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int pair, f, b;

    if (argc != 3 )
        return enif_make_badarg(env);

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
ex_attron(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int pair;

    if (argc != 1 )
        return enif_make_badarg(env);

    if (!enif_get_int(env, argv[0], &pair))
      return enif_make_badarg(env);

	  int code = attron(COLOR_PAIR(pair));

    return done(env, code);
}

static ERL_NIF_TERM
ex_attroff(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int pair;

    if (argc != 1 )
        return enif_make_badarg(env);

    if (!enif_get_int(env, argv[0], &pair))
      return enif_make_badarg(env);

	  int code = attroff(COLOR_PAIR(pair));

    return done(env, code);
}

static ERL_NIF_TERM
ex_gety(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  int y = getcury(stdscr);
  return enif_make_int(env, y);
}

static ERL_NIF_TERM
ex_getx(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  int x = getcurx(stdscr);
  return enif_make_int(env, x);
}

static ERL_NIF_TERM
ex_cols(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return enif_make_int(env, COLS);
}

static ERL_NIF_TERM
ex_lines(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return enif_make_int(env, LINES);
}

static ErlNifFunc nif_funcs[] =
{
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
    {"attroff",      1, ex_attroff,    0},
};

ERL_NIF_INIT(Elixir.ExNcurses, nif_funcs,
             NULL,NULL,NULL,NULL)
