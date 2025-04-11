#include <errno.h>
#include <stdlib.h>
#include "util.h"
#include "dkvs-client-cmds.h"
#include "network.h"
#include "config.h"

int64_t extract_negative_number(const char *str) {
    if (str == NULL) {
        errno = EINVAL;
        return 0;
    }
    int is_negative = (str[0] == '-');
    const char *num_str = (is_negative) ? str + 1 : str;

    uint64_t abs_value = atouint64(num_str);
    if (errno == ERANGE) {
        return 0;
    }
    if (is_negative) {
        if (abs_value > (uint64_t)INT64_MAX + 1) {
            errno = ERANGE;
            return 0;
        }
        return -(int64_t)abs_value;
    }

    return (int64_t)abs_value;
}


int cli_client_substr(client_t *client, int argc, char **argv){
    if (argv == NULL || argv[0] == NULL || argv[1] == NULL || argv[2] == NULL || argv[3] == NULL || argc != 4) {
        return ERR_INVALID_COMMAND;
    }
    if(client == NULL || strlen(argv[0]) > MAX_MSG_ELEM_SIZE || strlen(argv[3]) > MAX_MSG_ELEM_SIZE){
        return ERR_INVALID_ARGUMENT;
    }

    dkvs_const_key_t origin_key = argv[0];
    int64_t pos = extract_negative_number(argv[1]);
    uint64_t length = atouint64(argv[2]);
    dkvs_const_key_t destination_key = argv[3];

    if (errno == ERANGE || length < 0 || length > MAX_MSG_ELEM_SIZE){
        return ERR_INVALID_COMMAND;
    }

    dkvs_value_t* origin_val = calloc(1, sizeof(origin_val));
    if(origin_val==NULL){
        return ERR_OUT_OF_MEMORY;
    }

    int ret = network_get(client, origin_key, origin_val);
    if(ret != ERR_NONE){
        free(*origin_val);
        free(origin_val);
        return ret;
    }

    size_t origin_val_len = strlen(*origin_val);    
    if (pos < 0)pos = origin_val_len + pos;

    if (pos < 0 || (pos + length > origin_val_len) || length > origin_val_len) {        
        free(*origin_val);
        free(origin_val);
        return ERR_INVALID_COMMAND;
    }

    dkvs_value_t destination_val = calloc(length + 1, sizeof(char));
    if(destination_val==NULL){
        free(*origin_val);
        free(origin_val);
        return ERR_OUT_OF_MEMORY;
    }
    
    int counter = 0;
    while (counter < length) {
        destination_val[counter] = (*origin_val)[pos];
        counter++;
        pos++;
    }
    destination_val[counter] = '\0';

    ret = network_put(client, destination_key, destination_val);

    free(*origin_val);
    free(origin_val);
    free(destination_val);

    return ret;
}


