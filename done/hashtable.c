#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "hashtable.h"
#include "error.h"




struct bckt_t{
    kv_pair_t* kv_pair;
    bucket_t* collision;
};

size_t hash_function(dkvs_const_key_t key, size_t size){
    M_REQUIRE(size != 0, SIZE_MAX, "size == %d", 0);
    M_REQUIRE_NON_NULL_CUSTOM_ERR(key, SIZE_MAX);

    size_t hash = 0;
    const size_t key_len = strlen(key);
    for (size_t i = 0; i < key_len; ++i) {
        hash += (unsigned char) key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % size;
}


Htable_t* Htable_construct(size_t size){

    Htable_t* result = malloc(sizeof(Htable_t));
    if (!result)
    {
        fprintf(stderr,"Couldn't allocate memory for Hashtable: Returning NULL\n");
        return NULL;
    }

    result->size = size;
    
    if ((result->content = calloc(size,sizeof(bucket_t)))==NULL)
    {
        fprintf(stderr,"Couldn't allocate memory for the Hashtable content: Returning NULL\n");
        return NULL;
    }

 
    
   
    return result;
}

void Htable_free_content(Htable_t* table){
    free(table->content);
}


void Htable_free(Htable_t** p_table){
    Htable_free_content(*p_table);
    free(*p_table);
    *p_table = NULL;
}


int override_value(bucket_t* bucket, dkvs_const_key_t key,dkvs_const_value_t new_value){

    dkvs_const_key_t key_at_hash = bucket->kv_pair->key; // Current key of the entry
    dkvs_const_value_t value_at_hash = bucket->kv_pair->value; // Current value of the entry

    int overwrite = strncmp(key_at_hash, key, strlen(key) + 1); 

    if (overwrite){
        strncpy(bucket->kv_pair->value,
                new_value, 
                strlen(new_value) + 1);
        return 1;
    }

    return ERR_NONE;
}

int create_bucket(bucket_t* bucket,dkvs_const_key_t key,dkvs_const_value_t value){
    //Build new KV pair
    kv_pair_t* new_pair = malloc(sizeof(kv_pair_t));

    if (new_pair == NULL) {
        fprintf(stderr, "Failed to allocate memory for kv_pair\n");
        return ERR_OUT_OF_MEMORY;
    }
    new_pair->key = key;
    new_pair->value = value;
    
    //Build new Bucket with no overflow 
    bucket->kv_pair = new_pair;
    bucket->collision = NULL;
    return ERR_NONE;
}


int Htable_add_value(Htable_t* table, dkvs_const_key_t key, dkvs_const_value_t value){

    if (table == NULL || value == NULL){
        return ERR_INVALID_ARGUMENT;
    }
     
    size_t hash = hash_function(key,table->size); // Index in the Hash Table
    kv_pair_t* bckt_at_hash = table->content[hash].kv_pair; // KV of the bucket at [hash] index

    //Case 1: The table entry is empty
    if (bckt_at_hash == NULL){
    
        bucket_t* new_bucket = malloc(sizeof(bucket_t));
        create_bucket(new_bucket,key,value);
        
    
        //Assign bucket to [hash] index in the hashtable
        table->content[hash] = *new_bucket; 
        return ERR_NONE;
    }

    //Case 2.0 : The table entry is not empty

    bucket_t first_bucket = table->content[hash];
    bucket_t* new_location = &first_bucket;

    // Go through the chained list to get the first free spot
    while (new_location!=NULL)
    {
        if (override_value(new_location,key,value)){
            return ERR_NONE;
        }
        new_location = new_location->collision;
    }

    if((new_location = malloc(sizeof(bucket_t))) == NULL){
        fprintf(stderr, "Failed to allocate memory for collision bucket\n");
            return ERR_OUT_OF_MEMORY;
    }

    return create_bucket(new_location,key,value);
}


dkvs_value_t Htable_get_value(const Htable_t* table, dkvs_const_key_t key){
    if (table == NULL || key == NULL){
        return NULL;
    }
    
    size_t hash = hash_function(key,table->size); 
    bucket_t* bucket = &table->content[hash];
    do {
        if (bucket->kv_pair != NULL && bucket->kv_pair->key !=NULL)
        {
            if (strncmp(bucket->kv_pair->key, key, strlen(key) + 1) == 0) {
                return bucket->kv_pair->value;
            }
        }
        bucket = bucket->collision;
    } while (bucket != NULL);

    return NULL;
}





