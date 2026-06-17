#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdio.h>

#include "ds_handle.h"

#define BUF_SIZE 256
#define MAX_LINE_LEN 10000
#define MAX_KEY_LEN 20
#define MAX_VAL_LEN 30
#define MAX_WORD_LEN 50

void parse_md(FILE *inptr, FILE *outptr);
int get_tags(const char *config_name);
void config_put(char *key, char val[][MAX_VAL_LEN], Is_Void is_void, LineStatus line_status);

#endif