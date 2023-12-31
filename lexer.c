#include "token.c"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "error.c"

Error *error_type;

typedef struct LEXER_STRUCT
{
  char c;
  unsigned int i;
  char *contents;
} lexer_T;

lexer_T *init_lexer(char *contents);
void lexer_advance(lexer_T *lexer);
void lexer_skip_whitespace(lexer_T *lexer);
token_T *lexer_get_next_token(lexer_T *lexer);
token_T *lexer_collect_string(lexer_T *lexer);
token_T *lexer_advance_with_token(lexer_T *lexer, token_T *token);
token_T *lexer_collect_id(lexer_T *lexer);
char *lexer_get_current_char_as_string(lexer_T *lexer);
token_T *lexer_collect_argument(lexer_T *lexer);

lexer_T *init_lexer(char *contents)
{
  lexer_T *lexer = calloc(1, sizeof(lexer_T));
  lexer->contents = contents;
  lexer->i = 0;
  lexer->c = contents[lexer->i];
  return lexer;
}

void lexer_advance(lexer_T *lexer)
{
  if (error_type->isError)
  {
    return;
  }
  if (lexer->c != '\0' && lexer->i < strlen(lexer->contents))
  {
    lexer->i++;
    lexer->c = lexer->contents[lexer->i];
  }
}

void lexer_skip_whitespace(lexer_T *lexer)
{
  while (lexer->c == ' ')
  {
    lexer_advance(lexer);
  }
}

token_T *lexer_get_next_token(lexer_T *lexer)
{

  if (error_type->isError)
  {
    return NULL;
  }
  while (lexer->c != '\0' && lexer->i < strlen(lexer->contents))
  {
    // printf("lol2,%d\n", lexer->i);

    if (error_type->isError)
    {
      return NULL;
    }
    if (lexer->c == ' ' || lexer->c == 10)
    {
      lexer_skip_whitespace(lexer);
    }

    if (lexer->c == 'x')
    {
      if (lexer->contents[lexer->i - 1] == '(' &&
          (lexer->contents[lexer->i + 1] == ',' ||
           lexer->contents[lexer->i + 1] == ')'))
      {

        return lexer_advance_with_token(
            lexer, init_token(TOKEN_ARGUMENTS,
                              lexer_get_current_char_as_string(lexer)));
      }
      else if (lexer->contents[lexer->i + 1] == ')' &&
               (lexer->contents[lexer->i - 1] == ',' ||
                lexer->contents[lexer->i - 1] == '('))
      {
        return lexer_advance_with_token(
            lexer, init_token(TOKEN_ARGUMENTS,
                              lexer_get_current_char_as_string(lexer)));
      }
    }
    if (isdigit(lexer->c))
    {
      return lexer_collect_argument(lexer);
    }

    if (isalnum(lexer->c))
    {
      return lexer_collect_id(lexer);
    }

    switch (lexer->c)
    {
    case '(':
      return lexer_advance_with_token(
          lexer,
          init_token(TOKEN_LPAREN, lexer_get_current_char_as_string(lexer)));
      break;
    case ')':
      return lexer_advance_with_token(
          lexer,
          init_token(TOKEN_RPAREN, lexer_get_current_char_as_string(lexer)));
      break;
    case ',':
      return lexer_advance_with_token(
          lexer,
          init_token(TOKEN_COMMA, lexer_get_current_char_as_string(lexer)));
      break;
    default:
      define_error(error_type, lexer->i, "Unexpected token");
    }
  }
  return NULL;
}

token_T *lexer_advance_with_token(lexer_T *lexer, token_T *token)
{
  lexer_advance(lexer);
  return token;
}

char *lexer_get_current_char_as_string(lexer_T *lexer)
{
  char *str = calloc(2, sizeof(char));
  str[0] = lexer->c;
  str[1] = '\0';
  return str;
}

token_T *lexer_collect_id(lexer_T *lexer)
{

  char *value = calloc(1, sizeof(char));
  value[0] = '\0';

  while (isalnum(lexer->c))
  {
    char *s = lexer_get_current_char_as_string(lexer);
    value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
    strcat(value, s);

    lexer_advance(lexer);
  }

  return init_token(TOKEN_FUNC, value);
}
token_T *lexer_collect_argument(lexer_T *lexer)
{

  char *value = calloc(1, sizeof(char));
  value[0] = '\0';

  while (isalnum(lexer->c) || lexer->c == '.')
  {
    char *s = lexer_get_current_char_as_string(lexer);
    value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
    strcat(value, s);

    lexer_advance(lexer);
  }

  return init_token(TOKEN_ARGUMENTS, value);
}
