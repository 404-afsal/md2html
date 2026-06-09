#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>

#define MAX_BUCKETS 100
#define MAX_S_FRAMES 5

typedef enum
{
    LINE,
    INLINE
} LineStatus;

typedef enum
{
    VOID,
    NON_VOID
} Is_Void;
typedef struct mapEntry
{
    // key:value; symbol:tag[]
    char *symbol;
    char *tag[2]; // tag[0]: opening tag; tag[1]: closing tag
    Is_Void is_void;
    LineStatus line_status; // 0: line tags; 1: inline tags
    struct mapEntry *next;
} mapEntry;

// for tracking open/close state
typedef enum
{
    STATE_CLOSED,
    STATE_OPENED,
    STATE_UNKNOWN
} TagState;

typedef struct
{
    TagState tag_state;
    mapEntry *entry;
} StackFrame;

typedef struct
{
    StackFrame stack_frame[MAX_S_FRAMES];
    int top;
} Stack;

int hash_map(char *key);
mapEntry *search_key(mapEntry **table, char *key);
void free_map(mapEntry **table);

#endif