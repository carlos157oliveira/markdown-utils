#ifndef TOKEN_TYPES_H
#define TOKEN_TYPES_H

#define END_OF_SCAN 0

typedef enum {
  TOKEN_NEWLINE = 1,
  TOKEN_BREAKPOINT,
  TOKEN_LINE_INDENTATION,
  TOKEN_HEADER,
  TOKEN_ITEM,
  TOKEN_DEFAULT,
  TOKEN_BEGIN_CODE_BLOCK_OK,
  TOKEN_BEGIN_CODE_BLOCK_LEFT_UNPAIRED,
  TOKEN_BEGIN_CODE_BLOCK_RIGHT_UNPAIRED,
  TOKEN_BEGIN_CODE_BLOCK_UNPAIRED,
  TOKEN_CODE_BLOCK,
  TOKEN_END_CODE_BLOCK_OK,
  TOKEN_END_CODE_BLOCK_LEFT_UNPAIRED,
  TOKEN_END_CODE_BLOCK_RIGHT_UNPAIRED,
  TOKEN_END_CODE_BLOCK_UNPAIRED,
  TOKEN_CODE_BLOCK_NEWLINE
} TOKEN_TYPE;

#endif
