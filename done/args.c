#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "args.h"
#include "util.h"



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

int check_valid_flag(char* str, const char* option, int valid){
    return !strncmp(str,option,strlen(option)) && valid;
}

int parse_opt_args(args_t *args, size_t supported_args, int *argc, char ***rem_argv) {
    if (!args || !argc || !rem_argv) {
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

            
            if (*argc == 0 || !char_list[0] || (N = atouint32(char_list[0])) == 0) {
                return ERR_INVALID_COMMAND;
            }
            ++char_list;
            --(*argc);
            
        } 
        else if (check_valid_flag(char_list[0], "-w", option_W)) {
            --(*argc);
            ++char_list;
           
            if (*argc == 0 || !char_list[0] ||  (W = atouint32(char_list[0])) == 0) {
                return ERR_INVALID_COMMAND;
            }
            ++char_list;
            --(*argc);

        } 
        else if (check_valid_flag(char_list[0], "-r", option_R)) {
            --(*argc);
            ++char_list;
            
            (R = atouint32(char_list[0]));
            if (*argc == 0 || !char_list[0] ||  R == 0 || R >= 255) {
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
        N = (W != 0 || R != 0) ? MAX(R, W) : DKVS_DEFAULT_N;
    }
    if (R == 0) {
        R = MIN(DKVS_DEFAULT_R, N);
    }
    if (W == 0) {
        W = MIN(DKVS_DEFAULT_W, N);
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
