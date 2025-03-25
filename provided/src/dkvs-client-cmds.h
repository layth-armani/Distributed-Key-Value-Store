#pragma once

/**
 * @file dkvs-client-cmds.h
 * @brief Client CLI commands
 *
 * @author J.-C. Chappelier
 */

#include "client.h"

/**
 * @brief performs a get to the DKVS store for a client
 * @param client the client asking for the get
 * @param argc the number of available arguments in `argv`
 * @param argv the array of available arguments
 * @return an error code
 */
int cli_client_get(client_t *client, int argc, char **argv);

/**
 * @brief performs a put to the DKVS store for a client
 * @param client the client asking for the put
 * @param argc the number of available arguments in `argv`
 * @param argv the array of available arguments
 * @return an error code
 */
int cli_client_put(client_t *client, int argc, char **argv);

/**
 * @brief performs a concatenation of values
 * @param client the client asking for the concatenation
 * @param argc the number of available arguments in `argv`
 * @param argv the array of available arguments
 * @return an error code
 */
int cli_client_cat(client_t *client, int argc, char **argv);

/**
 * @brief performs a value substring extraction
 * @param client the client asking for the substring
 * @param argc the number of available arguments in `argv`
 * @param argv the array of available arguments
 * @return an error code
 */
int cli_client_substr(client_t *client, int argc, char **argv);

/**
 * @brief performs a value search
 * @param client the client asking for the search
 * @param argc the number of available arguments in `argv`
 * @param argv the array of available arguments
 * @return an error code
 */
int cli_client_find(client_t *client, int argc, char **argv);
