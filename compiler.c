#include "array.c"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

extern Error *e_t;

int isNumber(const char *s)
{
  if (s == NULL || *s == '\0')
    return false;

  bool hasDigit = false;
  bool hasDecimalPoint = false;
  bool hasExponent = false;
  bool hasSign = false;

  while (*s)
  {
    if (*s == '-' || *s == '+')
    {
      if (hasSign || hasDigit || hasDecimalPoint || hasExponent)
        return false; // Sign not allowed in the middle
      hasSign = true;
    }
    else if (*s == '.')
    {
      if (hasDecimalPoint || hasExponent)
        return false; // Multiple decimal points or already in exponent
      hasDecimalPoint = true;
    }
    else if (*s == 'e' || *s == 'E')
    {
      if (hasExponent || !hasDigit)
        return false;
      hasExponent = true;
      hasSign = false;
    }
    else if (*s >= '0' && *s <= '9')
    {
      hasDigit = true;
    }
    else
    {
      return false;
    }
    s++;
  }

  return hasDigit;
}
double logbase(double y, int b)
{
  double lg;
  lg = log10(b) / log10(y);
  return (lg);
}

typedef struct ASTNode
{
  char *value;
  double arg_value;
  struct ASTNode *children[2];
  token_T *token;
  Error *e_t;
} ASTNode;
ASTNode *createNode(char *value, token_T *token);
ASTNode *parseExpression(TokenArray *tArray, int count, int index);
ASTNode *createAST(TokenArray *tArray, int count);

ASTNode *createNode(char *value, token_T *token)
{
  ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
  node->value = value;
  node->children[0] = NULL;
  node->children[1] = NULL;
  node->token = token;
  return node;
}

ASTNode *createAST(TokenArray *tArray, int count)
{
  int index = 0;
  return parseExpression(tArray, count, index);
}

int globalIndex = 0;

