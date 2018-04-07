#include <erl_nif.h>
#include <ncurses.h>
#include <string.h>

struct key_atom {
    int code;
    const char *atom;
};

static struct key_atom mapping[] = {
    { KEY_DOWN, "down" },
    { KEY_UP, "up" },
    { KEY_LEFT, "left" },
    { KEY_RIGHT, "right" },
    { KEY_HOME, "home" },
    { KEY_BACKSPACE, "backspace" },
    { KEY_F0, "f0" },
    { KEY_F(1), "f1" },
    { KEY_F(2), "f2" },
    { KEY_F(3), "f3" },
    { KEY_F(4), "f4" },
    { KEY_F(5), "f5" },
    { KEY_F(6), "f6" },
    { KEY_F(7), "f7" },
    { KEY_F(8), "f8" },
    { KEY_F(9), "f9" },
    { KEY_F(10), "f10" },
    { KEY_F(11), "f11" },
    { KEY_F(12), "f12" },
    { KEY_F(13), "f13" },
    { KEY_F(14), "f14" },
    { KEY_DL, "dl" },
    { KEY_IL, "il" },
    { KEY_DC, "dc" },
    { KEY_IC, "ic" },
    { KEY_EIC, "eic" },
    { KEY_CLEAR, "clear" },
    { KEY_EOS, "eos" },
    { KEY_EOL, "eol" },
    { KEY_SF, "sf" },
    { KEY_SR, "sr" },
    { KEY_NPAGE, "npage" },
    { KEY_PPAGE, "ppage" },
    { KEY_STAB, "stab" },
    { KEY_CTAB, "ctab" },
    { KEY_CATAB, "catab" },
    { KEY_ENTER, "enter" },
    { KEY_PRINT, "print" },
    { KEY_LL, "ll" },
    { KEY_A1, "a1" },
    { KEY_A3, "a3" },
    { KEY_B2, "b2" },
    { KEY_C1, "c1" },
    { KEY_C3, "c3" },
    { KEY_BTAB, "btab" },
    { KEY_BEG, "beg" },
    { KEY_CANCEL, "cancel" },
    { KEY_CLOSE, "close" },
    { KEY_COMMAND, "command" },
    { KEY_COPY, "copy" },
    { KEY_CREATE, "create" },
    { KEY_END, "end" },
    { KEY_EXIT, "exit" },
    { KEY_FIND, "find" },
    { KEY_HELP, "help" },
    { KEY_MARK, "mark" },
    { KEY_MESSAGE, "message" },
    { KEY_MOVE, "move" },
    { KEY_NEXT, "next" },
    { KEY_OPEN, "open" },
    { KEY_OPTIONS, "options" },
    { KEY_PREVIOUS, "previous" },
    { KEY_REDO, "redo" },
    { KEY_REFERENCE, "reference" },
    { KEY_REFRESH, "refresh" },
    { KEY_REPLACE, "replace" },
    { KEY_RESTART, "restart" },
    { KEY_RESUME, "resume" },
    { KEY_SAVE, "save" },
    { KEY_SBEG, "sbeg" },
    { KEY_SCANCEL, "scancel" },
    { KEY_SCOMMAND, "scommand" },
    { KEY_SCOPY, "scopy" },
    { KEY_SCREATE, "screate" },
    { KEY_SDC, "sdc" },
    { KEY_SDL, "sdl" },
    { KEY_SELECT, "select" },
    { KEY_SEND, "send" },
    { KEY_SEOL, "seol" },
    { KEY_SEXIT, "sexit" },
    { KEY_SFIND, "sfind" },
    { KEY_SHELP, "shelp" },
    { KEY_SHOME, "shome" },
    { KEY_SIC, "sic" },
    { KEY_SLEFT, "sleft" },
    { KEY_SMESSAGE, "smessage" },
    { KEY_SMOVE, "smove" },
    { KEY_SNEXT, "snext" },
    { KEY_SOPTIONS, "soptions" },
    { KEY_SPREVIOUS, "sprevious" },
    { KEY_SPRINT, "sprint" },
    { KEY_SREDO, "sredo" },
    { KEY_SREPLACE, "sreplace" },
    { KEY_SRIGHT, "sright" },
    { KEY_SRSUME, "srsume" },
    { KEY_SSAVE, "ssave" },
    { KEY_SSUSPEND, "ssuspend" },
    { KEY_SUNDO, "sundo" },
    { KEY_SUSPEND, "suspend" },
    { KEY_UNDO, "undo" },
    { KEY_MOUSE, "mouse" },
    { KEY_RESIZE, "resize" },
    { KEY_EVENT, "event" }
};

ERL_NIF_TERM key_to_elixir(ErlNifEnv *env, int code)
{
    // Handle normal characters
    if (code < 256)
        return enif_make_int(env, code);

    // Handle special characters
    size_t i;
    for (i = 0; i < sizeof(mapping) / sizeof(mapping[0]); i++) {
        if (code == mapping[i].code)
            return enif_make_atom(env, mapping[i].atom);
    }

    // If no other option.
    return enif_make_int(env, code);
}
