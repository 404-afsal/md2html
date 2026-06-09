#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "parser.h"

mapEntry *table[MAX_BUCKETS];
Stack stack;

void parse_md(FILE *inptr, FILE *outptr)
{

    const char *config_name = "config.txt";

    if (get_tags(config_name) == 1)
    {
        printf("Missing configuration file\n");
        return;
    }

    char line_buf[MAX_LINE_LEN];
    char word[MAX_WORD_LEN];
    mapEntry *t;

    stackinit();

    while (fgets(line_buf, sizeof(line_buf), inptr) != NULL)
    {
        char *ptr = line_buf;
        int bytes_read = 0;
        int word_count = 0;

        while (sscanf(ptr, "%49s%n", word, &bytes_read) == 1)
        {
            if (word_count > 0)
            {
                fprintf(outptr, " "); // Print a separator space ONLY if it's not the first word
            }
            if (stack.top == -1)
            {
                t = search_key(table, word);

                if (t == NULL)
                {
                    fprintf(outptr, "%s", word);
                    word_count++;
                }
                else
                {
                    stack.stack_frame[++(stack.top)].entry = t;
                    if (stack.stack_frame[stack.top].entry->line_status == LINE)
                    {
                        fprintf(outptr, "%s", t->tag[0]);
                        word_count++;
                        stack.stack_frame[stack.top].tag_state = STATE_OPENED;
                    }
                }
            }
            else
            {

                if (stack.stack_frame[stack.top].entry->line_status == LINE)
                {
                    if (stack.stack_frame[stack.top].tag_state == STATE_OPENED)
                    {
                        fprintf(outptr, "%s", word);
                        word_count++;
                        stack.stack_frame[stack.top].tag_state = STATE_CLOSED;
                    }
                }
            }
            ptr += bytes_read;
        }
    }

    t = NULL;
    free_map(table);
}

void stackinit()
{
    stack.top = -1;
    for (int i = 0; i < MAX_S_FRAMES; i++)
    {
        stack.stack_frame[i].entry = NULL;
        stack.stack_frame[i].tag_state = STATE_UNKNOWN;
    }
}

int get_tags(const char *config_name)
{
    FILE *file = fopen(config_name, "r");
    if (file == NULL)
    {
        return 1;
    }

    char buffer[BUF_SIZE];

    // state tracking {0 = not exist, 1 = in the middle of processing, 2 = finished processsing}
    int key_status = 0;
    int val1_status = 0;
    int val2_status = 0;
    bool is_void_status = false;
    bool is_line_status = false;

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0'; // remove '\n' from the buffer

        key_status = 0;
        val1_status = 0;
        val2_status = 0;
        is_void_status = 0;

        char prev = ':';
        char *ptr = buffer;

        char key[MAX_STR_LEN];
        int keyidx = 0;
        char val[2][MAX_STR_LEN];
        int val1_idx = 0;
        int val2_idx = 0;
        Is_Void is_void = NON_VOID; // default
        LineStatus line = LINE;

        while (*ptr != '\0')
        {
            if (*ptr != ':')
            {
                if (prev == ':')
                {
                    if (key_status < 2)
                    {
                        key_status++;
                    }
                    if (key_status == 2)
                    {
                        if (*ptr == 't')
                        {
                            is_void_status = true;
                        }
                        else
                        {
                            is_void_status = false;
                        }
                    }
                    if (key_status == 2 && val1_status)
                    {
                        val1_status++;
                    }
                    if (is_void_status)
                    {
                        if (val1_status == 2)
                        {
                            val2_status++;
                        }
                        if (val2_status == 2)
                        {
                            is_line_status = true;
                        }
                    }
                }

                if (key_status == 1)
                {
                    key[keyidx++] = *ptr;
                    key[keyidx] = '\0';
                }

                if (is_void_status)
                {
                    if (*ptr == 't')
                    {
                        is_void = 1;
                    }
                    else
                    {
                        is_void = 0;
                    }
                }

                if (val1_status == 1)
                {
                    val[0][val1_idx++] = *ptr;
                    val[0][val1_idx] = '\0';
                }

                if (val1_status == 1)
                {
                    val[1][val2_idx++] = *ptr;
                    val[1][val2_idx] = '\0';
                }

                if (is_line_status)
                {
                    line = *ptr;
                }
                else
                {
                    line = 0; // default
                }
            }

            prev = *ptr;
            ptr++;
        }

        if (is_void)
        {
            val[1][val2_idx] = '\0';
        }

        if (key_status == 2 && val1_status == 2)
        {
            config_put(key, val, is_void, line);
        }
    }

    fclose(file);
    return 0;
}

void config_put(char *key, char val[][MAX_STR_LEN], Is_Void is_void, LineStatus line_status)
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
