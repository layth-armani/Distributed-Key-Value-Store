#pragma once

/**
 * @file network.h
 * @brief Every network_* operation
 *
 * @author Valérian Rousset
 */

#include "client.h"
#include "hashtable.h"

/**
 * @brief get a value from the network
 * @param client client to use
 * @param key key of what we want to find value
 * @param value value to write to, to be allocated by network_get()
 * @return an error code
 */
int network_get(const client_t* client, dkvs_const_key_t key, dkvs_value_t* value);

/**
 * @brief put a value in the network
 * @param client client to use
 * @param key key to retrieve value later
 * @param value value to add
 * @return an error code
 */
int network_put(const client_t* client, dkvs_const_key_t key, dkvs_const_value_t value);

/**
 * @brief delete a key in the network
 * @param client client to use
 * @param key key to delete
 * @return an error code
 */
int network_del(const client_t* client, dkvs_const_key_t key);
