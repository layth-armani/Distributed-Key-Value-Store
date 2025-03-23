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

    if ((key == NULL) || (size == 0)){
        return SIZE_MAX;
    }

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


void kv_pair_free(kv_pair_t *kv){
    free(kv->key);
    free(kv->value);
    free(kv);
}


void Htable_free_content(Htable_t* table){
    for (size_t i = 0; i < table->size; i++)
    {
        bucket_t* b = table->content[i].collision; 

        while (b != NULL)
        {
            bucket_t* next = b->collision; 
            if (b->kv_pair != NULL)
            {
                kv_pair_free(b->kv_pair);
            }
            free(b);
            b = next;
        }

      
        if (table->content[i].kv_pair != NULL)
        {
            kv_pair_free(table->content[i].kv_pair);
        }

    }
}
    

void Htable_free(Htable_t** p_table){
    Htable_free_content(*p_table);
    free((*p_table)->content);
    free(*p_table);
    *p_table = NULL;
}

/**
 * Overrides the value of an existing key in a bucket.
 * @param bucket The bucket to modify.
 * @param key The key to check.
 * @param new_value The new value to set.
 * @return 1 if overridden, 0 otherwise.
 */


int override_value(bucket_t* bucket, dkvs_const_key_t key,dkvs_const_value_t new_value){

    dkvs_key_t key_at_hash = bucket->kv_pair->key; // Current key of the entry
    dkvs_value_t value_at_hash = bucket->kv_pair->value; // Current value of the entry

    //printf("VALUE TO OVERRIDE: \"%s\"\n",value_at_hash);



    int overwrite = strncmp(key_at_hash, key, strlen(key) + 1) == 0; 

    if (overwrite){

        free(value_at_hash);

        bucket->kv_pair->value = calloc(strlen(new_value)+1,sizeof(char));
        
        strncpy(bucket->kv_pair->value,
                new_value, 
                strlen(new_value) + 1);
        
        return 1;
    }

    return 0;
}

/**
 * Creates a new bucket with a key-value pair.
 * @param bucket The bucket to initialize.
 * @param key The key for the pair.
 * @param value The value for the pair.
 * @return ERR_NONE on success, ERR_OUT_OF_MEMORY on failure.
 */

int create_bucket(bucket_t* bucket,dkvs_const_key_t key,dkvs_const_value_t value){
    //Build new KV pair
    
    
    //printf("Creating bucket at: %p\n",bucket);

    kv_pair_t* new_pair = calloc(1,sizeof(kv_pair_t));
    new_pair->key = calloc(strlen(key)+1,sizeof(char));
    new_pair->value = calloc(strlen(value)+1,sizeof(char));
    

    if (new_pair == NULL || new_pair->key == NULL || new_pair->value == NULL) {
        fprintf(stderr, "Failed to allocate memory for kv_pair\n");
        return ERR_OUT_OF_MEMORY;
    }

    strncpy(new_pair->key,key,strlen(key)+1);
    strncpy(new_pair->value,value,strlen(value)+1);
      
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
    bucket_t* bckt_at_hash = table->content + hash; //Bucket at [hash] index

    //Case 1: The table entry is empty
    if (bckt_at_hash->kv_pair == NULL){ 
        create_bucket(bckt_at_hash, key, value);
        return ERR_NONE;
    }

    //Case 2.0 : The table entry is not empty
    //printf("Accessing bucket at: %p\n",bckt_at_hash);

    // Go through the chained list to get the first free spot
    while (bckt_at_hash!=NULL)
    {
        //printf("KEY OF BUCKET: \"%s\"\n",bckt_at_hash->kv_pair->key);
        //printf("VALUE OF BUCKET: \"%s\"\n",bckt_at_hash->kv_pair->value);
        if (override_value(bckt_at_hash,key,value)){
            return ERR_NONE;
        }
        bckt_at_hash = bckt_at_hash->collision;
    }


    bckt_at_hash = calloc(1,sizeof(bucket_t));

    return create_bucket(bckt_at_hash,key,value);
}

//WARNING : CALLER SHOULD FREE THE RETURN VALUE

dkvs_value_t Htable_get_value(const Htable_t* table, dkvs_const_key_t key){
    if (table == NULL || key == NULL){
        return NULL;
    }
    
    size_t hash = hash_function(key,table->size); 
    bucket_t* bucket = table->content + hash;
    //printf("GET_VALUE AT %p\n",bucket);
    do {
        if (bucket->kv_pair != NULL && bucket->kv_pair->key !=NULL)
        {
            if (strncmp(bucket->kv_pair->key, key, strlen(key) + 1) == 0) {
                dkvs_value_t result = calloc(strlen(bucket->kv_pair->value)+1,sizeof(char));
                strncpy(result,bucket->kv_pair->value,strlen(bucket->kv_pair->value)+1);
                return result;
            }
        }
        bucket = bucket->collision;
    } while (bucket != NULL);

    return NULL;
}


void Htable_print(const Htable_t* table) {
    if (table == NULL) {
        printf("Hashtable is NULL.\n");
        return;
    }

    for (size_t i = 0; i < table->size; i++) {
        bucket_t* bucket = &table->content[i];
        printf("Bucket %zu: ", i);

        if (bucket->kv_pair == NULL) {
            printf("Empty\n");
        }
        
        else{
        while (bucket != NULL) {
            if (bucket->kv_pair != NULL) {
                printf("[Key: %s, Value: %s] -> ", bucket->kv_pair->key, bucket->kv_pair->value);
            }
            bucket = bucket->collision;
        }
        printf("NULL\n");
    }
    }
}





