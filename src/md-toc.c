#include <stdio.h>
#include <ctype.h>

#include "md-toc.h"
#include "token_types.h"
#include "../build/lex.yy.h"


void generate_table_of_contents (FILE *input_file) {
    yyin = input_file;
    TOKEN_TYPE token_type;
    int line_count = 1;
    fputs("**Table of Contents**\n\n", stdout);
    while ((token_type = yylex()) != END_OF_SCAN) {
        if (token_type == TOKEN_NEWLINE || token_type == TOKEN_CODE_BLOCK_NEWLINE) {
            line_count += yyleng;
        }
        else if (token_type == TOKEN_HEADER) {
            int header_level = 1;
            char *text = yytext;
            while (*text++ != '#') {}
            // Skipped one "#" header indicator. For that header level begins in 1.
            while (*text++ == '#') {
                header_level++;
            }
            // Skip the check of a least one character after the "#" header indicator
            // because there must be some blank space.
            while (isblank(*text++)) {}
            for (int i = 1; i < header_level; i++) {
                fputs("\t", stdout);
            }
            fprintf(stdout, "- %s (line %d)\n", --text, line_count);
        }
    }
}
