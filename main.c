#include <stdio.h>

#include "parser.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Missing input file\n");
        return 1;
    }
    else if (argc > 3)
    {
        printf("Usage: ./md2html input.md\n");
        return 1;
    }

    const char *input_name = argv[1];
    const char *output_name;
    if (argc == 2)
    {
        output_name = "output.html";
    }
    else
    {
        output_name = argv[2];
    }

    FILE *inptr = fopen(input_name, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s\n", input_name);
        return 1;
    }

    FILE *outptr = fopen(output_name, "w");
    if (outptr == NULL)
    {
        printf("Failed to create %s\n", output_name);
        return 1;
    }

    parse_md(inptr, outptr);

    fclose(inptr);
    fclose(outptr);
    return 0;
}