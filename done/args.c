#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "args.h"
#include "util.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))


/**
 * @brief Validates if the total number of servers meets the required thresholds
 *        for PUT and GET operations.
 *
 * @param args Pointer to an args_t structure containing server and operation requirements.
 * @return int Returns 1 (true) if the total servers are sufficient for both
 *             PUT and GET operations, otherwise returns 0 (false).
 */
int check_Valid_NWR (args_t* args){
    int check = (args->total_servers >= args->put_needed) &&
                 (args->total_servers >= args->get_needed);
    return check;
}


int check_valid_option(char* c) {
    if (!c || (strlen(c)==0)) {
        return 0; 
    }

    for (int i = 0; c[i] != '\0'; i++) {
        if (c[i] < '0' || c[i] > '9') {
            return 0; 
        }
    }

    return 1; 
}


int check_valid_flag(char* str, const char* option, int valid){
    return !strncmp(str,option,strlen(option)) && valid;
}




/**
 * @brief parse optional arguments
 * @param args (output) the args_t struct to be initialized
 * @param supported_args OR'ed args_kind flags of supported options
 * @param argc (output) the number of arguments remaining (after optional arguments have been parsed)
 * @param rem_argv (output) the array of remaining arguments
 * @return an error code
 */
int parse_opt_args(args_t *args, size_t supported_args, int *argc, char ***rem_argv) {
    if (!args || !argc || !rem_argv) {
        fprintf(stderr, "ERROR: ONE OF THE ARGUMENTS IS NULL\n");
        return ERR_INVALID_ARGUMENT;
    }

    char **char_list = *rem_argv;
    size_t N = 0, R = 0, W = 0;
    int endParse = 0;

    int option_N = (TOTAL_SERVERS & supported_args) != 0;
    int option_R = (GET_NEEDED & supported_args) != 0;
    int option_W = (PUT_NEEDED & supported_args) != 0;

    while (*argc > 0 && !endParse) {
        if (check_valid_flag(char_list[0], "-n", option_N)) {
            --(*argc);
            ++char_list;

            if (*argc == 0 || !check_valid_option(char_list[0])) {
                fprintf(stderr, "NO ARGUMENTS FOR OPTION OR INVALID COMMAND -n\n");
                return ERR_INVALID_COMMAND;
            }
            if ((N = atouint16(char_list[0])) <= 0) {
                return ERR_INVALID_COMMAND;
            }
            ++char_list;
            --(*argc);
            
        } 
        else if (check_valid_flag(char_list[0], "-w", option_W)) {
            --(*argc);
            ++char_list;
            if (*argc == 0 || !check_valid_option(char_list[0])) {
                fprintf(stderr, "NO ARGUMENTS FOR OPTION OR INVALID COMMAND -w\n");
                return ERR_INVALID_COMMAND;
            }
            if ((W = atouint16(char_list[0])) <= 0) {
                return ERR_INVALID_COMMAND;
            }
            ++char_list;
            --(*argc);

        } 
        else if (check_valid_flag(char_list[0], "-r", option_R)) {
            --(*argc);
            ++char_list;

            if (*argc == 0 || !check_valid_option(char_list[0])) {
                fprintf(stderr, "NO ARGUMENTS FOR OPTION OR INVALID COMMAND -r\n");
                return ERR_INVALID_COMMAND;
            }
            if ((R = atouint16(char_list[0])) <= 0) {
                return ERR_INVALID_COMMAND;
            }
            ++char_list;
            --(*argc);

        } else if (!strncmp(char_list[0], "--", 2)) {
            ++char_list;
            --(*argc);
            endParse = 1;
        }

        

        
    }

    if (N == 0) {
        N = (W != 0 || R != 0) ? max(R, W) : DKVS_DEFAULT_N;
    }
    if (R == 0) {
        R = min(DKVS_DEFAULT_R, N);
    }
    if (W == 0) {
        W = min(DKVS_DEFAULT_W, N);
    }

    if (R > N || W > N) {
        return ERR_INVALID_COMMAND;
    }

    args->total_servers = N;
    args->get_needed = R;
    args->put_needed = W;
    *rem_argv = char_list;

    return ERR_NONE;
}
