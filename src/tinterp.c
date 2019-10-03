//-- Includes ------------------------------------------------------------------
#include "tinterp.h"

//-- Private types -------------------------------------------------------------
typedef struct {
    const char* name;
    void (*func)(uint32_t, tinterp_arg_t*);
    const char* args;
    const char* descr;
} cmd_t;

//-- Private functions prototypes ----------------------------------------------
static void cmd_help_callback(uint32_t argc, tinterp_arg_t* argv);
static void cmd_generic_callback(uint32_t argc, tinterp_arg_t* argv);
static bool is_space(char ch);
static bool is_en_capital(char ch);
static bool is_en_small(char ch);
static void conv_en_lower(char* ch);
static bool is_digit(char ch);
static bool is_ru_capital(char* ch);
static void conv_ru_lower(char* ch);
static void strip_multiple_spaces(char* str);
static void trim_spaces(char* s);
static int symbol_length(char ch);
static void clean_format(char* str);

//-- Private variables ---------------------------------------------------------
cmd_t cmd_map[TINTERP_CMDS_TOTAL] = {
        [TINTERP_CMD_HELP] = { "помощь", &cmd_help_callback, "", "- отображение этого сообщения" },
        [TINTERP_CMD_GET_TIME] = { "время", &cmd_generic_callback, "", "- показать текущее время" },
        [TINTERP_CMD_SET_TIME] = { "установить время", &cmd_generic_callback, "iii", "%ЧЧ%:%ММ%:%СС% - изменить текущее время" },
        [TINTERP_CMD_GET_NAME] = { "имя", &cmd_generic_callback, "", "- показать текущее имя устройства" },
        [TINTERP_CMD_SET_NAME] = { "установить имя", &cmd_generic_callback, "s", "%ИМЯ% - изменить текущее имя устройства" }
};

const char* delim = " ";

//-- Exported functions --------------------------------------------------------
void tinterp_parse(char* str)
{
    uint32_t argc = 0;
    tinterp_arg_t argv[TINTERP_CMD_ARGS_MAX];

    // prepare string
    clean_format(str);

    // try to find command
    for (int cmd_n = 0; cmd_n < TINTERP_CMDS_TOTAL; cmd_n++) {
        // first symbols of the str should be cmd name
        if (strncmp(str, cmd_map[cmd_n].name, strlen(cmd_map[cmd_n].name)) == 0) {
            // remove cmd name - only args stay in str
            // dest pointer need +1 to skip leading space
            memmove(str, str + strlen(cmd_map[cmd_n].name) + 1, strlen(str) - strlen(cmd_map[cmd_n].name));
            // parse args
            for (uint32_t arg_n = 0; arg_n < strlen(cmd_map[cmd_n].args); arg_n++) {
                char* tok = (arg_n == 0) ? strtok(str, delim) : strtok(NULL, delim);
                if (tok) {
                    switch (cmd_map[cmd_n].args[arg_n]) {
                    case 's':
                        argv[arg_n].s = tok;
                        break;
                    case 'i':
                        argv[arg_n].ui = strtoul(tok, NULL, 0);
                        break;
                    }
                    argc++;
                }
            }
            cmd_map[cmd_n].func(argc, argv);
            break;
        }
    }
}

void tinterp_set_callback(tinterp_cmds_t cmd, void (*func)(uint32_t, tinterp_arg_t*))
{
    if (cmd < TINTERP_CMDS_TOTAL)
        cmd_map[cmd].func = func;
}

//-- Private functions ---------------------------------------------------------
static void cmd_help_callback(uint32_t argc, tinterp_arg_t* argv)
{
    (void)argc;
    (void)argv;

    printf("Доступные команды:\n");
    for (int i = 0; i < TINTERP_CMDS_TOTAL; i++) {
        printf("\t%s %s\n", cmd_map[i].name, cmd_map[i].descr);
    }
}

static void cmd_generic_callback(uint32_t argc, tinterp_arg_t* argv)
{
    (void)argc;
    (void)argv;

    printf("Обработчик для команды не назначен!\n");
}

static bool is_space(char ch)
{
    return ch == ' ';
}

static bool is_en_capital(char ch)
{
    return (ch >= 'A') && (ch <= 'Z'); // 'A' - 'Z'
}

static bool is_en_small(char ch)
{
    return (ch >= 'a') && (ch <= 'z'); // 'a' - 'z'
}

static void conv_en_lower(char* ch)
{
    *ch += 0x20;
}

static bool is_digit(char ch)
{
    return (ch >= '0') && (ch <= '9'); // '0' - '9'
}

static bool is_ru_capital(char* ch)
{
    return ((unsigned char)ch[0] == 0xd0) && // 'А' - 'Я'
           ((unsigned char)ch[1] > 0x8f) && ((unsigned char)ch[1] < 0xb0);
}

static void conv_ru_lower(char* ch)
{
    if ((unsigned char)ch[0] == 0xd0 &&
        (unsigned char)ch[1] > 0x8f && // 'А' - 'П'
        (unsigned char)ch[1] < 0xa0)
        ch[1] += 0x20;
    else if ((unsigned char)ch[0] == 0xd0 &&
             (unsigned char)ch[1] > 0x9f && // 'Р' - 'Я'
             (unsigned char)ch[1] < 0xb0) {
        ch[0] += 0x01;
        ch[1] -= 0x20;
    }
}

static void strip_multiple_spaces(char* str)
{
    int i, x;
    for (i = x = 0; str[i]; i++)
        if (!is_space(str[i]) || (i > 0 && !is_space(str[i - 1])))
            str[x++] = str[i];
    str[x] = '\0';
}

static int symbol_length(char ch)
{
    if ((unsigned char)ch < 128)
        return 1; // ASCII byte
    else if ((unsigned char)ch < 128 + 64)
        return 1; //invalid byte
    else if ((unsigned char)ch < 128 + 64 + 32)
        return 2; // 2 bytes UTF-8 symbol
    return 1;
}

static void clean_format(char* str)
{
    int len = strlen(str);
    int nbytes = 1;

    for (int i = 0; i < len; i += nbytes) {
        nbytes = symbol_length(str[i]);
        if (nbytes == 1) { // ASCII symbols
            // convert to lower case
            if (is_en_capital(str[i]))
                conv_en_lower(&str[i]);
            // replace all symbols except letters and digits with spaces
            if (!is_en_capital(str[i]) && !is_en_small(str[i]) && !is_digit(str[i]))
                str[i] = ' ';
        } else if (nbytes == 2) { // cyrrilic utf8 symbols
            // convert to lower case
            if (is_ru_capital(&str[i]))
                conv_ru_lower(&str[i]);
        }
    }

    strip_multiple_spaces(str);
    trim_spaces(str);
}

static void trim_spaces(char* str)
{
    char* tmp_str = str;
    int l = strlen(tmp_str);

    if (l == 0)
        return;

    while (is_space(tmp_str[l - 1]))
        tmp_str[--l] = 0;
    while (*tmp_str && is_space(*tmp_str))
        ++tmp_str, --l;

    memmove(str, tmp_str, l + 1);
}
