#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <argp.h>

#include "md-fmt.h"
#include "md-toc.h"

struct argp_option fmt_options[] = {
  { "output",          'o', "FILE",   0, "Output to FILE instead of standard output" },
  { "max-line-length", 'm', "LENGTH", 0, "Try to wrap the line at a blank space before exceeding this LENGTH" },
  { 0 }
};

struct fmt_arguments {
  char *output_file;
  int max_line_length;
  char *input_file;
};

char fmt_doc[] = "Wrap the lines of markdown considering its special elements";

char fmt_args_doc[] = "MARKDOWN-FILE";

error_t fmt_parse_opt (int key, char *arg, struct argp_state *state) {
  struct fmt_arguments *fmt_args = state->input;

  switch (key) {
    case 'o':
      fmt_args->output_file = arg;
      break;
    case 'm':
      fmt_args->max_line_length = atoi(arg);
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num > 1) {
        argp_usage(state);
      }
      fmt_args->input_file = arg;
      state->next = state->argc;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1) {
        argp_usage(state);
      }
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

struct argp fmt_argp = { fmt_options, fmt_parse_opt, fmt_args_doc, fmt_doc };

struct toc_arguments {
  char *input_file;
};

char toc_doc[] = "Generate a table of contents (TOC) from markdown section headers";

char toc_args_doc[] = "MARKDOWN-FILE";

error_t toc_parse_opt (int key, char *arg, struct argp_state *state) {
  struct toc_arguments *toc_args = state->input;

  switch (key) {
    case ARGP_KEY_ARG:
      if (state->arg_num > 1) {
        argp_usage(state);
      }
      toc_args->input_file = arg;
      state->next = state->argc;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1) {
        argp_usage(state);
      }
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

struct argp toc_argp = { NULL, toc_parse_opt, toc_args_doc, toc_doc };

int main (int argc, char **argv) {
  if (
    argc <= 1
    || strcmp(argv[1], "--help") == 0
    || strcmp(argv[1], "-h") == 0
    || strcmp(argv[1], "-?") == 0
  ) {
    int flags = ARGP_HELP_LONG | ARGP_HELP_USAGE | ARGP_HELP_DOC;
    char *program_name = strdup(argv[0]);
    argp_help(&fmt_argp, stderr, flags, strncat(program_name, " fmt", 4));
    free(program_name);
    fputs("\n", stderr);
    program_name = strdup(argv[0]);
    argp_help(&toc_argp, stderr, flags, strncat(program_name, " toc", 4));
    free(program_name);
    if (argc <= 1) {
      return EXIT_FAILURE;
    }
    return 0;
  }
  if (strcmp(argv[1], "fmt") == 0) {
    struct fmt_arguments fmt_args;
    fmt_args.max_line_length = 80;
    fmt_args.output_file = NULL;
    fmt_args.input_file = NULL;
    argp_parse(&fmt_argp, argc-1, argv+1, 0, NULL, &fmt_args);
    FILE *output;
    if (fmt_args.output_file) {
      if (strcmp(fmt_args.output_file, fmt_args.input_file) == 0) {
        error(EXIT_FAILURE, 0, "The output and the input files are the same!");
      }
      output = fopen(fmt_args.output_file, "w");
    } else {
      output = stdout;
    }
    FILE *input = fopen(fmt_args.input_file, "r");
    if (input == NULL) {
      error(EXIT_FAILURE, 0, "It wasn't possible to open input file: %s", fmt_args.input_file);
    }
    if (output == NULL) {
      error(EXIT_FAILURE, 0, "It wasn't possible to open output file: %s", fmt_args.output_file);
    }
    format_markdown(input, output, fmt_args.max_line_length);
    fclose(input);
    fclose(output);
  } else if (strcmp(argv[1], "toc") == 0) {
    struct toc_arguments toc_args;
    toc_args.input_file = NULL;
    argp_parse(&toc_argp, argc-1, argv+1, 0, NULL, &toc_args);
    FILE *input = fopen(toc_args.input_file, "r");
    generate_table_of_contents(input);
    fclose(input);
  } else {
    error(EXIT_FAILURE, 0, "Invalid subcommand!");
  }
  return 0;
}
