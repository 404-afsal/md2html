#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdio.h>

#include "hashmap.h"

#define BUF_SIZE 256
#define MAX_LINE_LEN 10000
#define MAX_STR_LEN 20
#define MAX_WORD_LEN 50

void stackinit();
void parse_md(FILE *inptr, FILE *outptr);
int get_tags(const char *config_name);
void config_put(char *key, char val[][MAX_STR_LEN], Is_Void is_void, LineStatus line_status);

#endif