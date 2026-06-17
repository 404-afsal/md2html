#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds_handle.h"

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

void stackinit(TagStack *stack, int size)
{
    stack->top = -1;
    stack->size = size;
    stack->frame = malloc(sizeof(StackFrame) * stack->size);
    for (int i = 0; i < size; i++)
    {
        stack->frame[i].entry = NULL;
        stack->frame[i].tag_state = STATE_UNKNOWN;
    }
}

void push(TagStack *stack, mapEntry *entry)
{
    stack->top++;
    stack->frame[stack->top].entry = entry;
    stack->frame[stack->top].tag_state = STATE_OPENED;
}

bool peek(const TagStack *stack, mapEntry *entry)
{
    if ((uintptr_t)stack->frame[stack->top].entry == (uintptr_t)entry)
    {
        return true;
    }
    return false;
}

void pop(TagStack *stack)
{
    stack->top--;
}

void free_stack(TagStack *stack)
{
    free(stack->frame);
}

void tickets_init(Tickets *t)
{
    t->size = 0;
    t->cap = 1;
    t->tickets = malloc(sizeof(EventTicket) * t->cap);
}

void tickets_resize(Tickets *t)
{
    t->cap *= 2;
    t->tickets = realloc(t->tickets, sizeof(EventTicket) * t->cap);
}

int compare_tickets(const void *a, const void *b)
{
    // qsort points to the structure slots, so 'a' and 'b' are pointers to 'Tickets'
    const EventTicket *ticket_a = (const EventTicket *)a;
    const EventTicket *ticket_b = (const EventTicket *)b;

    // Convert the memory addresses inside the char* pointers into numbers
    uintptr_t addr_a = (uintptr_t)(ticket_a->line_ptr);
    uintptr_t addr_b = (uintptr_t)(ticket_b->line_ptr);

    // Compare the numerical addresses safely
    if (addr_a < addr_b)
        return -1;
    if (addr_a > addr_b)
        return 1;
    return 0;
}

void free_tickets(Tickets *t)
{
    free(t->tickets);
}
