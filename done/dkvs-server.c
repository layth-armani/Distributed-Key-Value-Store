/**
 * @file dkvs-server.c
 * @brief server waiting for requests from the DKVS clients
 *
 */

#include "error.h"
#include "config.h" // MAX_MSG_SIZE
#include "hashtable.h"
#include "socket_layer.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define HTABLE_SIZE 256

pthread_mutex_t mutex;




// ======================================================================
static int server_get(int fd, dkvs_const_key_t key,
                      const struct sockaddr_in *client, const Htable_t* table)
{
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(table);

    ssize_t ret = 0;
    char buffer[MAX_MSG_SIZE];
    
    size_t to = 0;
    size_t from = 0;

    if (strlen(key)==0)
    {
        while (to < table->size)
        {
            int dump = Htable_dump(table, from, &to, buffer, MAX_MSG_SIZE);
            from = to;

            if (dump!=ERR_NONE)
            {
                return dump;
            }
            
            ret = udp_send(fd,buffer, MAX_MSG_SIZE, client);
        }

        return ret < 0 ? (int)ret : ERR_NONE;
    }

    dkvs_const_value_t value = Htable_get_value(table, key);
    
    if(value == NULL){
        debug_printf("value not found for server get for key \"%s\" \n", key);
        ret = udp_send(fd, "\0", 1, client);
    }
    else{
        debug_printf("server get for key \"%s\" has value \"%s\" \n", key, value);
        ret = udp_send(fd, value, strlen(value), client);
        free((void*)value);
    }

    return ret < 0 ? (int) ret : ERR_NONE;
}

// ======================================================================
static int server_put(int fd, dkvs_const_key_t key, dkvs_const_value_t value,
                      const struct sockaddr_in *client, Htable_t* table)
{
    M_REQUIRE_NON_NULL(key);
    M_REQUIRE_NON_NULL(value);
    M_REQUIRE_NON_NULL(client);
    M_REQUIRE_NON_NULL(table);

    debug_printf("server put for \"%s\" --> \"%s\":\n", key, value);

   
    ssize_t ret = 0;

    int err = 0;
    if( strlen(key) !=0 && (err = Htable_add_value(table, key, value)) == ERR_NONE){
        debug_printf("success %s\n", "");
        ret = udp_send(fd, "\0", 1, client);
    }
    else {
        debug_printf("failure %s\n", "");
        ret = udp_send(fd, "\1", 0, client); 
    }
    
    return ret < 0 ? (int) ret : ERR_NONE;
}

// ======================================================================
static int out(int error_code)
{
    if (error_code != ERR_NONE) {
        fprintf(stderr, "ERROR: %s\n", ERR_MSG(error_code));
    }
    return error_code;
}

int mutex_init(int type, int robust){
    int err1 = 0, err2 = 0;
    pthread_mutexattr_t mutex_attribute;
    pthread_mutexattr_init(&mutex_attribute);

    if (type)
    {
        err1 = pthread_mutexattr_settype(&mutex_attribute,type);
    }
    
    if (robust)
    {
        err2 = pthread_mutexattr_setrobust(&mutex_attribute, robust);
    }

    return (err1 || err2) ? ERR_THREADING : pthread_mutex_init(&mutex, &mutex_attribute);
}

// ======================================================================
int main(int argc, char **argv)
{
    int err = ERR_NONE;
    int t = 0;


    // Create and set options for mutex variable
    #if defined(PTHREAD_MUTEX_NORMAL) && defined(PTHREAD_MUTEX_ROBUST)
    if (mutex_init(PTHREAD_MUTEX_NORMAL, PTHREAD_MUTEX_ROBUST))
    {
        return ERR_THREADING;
    }
    #else 
    mutex_init(0,0);
    #endif

    // usage: prog <IP> <port> [<key> <value> ...]
    if ((argc < 3) || (argc % 2 == 0)) return out(ERR_INVALID_COMMAND);

    ++argv;
    --argc; 

    const char* ip = argv[0];

    // --------------- Get port number ---------------
    ++argv;
    --argc;

    const char *port_str = argv[0];
    char *endptr;

    // Convert string to unsigned long
    uint16_t port = (uint16_t)strtoul(port_str, &endptr, 10);

    // --------------- Lauch UDP server ---------------
    int fd = udp_server_init(ip, port, t);
    debug_printf("Server listening on %s:%d\n", ip, port);

    // --------------- Init Hash table ---------------
    Htable_t* table = Htable_construct(HTABLE_SIZE);
    if (table == NULL) {
        // maybe add something here (or simply remove that comment)...
        return out(ERR_OUT_OF_MEMORY);
    }

    // ...to be continued week 11...
    ++argv;
    --argc;



    while (argc > 1) // Ensure at least key and value remain
    {
        
        char key[MAX_MSG_ELEM_SIZE];
        char value[MAX_MSG_ELEM_SIZE];

        strncpy(key, argv[0], MAX_MSG_ELEM_SIZE - 1);
        key[MAX_MSG_ELEM_SIZE - 1] = '\0';
        strncpy(value, argv[1], MAX_MSG_ELEM_SIZE - 1);
        value[MAX_MSG_ELEM_SIZE - 1] = '\0';

        err = Htable_add_value(table, key, value);

        if (err != ERR_NONE)
        {
            return ERR_INVALID_COMMAND;
        }

        argv += 2;
        argc -= 2;
    }

    // --------------- Listening loop ---------------
    while (err == ERR_NONE) {
        
        char* buffer = calloc(MAX_MSG_SIZE, sizeof(char));
        if (!buffer)
        {
            return ERR_OUT_OF_MEMORY;
        }
        
        

        struct sockaddr_in address;
        err = get_server_addr(ip, port, &address);
        if (err != ERR_NONE){
            free(buffer);
            return out(err);
        }
        

        ssize_t bytes = udp_read(fd, buffer, MAX_MSG_SIZE, &address);
        debug_printf("Received: \"%s\" (size: %ld)\n", buffer, bytes);
        if(bytes < 0){
            free(buffer);
            return out(err);
        }


        if (memchr(buffer, '\0', (size_t)bytes) != NULL)    
        {
            err = server_put(fd, buffer, buffer + strlen(buffer) + 1, &address, table);
        }
        else if (buffer[0]  == '\0' && bytes != 0) {
            err = ERR_NOT_FOUND;
        } else {
            err = server_get(fd, buffer, &address, table);
        }   

        free(buffer);

    }

    // --------------- Garbage collecting ---------------
    if (err != ERR_NONE) {        
        Htable_free(&table);
    }

    return out(err);
}
