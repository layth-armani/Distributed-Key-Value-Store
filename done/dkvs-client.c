/**
 * @file dkvs-client.c
 * @brief DKVS command line interpreter
 *
 * DKVS Command Line Tool
 *
 * @author Ludovic Mermod
 * @author Jean-Cédric Chappelier
 */

#include "dkvs-client-cmds.h"
#include "util.h"

#include <limits.h>

// where dkvs command is
#define CMD_ARGC 1

#define STR_EQ(a, b) (strlen(a) == strlen(b) && strcmp(a, b) == 0)

#define CHECK_REM_ARGC(cond) ((ret) == ERR_NONE ? ((argc)cond ? ERR_NONE : ERR_INVALID_COMMAND) : (ret))

// Generic command type
typedef int (*command)(client_t *client, int argc, char **argv);

// All what is need to handle the command
struct command_mapping {
    const char *cmd_str;
    command cmd;
    size_t supported_args;
    int min_argc;
    int max_argc;
};

// commands mapping
int help(client_t *client _unused, int argc _unused, char **argv _unused);
int put(client_t *client, int argc, char **argv);
int get(client_t *client, int argc, char **argv);
int find(client_t *client, int argc, char **argv);

static struct command_mapping commands[] = {
    { "help"  , help            , 0                                       , -1, INT_MAX },
    { "get"   , get             , TOTAL_SERVERS | GET_NEEDED              , -1, INT_MAX },
    { "put"   , put             , TOTAL_SERVERS | PUT_NEEDED              , -1, INT_MAX },
};

// ======================================================================
int help(client_t *client _unused, int argc _unused, char **argv _unused)
{
    puts("dkvs-client <COMMAND> [-r R] [-w W] [-n N] -- [ARGUMENTS]");
    puts("where possible commands are:");
    puts("  help: displays this help;");
    puts("  get: get a value from a key, to be provided as extra argument (after \"--\");");
    puts("  put: put a key-value pair, to be provided as extra arguments, key first, then value;");
    puts("  cat: concatenate several values into a new key; the values are indicated by their key;");
    puts("       for example: cat -- a b c, will concatenate the values of keys a and b into the value for key c;");
    puts("  substr: extract a substring from a value and store the result under a new key;");
    puts("          it requires 4 arguments: key to read from, index of substr, length of substr, and key where to put the result to;");
    puts("  find: to search from a value into another; it requires two keys: which value to search into and what value to search;");
    puts("        it returns the index of the searched value inside the reference value (starting from 0) or -1 if the value is not found.");

    return ERR_NONE;
}

int put(client_t *client, int argc, char **argv){
    int ret = cli_client_put(client, argc,argv);
    return ret;
}
int get(client_t *client, int argc, char **argv){
    int ret = cli_client_get(client, argc,argv);
    return ret;
}

// ======================================================================
int main(int argc, char *argv[])
{
    int ret = ERR_NONE;
    if (argc < 2) {
        ret = ERR_INVALID_COMMAND;      
    } else {
        char *const cmd = argv[CMD_ARGC];
        argv += CMD_ARGC + 1;
        argc -= CMD_ARGC + 1;

        int found = 0;
        const size_t nb_cmds = sizeof(commands) / sizeof(commands[0]);
        for (size_t i = 0; !found && (i < nb_cmds); i++) {

            if (STR_EQ(commands[i].cmd_str, cmd)) {
                /* Cmd found */
                found = 1;

                client_t client;
                ret = client_init(
                      &client, commands[i].supported_args, &argc, &argv);
                if (ret == ERR_NONE) {
                    if (argc > commands[i].max_argc) {
                        ret = ERR_INVALID_COMMAND;
                    } else if (argc < commands[i].min_argc) {
                        ret = ERR_INVALID_COMMAND;
                    } else {
                        ret = commands[i].cmd(&client, argc, argv);
                    }
                    client_end(&client);
                }
            }
        }
        if (!found) {
            ret = ERR_INVALID_COMMAND;
        }
    }

    if (ret) {
        fprintf(stderr, "ERROR: %s\n", ERR_MSG(ret));
        help(NULL, argc, argv);
    }

    return ret;
}
