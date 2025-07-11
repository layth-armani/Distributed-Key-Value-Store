#pragma once

/**
 * @file config.h
 * @brief Contains useful macro definitions (const parameters, mainly)
 *
 * @author Valérian Rousset
 */

#include "util.h" // for STR()

/**
 * @brief maximum number of pending connections
 */
#define MAX_PENDING_CONNECTIONS 5

/**
 * @brief working on localhost by default
 */
#define DKVS_DEFAULT_IP "127.0.0.1"

/**
 * @brief default port for UDP communication
 */
#define DKVS_DEFAULT_PORT 1234

/**
 * @brief local file for list of servers
 */
#define DKVS_SERVERS_LIST_FILENAME "servers.txt"

/**
 * @brief maximum number of bytes in a key or a value in messages without '\0'
 */
#define MAX_MSG_ELEM_SIZE 32753

/**
 * @brief string conversion for scanf for a MAX_MSG_ELEM
 */
#define XSTR(s) STR(s)
#define MAX_MSG_ELEM_SCANF "%" XSTR(MAX_MSG_ELEM_SIZE) "s"

/**
 * @brief maximum number of bytes in a message
 */
#define MAX_MSG_SIZE (MAX_MSG_ELEM_SIZE * 2 + 1)
