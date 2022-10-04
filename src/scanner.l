%option nodefault
%option 8bit
%option full
%option noyywrap

%{
  #include "../src/token_types.h"
%}

%x CODE_BLOCK

BLANK   [[:blank:]]
NON_BLANK [^[:blank:]\n]

%%

^{BLANK}*```[[:alnum:]]*{BLANK}*$       {
                                          BEGIN(CODE_BLOCK);
                                          return TOKEN_BEGIN_CODE_BLOCK_OK;
                                        }

^{BLANK}*```[[:alnum:]]*{BLANK}*      {
                                        BEGIN(CODE_BLOCK);
                                        return TOKEN_BEGIN_CODE_BLOCK_RIGHT_UNPAIRED;
                                      }

{BLANK}*```[[:alnum:]]*{BLANK}*$      {
                                        BEGIN(CODE_BLOCK);
                                        return TOKEN_BEGIN_CODE_BLOCK_LEFT_UNPAIRED;
                                      }    

{BLANK}*```[[:alnum:]]*{BLANK}*       {
                                        BEGIN(CODE_BLOCK);
                                        return TOKEN_BEGIN_CODE_BLOCK_UNPAIRED;
                                      }

<CODE_BLOCK>{BLANK}+$

<CODE_BLOCK>\n+                           return TOKEN_CODE_BLOCK_NEWLINE;

<CODE_BLOCK>({BLANK}*[^`\n[:blank:]]+)+   return TOKEN_CODE_BLOCK;

  /*
    Because the rule with three backticks will have higher preference because of its length,
    simply consume unpaired backticks (one or two) here.
  */  
<CODE_BLOCK>`{1,2}             return TOKEN_CODE_BLOCK;

<CODE_BLOCK>^{BLANK}*```{BLANK}*$   {
                                      BEGIN(INITIAL);
                                      return TOKEN_END_CODE_BLOCK_OK;
                                    }

<CODE_BLOCK>{BLANK}*```{BLANK}*$  {
                                    BEGIN(INITIAL);
                                    return TOKEN_END_CODE_BLOCK_LEFT_UNPAIRED;
                                  }    


<CODE_BLOCK>^{BLANK}*```{BLANK}*  {
                                    BEGIN(INITIAL);
                                    return TOKEN_END_CODE_BLOCK_RIGHT_UNPAIRED;
                                  }

<CODE_BLOCK>{BLANK}*```{BLANK}*   {
                                    BEGIN(INITIAL);
                                    return TOKEN_END_CODE_BLOCK_UNPAIRED;
                                  }

\[[^\]\n]*\]\([^\)\n]*\)      return TOKEN_DEFAULT;

`[^`\n]*`                     return TOKEN_DEFAULT;

[^\ \t\n\(\)\[\]\#\-\>\`]+    return TOKEN_DEFAULT;

[\(\)\[\]\#\-\>\`]+           return TOKEN_DEFAULT;

^{BLANK}*#+({BLANK}+{NON_BLANK}+)+  return TOKEN_HEADER;

^{BLANK}*(-|>){BLANK}+{NON_BLANK}+  return TOKEN_ITEM;

{BLANK}+$                   /* trim blank spaces at the end of lines */

^{BLANK}+                   return TOKEN_LINE_INDENTATION;

{BLANK}+                    return TOKEN_BREAKPOINT;

\n+                         return TOKEN_NEWLINE;

  /* Let these extra lines so Flex doesn't complain about unexpected end of file */
