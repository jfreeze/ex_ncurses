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

    if(!enif_make_existing_atom(env, atom, &ret, ERL_NIF_LATIN1))
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

/*
static ERL_NIF_TERM
done_with_value(ErlNifEnv* env, char *str)
{
    return enif_make_string(env, str, ERL_NIF_LATIN1);
}
*/

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
static ERL_NIF_TERM
ex_getstr(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  //extern NCURSES_EXPORT(int) getstr (char *);
  //timeout(1);
    int code = getstr(str);

    return done_with_value(env, str);
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
cols(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return enif_make_int(env, COLS);
}

static ERL_NIF_TERM
lines(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    return enif_make_int(env, LINES);
}



static ERL_NIF_TERM
hello(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
	  WINDOW *my_win;
	  int startx, starty, width, height;
    char buffer [100];
    sprintf(buffer, "argc is: %d", argc);
    int r = 0;
    sprintf(buffer, "Hello world! - %d", r);

    initscr();                      /* Start curses mode            */
    noecho();
    cbreak();                       /* Line buffering disabled, Pass on
                                  * everty thing to me           */
    keypad(stdscr, TRUE);           /* I need that nifty F1         */

    height = 3;
    width = 10;
    starty = (LINES - height) / 2;  /* Calculating for a center placement */
    startx = (COLS - width) / 2;    /* of the window                */
    mvprintw(starty, startx, "Press F1 to exit");
    refresh();
    return enif_make_string(env, buffer, ERL_NIF_LATIN1);
}

static ErlNifFunc nif_funcs[] =
{
    {"hello", 0, hello},

    {"ex_initscr", 0, ex_initscr},
    {"ex_endwin", 0, ex_endwin},

    {"ex_clear", 0, ex_clear},
    {"ex_refresh", 0, ex_refresh},

    {"ex_raw", 0, ex_raw},

    {"ex_cbreak", 0, ex_cbreak},
    {"ex_nocbreak", 0, ex_nocbreak},

    {"ex_printw", 1, ex_printw},
    {"ex_mvprintw", 3, ex_mvprintw},

    {"cols", 0, cols},
    {"lines", 0, lines},
    {"ex_getx", 0, ex_getx},
    {"ex_gety", 0, ex_gety},

    {"ex_flushinp", 0, ex_flushinp},
    {"ex_keypad", 0, ex_keypad},
    {"ex_getch", 0, ex_getch}
};

ERL_NIF_INIT(Elixir.ExNcurses, nif_funcs,
             NULL,NULL,NULL,NULL)
