#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ds_handle.h"
#include "parser.h"

mapEntry *table[MAX_BUCKETS];
TagStack stack;

void parse_md(FILE *inptr, FILE *outptr)
{

    const char *config_name = "config.txt";

    if (get_tags(config_name) == 1)
    {
        printf("Missing configuration file\n");
        return;
    }

    char line_buff[MAX_LINE_LEN];

    while (fgets(line_buff, sizeof(line_buff), inptr) != NULL)
    {
        char *ptr = line_buff;

        char *event_ptr;
        Tickets ts;
        tickets_init(&ts);

        int table_idx = 0;
        while (table_idx < MAX_BUCKETS) // Is the current idx exist
        {
            if (table[table_idx])
            {
                mapEntry *curr_entry = table[table_idx];
                while (curr_entry != NULL) // Is the current entry in the current idx exist
                {
                    // Find all the keys in the input line
                    char *key = table[table_idx]->symbol;
                    while ((event_ptr = strstr(ptr, key)) != NULL)
                    {
                        if (ts.size >= ts.cap)
                        {
                            tickets_resize(&ts);
                        }
                        ts.tickets[ts.size].entry = table[table_idx];
                        ts.tickets[ts.size].line_ptr = event_ptr;
                        ts.size++;

                        ptr = event_ptr + strlen(key);
                    }
                    curr_entry = curr_entry->next;
                }
            }

            table_idx++;
        }

        // Shift the multi-symbol ticket array into a singular ordered timeline
        qsort(ts.tickets, ts.size, sizeof(EventTicket), compare_tickets);

        char *current = line_buff;
        if (*current == '\n' || *current == '\0')
        {
            fprintf(outptr, "\n");
            continue;
        }

        stackinit(&stack, ts.size);

        int t_idx = 0;
        while (t_idx < ts.size)
        {
            int text_len = ts.tickets[t_idx].line_ptr - current;
            fprintf(outptr, "%.*s", text_len, current);
            current = ts.tickets[t_idx].line_ptr;

            // Print the appropriate tag/tags
            if (stack.top != -1)
            {
                if (peek(&stack, ts.tickets[t_idx].entry))
                {
                    stack.frame[stack.top].tag_state = STATE_CLOSED;
                }
                else
                {
                    // Push
                    push(&stack, ts.tickets[t_idx].entry);
                }
            }
            else
            {
                // Push
                push(&stack, ts.tickets[t_idx].entry);
            }

            if (stack.frame[stack.top].tag_state == STATE_CLOSED)
            {
                fprintf(outptr, "%s", stack.frame[stack.top].entry->tag[1]);
                current += strlen(stack.frame[stack.top].entry->symbol);

                pop(&stack);
            }
            else
            {
                fprintf(outptr, "%s", stack.frame[stack.top].entry->tag[0]);
                if (stack.frame[stack.top].entry->is_void == VOID)
                {
                    current += strlen(stack.frame[stack.top].entry->symbol);
                    pop(&stack);
                }
                else
                {
                    stack.frame[stack.top].tag_state = STATE_CLOSED;
                    current += strlen(stack.frame[stack.top].entry->symbol);
                }
            }
            t_idx++;
        }
        if (stack.top != -1)
        {
            char *next_nl = strchr(current, '\n');
            int len = next_nl - current;
            if (len)
            {
                fprintf(outptr, "%.*s%s", len, current, stack.frame[stack.top].entry->tag[1]);
            }
            else
            {
                fprintf(outptr, "%s%s", current, stack.frame[stack.top].entry->tag[1]);
            }
            current += len;

            pop(&stack);
        }
        if (*current)
        {
            fprintf(outptr, "%s", current);
        }
        // Free stack & tickets
        free_stack(&stack);
        free_tickets(&ts);
    }

    free_map(table);
}

int get_tags(const char *config_name)
{
    FILE *file = fopen(config_name, "r");
    if (file == NULL)
    {
        return 1;
    }

    char buffer[BUF_SIZE];

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0'; // remove '\n' from the buffer
        char *ptr = buffer;
        int bytes_read;

        char key[MAX_KEY_LEN];
        char val[2][MAX_VAL_LEN];
        Is_Void is_void;
        char is_void_str[32];
        LineStatus line;
        char line_str[32];

        sscanf(ptr, "%[^:]::%n", is_void_str, &bytes_read);
        ptr += bytes_read;
        if (strcmp(is_void_str, "VOID") == 0)
        {
            is_void = VOID;
        }
        else
        {
            is_void = NON_VOID; // default
        }

        if (is_void == VOID)
        {
            sscanf(ptr, "%[^:]::%[^:]::%s", key, val[0], line_str);
        }
        else
        {
            sscanf(ptr, "%[^:]::%[^:]::%[^:]::%s", key, val[0], val[1], line_str);
        }

        if (strcmp(line_str, "INLINE") == 0)
        {
            line = INLINE;
        }
        else
        {
            line = LINE; // default
        }

        if (strlen(key) > 0 && strlen(val[0]) > 0)
        {
            config_put(key, val, is_void, line);
        }
    }

    fclose(file);
    return 0;
}

void config_put(char *key, char val[][MAX_VAL_LEN], Is_Void is_void, LineStatus line_status)
{
    int idx = hash_map(key);

    mapEntry *new_entry = malloc(sizeof(mapEntry));
    new_entry->symbol = strdup(key);
    new_entry->tag[0] = strdup(val[0]);
    new_entry->tag[1] = strdup(val[1]);
    new_entry->is_void = is_void;
    new_entry->line_status = line_status;

    // if an entry already exists
    if (table[idx] != NULL)
    {
        new_entry->next = table[idx];
    }
    else
    {
        new_entry->next = NULL;
    }

    table[idx] = new_entry;
}
