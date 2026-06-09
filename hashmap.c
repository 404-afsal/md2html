#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "hashmap.h"

// djb2 hash
int hash_map(char *key)
{
    unsigned long hash = 5381;
    char c;

    while ((c = *key++))
    {
        hash = (hash << 5) + hash + c;
    }

    return hash % MAX_BUCKETS;
}

// return the entry
mapEntry *search_key(mapEntry **table, char *key)
{
    int idx = hash_map(key);
    mapEntry *cursor = table[idx];
    while (cursor != NULL)
    {
        if (cursor->symbol)
        {
            if (strcmp(cursor->symbol, key) == 0)
            {
                return cursor;
            }
        }
        else
        {
            printf("Key doesn't exist\n");
            return NULL;
        }
        cursor = cursor->next;
    }

    return NULL;
}

void free_map(mapEntry **table)
{
    for (int i = 0; i < MAX_BUCKETS; i++)
    {
        if (table[i])
        {
            while (table[i] != NULL)
            {
                mapEntry *temp = table[i]->next;
                free(table[i]->symbol);
                free(table[i]->tag[0]);
                free(table[i]->tag[1]);
                free(table[i]);
                table[i] = temp;
            }
        }
    }
}
