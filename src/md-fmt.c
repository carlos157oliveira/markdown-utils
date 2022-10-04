#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <error.h>

#include "md-fmt.h"
#include "token_types.h"
#include "../build/lex.yy.h"

typedef int bool;
#define true 1
#define false 0

typedef struct {
  TOKEN_TYPE type;
  char *text;
  // column at which the token ends
  int end_col;
} TOKEN;

typedef struct node {
  TOKEN *token;
  struct node *next;
} NODE;

FILE *output;

// Modify string in place.
// Returns pointer to the begining of string.
// Cannot be used on read-only strings.
char *trim (char *string, int length) {
  char *string_end = string + length;
  char *pos;
  char *end_pos;
  
  for (pos = string; pos < string_end && isblank(*pos); pos++) {}
  for (end_pos = string_end - 1; end_pos >= string && end_pos >= pos && isblank(*end_pos); end_pos--) {}
  *++end_pos = '\0';
  return pos;
}

bool is_code_block (TOKEN_TYPE type) {
  // The first type at the enum referring to code blocks.
  // Values bigger than it also refer to code blocks
  // because of the disposition at the enum TOKEN_TYPE.
  return type >= TOKEN_BEGIN_CODE_BLOCK_OK ? true : false;
}

void process_code_block (TOKEN* token, int text_length) {
  switch (token->type) {
    case TOKEN_BEGIN_CODE_BLOCK_OK:
    case TOKEN_END_CODE_BLOCK_OK:
      fputs(trim(token->text, text_length), output);
      break;
    case TOKEN_BEGIN_CODE_BLOCK_LEFT_UNPAIRED:
    case TOKEN_END_CODE_BLOCK_LEFT_UNPAIRED:
      fprintf(output, "\n%s", trim(token->text, text_length));
      break;
    case TOKEN_BEGIN_CODE_BLOCK_RIGHT_UNPAIRED:
    case TOKEN_END_CODE_BLOCK_RIGHT_UNPAIRED:
      fprintf(output, "%s\n", trim(token->text, text_length));
      break;
    case TOKEN_BEGIN_CODE_BLOCK_UNPAIRED:
    case TOKEN_END_CODE_BLOCK_UNPAIRED:
      fprintf(output, "\n%s\n", trim(token->text, text_length));
      break;
    case TOKEN_CODE_BLOCK:
    case TOKEN_CODE_BLOCK_NEWLINE:
      fputs(token->text, output);
      break;
    default:
      error(1, 0, "Invalid token for code block!");
  }
}

int flush_tokens_list (NODE *tokens_list, NODE **current_node, NODE **breakpoint, char *current_indentation, int line_pos) {
  NODE *node;
  while ((node = tokens_list->next) != NULL) {
    if (node == *breakpoint) {
      fputc('\n', output);
      line_pos -= node->token->end_col;
      // Avoid inserting indentation in an empty line.
      if (current_indentation != NULL && (node->next == NULL || node->next->token->type != TOKEN_NEWLINE)) {
        fputs(current_indentation, output);
        line_pos += strlen(current_indentation);
      }
    } else {
      fputs(node->token->text, output);
    }
    NODE *tmp = node->next;
    free(node->token->text);
    free(node->token);
    free(node);
    tokens_list->next = tmp;
  }
  *breakpoint = NULL;
  *current_node = tokens_list;
  return line_pos;
}

void format_markdown (FILE *input_file, FILE *output_file, int max_line_length) {
  yyin = input_file;
  output = output_file;
  NODE *breakpoint = NULL;
  NODE *tokens_list = (NODE *) malloc(sizeof(NODE));
  NODE *current_node = tokens_list;
  int line_pos = 0;
  char *current_indentation = NULL;
  
  TOKEN_TYPE token_type;
  while ((token_type = yylex()) != END_OF_SCAN) {
    TOKEN *token = malloc(sizeof(TOKEN));
    token->type = token_type;
    token->text = strdup(yytext);

    // Verify indentation at the start of a line to insert it
    // in the case it is broken in more parts.
    if (token_type == TOKEN_LINE_INDENTATION) {
      free(current_indentation);
      current_indentation = strdup(yytext);
    } else if (token_type == TOKEN_ITEM) {
      free(current_indentation);
      char *pos;
      for (pos = token->text; isblank(*pos); pos++) {}
      current_indentation = strndup(token->text, pos - token->text);
    }

    if (is_code_block(token_type)) {
      flush_tokens_list(tokens_list, &current_node, &breakpoint, current_indentation, line_pos);
      line_pos = 0;
      do {
        process_code_block(token, yyleng);
        token_type = yylex();
        token->type = token_type;
        token->text = strdup(yytext);
      } while(is_code_block(token_type));
      if (token_type == END_OF_SCAN) {
        break;
      }
    }
    
    NODE *next_node = (NODE *) malloc(sizeof(NODE));
    current_node->next = next_node;
    current_node = next_node;
    current_node->next = NULL;

    if (token_type == TOKEN_BREAKPOINT && (line_pos <= max_line_length || breakpoint == NULL)) {
      breakpoint = current_node;
    }

    if (token_type != TOKEN_NEWLINE) {
      line_pos += yyleng;
    } else {
      breakpoint = NULL;
      line_pos = 0;
    }
    token->end_col = line_pos;
    current_node->token = token;

    if (line_pos <= max_line_length && token_type != TOKEN_NEWLINE) {
      continue;
    }
    line_pos = flush_tokens_list(tokens_list, &current_node, &breakpoint, current_indentation, line_pos);

    // Reset indentation to respect the space of the next line
    // of the original document in the resulting formated one.
    if (token_type == TOKEN_NEWLINE) {
      current_indentation = NULL;
    }
  }
  /*
    It isn't needed anymore to know the current node of the tokens list
    nor the location of a possible line break point.
    If the line was getting bigger than permitted then it was already flushed
    at the end of the main loop if it was possible (break point available).
  */
  current_node = NULL;
  breakpoint = NULL;
  flush_tokens_list(tokens_list, &current_node, &breakpoint, current_indentation, line_pos);
  free(tokens_list);
  free(current_indentation);
}
