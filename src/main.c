#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

#include "md-fmt.h"
#include "md-toc.h"


int main (int argc, char **args) {
  if (argc == 1) {
    error(3, 0, "Specify a subcommand!");
  }
  if (strcmp(args[1], "--help") == 0 || strcmp(args[1], "-h") == 0) {
    printf("%s fmt [--max-line-length=80] [--output=formated-file.md] markdown-file.md\n", args[0]);
    puts("\tDefault maximum line length: 80 8-bit characters");
    puts("\tDefault output: stdout.");
    puts("\tThe first non-option value ends the argument list.");

    printf("%s toc markdown-file.md\n", args[0]);

    printf("%s [-h|--help]\n", args[0]);
    exit(0);
  } else if (strcmp(args[1], "fmt") == 0) {
    int arg_idx = 1;
    int max_line_length = 80;
    FILE *input_file = NULL;
    FILE *output_file = stdout;
    char *output_filename = NULL;
    while (++arg_idx < argc) {
      char *option = strtok(args[arg_idx], "=");
      if (strcmp(option, "--max-line-length") == 0) {
        char *option_value = strtok(NULL, "");
        sscanf(option_value, "%d", &max_line_length);
      } else if (strcmp(option, "--output") == 0) {
        output_filename = strtok(NULL, "");
      } else if (strcmp(option, "--") == 0) {
        arg_idx++;
        break;
      } else {
        break;
      }
    }
    if (arg_idx == argc) {
      error(6, 0, "Pass an input file!");
    }
    if (strcmp(args[arg_idx], output_filename) == 0) {
      error(8, 0, "The output and the input files are the same!");
    }
    input_file = fopen(args[arg_idx], "r");
    if (output_filename != NULL) {
      output_file = fopen(output_filename, "w");
    }
    if (input_file == NULL) {
      error(4, 0, "It wasn't possible to open input file: %s", args[arg_idx]);
    }
    if (output_file == NULL) {
      error(5, 0, "It wasn't possible to open output file: %s", output_filename);
    }
    format_markdown(input_file, output_file, max_line_length);
    fclose(input_file);
    fclose(output_file);
  } else if (strcmp(args[1], "toc") == 0) {
    if (argc == 2) {
      error(7, 0, "Pass an input file!");
    }
    FILE *input_file = fopen(args[2], "r");
    generate_table_of_contents(input_file);
    fclose(input_file);
  } else {
    error(2, 0, "Invalid command!");
  }
}