ASTNode *parseExpression(TokenArray *tArray, int count, int index)
{

  token_T token = *(tArray->tokens[index]);

  if (token.type == TOKEN_FUNC)
  {

    index++;
    if (index < count && tArray->tokens[index]->type == TOKEN_LPAREN)
    {
      index++;
      int localIndex = index;
      int parenCount = 1;
      int left = index;
      int right = -1;
      while (localIndex < count)
      {
        if (tArray->tokens[localIndex]->type == TOKEN_LPAREN)
        {
          parenCount++;
        }
        else if (tArray->tokens[localIndex]->type == TOKEN_RPAREN)
        {
          parenCount--;
        }

        if (parenCount == 0)
        {
          right = localIndex;
          break;
        }
        localIndex++;
      }
      if (right == -1)
      {
        e_t->index = localIndex;
        e_t->isError = true;
        e_t->type = "Unmatched parentheses";
      }
      else
      {

        ASTNode *node = createNode(tArray->tokens[index - 2]->value,
                                   tArray->tokens[index - 2]);
        int gIndex = left;
        node->children[0] = parseExpression(tArray, right, left);
        int gLIndex = left;
        int parentCount = 0;
        int found = -1;
        while (gLIndex < right)
        {
          if (tArray->tokens[gLIndex]->type == TOKEN_LPAREN)
          {
            parenCount++;
          }
          else if (tArray->tokens[gLIndex]->type == TOKEN_RPAREN)
          {
            parenCount--;
          }
          if (parenCount == 0 && tArray->tokens[gLIndex]->type == TOKEN_COMMA)
          {
            found = 1;
            break;
          }

          gLIndex++;
        }
        if (found == 1)
        {
          node->children[1] = parseExpression(tArray, right, gLIndex + 1);
        }
        globalIndex = right;
        return node;
      }
    }
    else
    {
      e_t->index = index;
      e_t->isError = true;
      e_t->type = "Need atleat one argument";
    }
  }
  else
  {
    if (token.type == TOKEN_ARGUMENTS)
    {
      ASTNode *node =
          createNode(tArray->tokens[index]->value, tArray->tokens[index]);
      return node;
    }
  }
}
void checkArgument(ASTNode *node, int l)
{

  int i = 0;

  if (node->children[0] != NULL)
  {
    i++;
  }
  if (node->children[1] != NULL)
  {
    i++;
  }

  if (i == l)
  {
    return;
  }
  else
  {
    define_error(e_t, -1, "Incorrect arguments count");
  }
}
double evaluateAST(ASTNode *node, double x)
{
  if (e_t->isError)
  {
    return 0;
  }
  if (strcmp(node->value, "tan") == 0)
  {
    checkArgument(node, 1);

    double l = evaluateAST(node->children[0], x);

    return tan(l);
  }
  if (strcmp(node->value, "cos") == 0)
  {
    checkArgument(node, 1);

    double l = evaluateAST(node->children[0], x);
    return cos(l);
  }
  if (strcmp(node->value, "sin") == 0)
  {
    checkArgument(node, 1);

    double l = evaluateAST(node->children[0], x);
    return sin(l);
  }
  if (node->token->type == TOKEN_ARGUMENTS)
  {
    if (strcmp(node->value, "x") == 0)
    {
      return x;
    }
    else
    {
      if (isNumber(node->token->value))
      {
        return atof(node->token->value);
      }
    }
  }

  if (strcmp(node->value, "add") == 0)
  {
    checkArgument(node, 2);

    double l = evaluateAST(node->children[0], x);
    double l2 = evaluateAST(node->children[1], x);

    return l + l2;
  }
  if (strcmp(node->value, "minus") == 0)
  {
    checkArgument(node, 2);

    double l = evaluateAST(node->children[0], x);
    double l2 = evaluateAST(node->children[1], x);

    return l - l2;
  }
  if (strcmp(node->value, "multiply") == 0)
  {
    checkArgument(node, 2);

    double l = evaluateAST(node->children[0], x);
    double l2 = evaluateAST(node->children[1], x);

    return l * l2;
  }
  if (strcmp(node->value, "divide") == 0)
  {
    checkArgument(node, 2);

    double l = evaluateAST(node->children[0], x);
    double l2 = evaluateAST(node->children[1], x);

    return l / l2;
  }
  if (strcmp(node->value, "power") == 0)
  {
    checkArgument(node, 2);

    double l = evaluateAST(node->children[0], x);
    double l2 = evaluateAST(node->children[1], x);

    return pow(l, l2);
  }
  if (strcmp(node->value, "exp") == 0)
  {
    checkArgument(node, 1);

    double l = evaluateAST(node->children[0], x);

    return exp(l);
  }
  if (strcmp(node->value, "log10") == 0)
  {
    checkArgument(node, 1);

    double l = evaluateAST(node->children[0], x);

    return log10(l);
  }
  if (strcmp(node->value, "log") == 0 || strcmp(node->value, "ln") == 0)
  {
    checkArgument(node, 1);

    double l = evaluateAST(node->children[0], x);

    return log(l);
  }
  if (strcmp(node->value, "logBase") == 0)
  {
    checkArgument(node, 2);

    double l = evaluateAST(node->children[0], x);
    double l2 = evaluateAST(node->children[1], x);

    return logbase(l, l2);
  }
}

void display(ASTNode *ast)
{
  if (ast == NULL)
  {
    printf("end");
    return;
  }
  printf("%s", ast->value);
  printf("\n");
  printf("(");
  display(ast->children[0]);
  display(ast->children[1]);
  printf(")\n");
}

typedef struct
{
  ASTNode ast;
  Error error;
} Value;

Value func(char *code, double x)
{
  Value v;
  lexer_T *lexer = init_lexer(code);

  token_T *token = lexer_get_next_token(lexer);
  TokenArray *tokenArray = createTokenArray();
  addToTokenArray(tokenArray, token);
  while (token != NULL)
  {
    token = lexer_get_next_token(lexer);
    addToTokenArray(tokenArray, token);
  }
  int s = 0;
  int token_count = tokenArray->size - 1;

  ASTNode *ast = parseExpression(tokenArray, token_count, 0);

  int t_index = 0;
  v.ast = *ast;
  return v;
}
