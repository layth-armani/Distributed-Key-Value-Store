#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"
#include "error.h"


Htable_t* Htable_construct(size_t size){

    Htable_t* result = malloc(sizeof(Htable_t));
    if (!result)
    {
        fprintf(stderr,"Couldn't allocate memory for Hashtable: Returning NULL\n");
        return NULL;
    }

    result->size = size;
    
    if ((result->content = calloc(size,sizeof(bucket_t*)))==NULL)
    {
        fprintf(stderr,"Couldn't allocate memory for the Hashtable content: Returning NULL\n");
        return NULL;
    }
    return result;
}

void Htable_free_content(Htable_t* table){

    for (size_t i = 0; i < table->size; i++)
    {
        free(table->content[i]);
    }
    free(table->content);
}


void Htable_free(Htable_t** p_table){
    Htable_free_content(*p_table);
    free(*p_table);
    *p_table = NULL;
}

int Htable_add_value(Htable_t* table, dkvs_const_key_t key, dkvs_const_value_t value);

