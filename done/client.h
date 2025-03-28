#pragma once

/**
 * @file client.h
 * @brief Client definition and related functions
 *
 * @author Valérian Rousset
 */

#include <stddef.h> // for size_t

#include "args.h"
#include "error.h"
#include "ring.h"

/**
 * @brief client state
 */
struct client
{
    args_t* args;
    ring_t* ring;
};

typedef struct client client_t;

/**
 * @brief does all the work to be done at the end of life of a client
 * @param client the client to end
 */
void client_end(client_t *client);

/**
 * @brief does all the work to be done at the beginning of life of a client
 * @param client the client to initialize
 * @return some error code, if any.
 */
int client_init(client_t *client,
                size_t supported_args,
                int *argc,
                char ***argv);
