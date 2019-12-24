/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  // Keep reading blank until no more space
  while(currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
    readChar();
}

void skipComment() {
  while(currentChar != EOF )
  {
    if(charCodes[currentChar] == CHAR_TIMES)
    {
      readChar();
      if(currentChar == EOF)
        break;
      if(charCodes[currentChar] == CHAR_RPAR)
      {
        readChar();
        return;
      }
    }
    readChar();
  }

  error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

Token* readIdentKeyword(void) {
  // TODO
  Token *token = makeToken(TK_IDENT, lineNo, colNo);
  int string_pos = 1;
  token->string[0] = currentChar;
  // int start_Line_No = lineNo, start_Col_No = colNo;
  readChar();
  while(currentChar != EOF && (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_LETTER))
  {
    if(string_pos > MAX_IDENT_LEN)
    {
      token = makeToken(TK_NONE, lineNo, colNo);
      error(ERR_IDENTTOOLONG, lineNo, colNo);
      return token;
    }
    token->string[string_pos++] = currentChar;
    readChar();
  }

  if(string_pos > MAX_IDENT_LEN)
  {
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_IDENTTOOLONG, lineNo, colNo);
    return token;  
  }
  else
  {
    if(checkKeyword(token->string) == TK_NONE)
      token->tokenType = TK_IDENT;
    else     
      token->tokenType = checkKeyword(token->string);    
  }
  return token;
}

Token* readNumber(void) {
  // TODO
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int string_pos = 0;

  while(currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT)
  {
    if(string_pos > MAX_NUMBER_LEN)
    {
      readChar();
      error(ERR_NUMBERTOOLONG, lineNo, colNo);
    }
    token->string[string_pos++] = currentChar;
    readChar(); 
  }
  token->value = atoi(token->string);
  return token;
}

Token* readConstChar(void) {
  // TODO
  readChar();
  Token *token;
  int ln = lineNo, col = colNo;
  int pos = 0;
  if(currentChar != EOF || charCodes[currentChar] == CHAR_BACKSLASH)
  {
    char temp;
    temp = currentChar;
    if(charCodes[currentChar] == CHAR_BACKSLASH)
    {
      token = makeToken(TK_CHAR, ln, col);
      token->string[pos] = temp;
      readChar();
      // ln = lineNo; col = colNo;
      if(charCodes[currentChar] == CHAR_SINGLEQUOTE)
      {
        readChar();
        if(charCodes[currentChar] == CHAR_SINGLEQUOTE)
        {
          token->string[pos] = currentChar;          
          readChar();      
          return token;
        }
        else
        {
          readChar();
          return token;
        }      
      }
      else if(charCodes[currentChar] == CHAR_BACKSLASH)
      {
        readChar();
        if(charCodes[currentChar] == CHAR_SINGLEQUOTE)
        {
          readChar();
          return token;
        }
        else
        {
          error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
        }
      }
      
    }
    readChar();
    if(charCodes[currentChar] == CHAR_SINGLEQUOTE)
    {
      token = makeToken(TK_CHAR, ln, col);
      token->string[pos] = temp;
      if(charCodes[temp] == CHAR_SINGLEQUOTE)
      {
        token = makeToken(TK_NONE, lineNo, colNo);
        error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
        return token;
      }
      else{
        readChar();
        return token;
      }
    }
    else
    {
      token = makeToken(TK_NONE, ln, col);
      error(ERR_INVALIDCHARCONSTANT, ln, col);
      return token;
    }

  }
  else
  {
    token = makeToken(TK_NONE, ln, col);
    error(ERR_INVALIDCHARCONSTANT, ln, col);

    return token;
  }
}

/*
 * getToken() function does lexical analysis and make the stream of corresponding tokens
 */

Token* getToken(void) 
{
  Token *token;
  int ln = lineNo, cn = colNo;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE:
    skipBlank();
    return getToken();
  case CHAR_LETTER:
    return readIdentKeyword();
  case CHAR_DIGIT: 
    return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;

  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;

  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;

  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;

  case CHAR_LT:
    ln = lineNo; cn = colNo;
    readChar();
    if(currentChar != EOF && charCodes[currentChar] == CHAR_EQ)
    {
      readChar();
      token = makeToken(SB_LE, lineNo, colNo);
    }
      token = makeToken(SB_LT, lineNo, colNo);
      return token;

  case CHAR_GT:
    ln = lineNo; cn = colNo;
    if(currentChar != EOF && charCodes[currentChar] == CHAR_EQ)
    {
      readChar();
      token = makeToken(SB_GE, lineNo, colNo);
    }
      token = makeToken(SB_GT, lineNo, colNo);
      return token;

  case CHAR_EXCLAIMATION:
    ln = lineNo; cn = colNo;
    readChar();
    if(currentChar != EOF && charCodes[currentChar] == CHAR_EQ)
    {
      readChar();
      token = makeToken(SB_NEQ, lineNo, colNo);
    }
    else
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
    return makeToken(TK_NONE, lineNo, colNo);

  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;

  case CHAR_COMMA:
    token = makeToken(SB_COMMA, ln, cn);
    readChar();
    return token;

  case CHAR_PERIOD:
    readChar();
    if(currentChar != EOF && charCodes[currentChar] == CHAR_RPAR)
    {
      return makeToken(SB_RSEL, ln, cn);
    }
    return makeToken(SB_PERIOD, ln, cn);  

  case CHAR_COLON:
    token = makeToken(SB_COLON, ln, cn);
    readChar();
    return token;

  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, ln, cn);
    readChar();
    return token;

  case CHAR_SINGLEQUOTE:
    return readConstChar();

  case CHAR_LPAR:
    readChar();
    switch(charCodes[currentChar])
    {
      case CHAR_PERIOD:
        token = makeToken(SB_LSEL, ln, cn);
        readChar();
        return token;
      case CHAR_TIMES:
        skipComment();
        return getToken();
      case CHAR_SPACE:
        skipBlank();
        return getToken();
      default:
        token = makeToken(SB_LPAR, ln, cn);
        readChar();
        return token;
    }

  case CHAR_RPAR:
    token = makeToken(SB_RPAR, ln, cn);
    readChar();
    return token;

  case CHAR_BACKSLASH:
    token = makeToken(SB_BSLA, ln, cn);
    readChar();
    return token;

  case CHAR_DOUBLEQUOTE:
    token = makeToken(SB_DBQUOTE, ln, cn);
    readChar();
    return token;

  case CHAR_UNKNOWN:
    token = makeToken(TK_NONE, ln, cn);
    readChar();
    return token; 

  default:
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    break;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  case SB_BSLA: printf("SB_BSLA\n"); break;
  case SB_DBQUOTE: printf("SB_DBQUOTE\n"); break;
  }
}

/*
 * scan() function scans content of files, lexical analysis
 * and print out tokens with the corresponding position 
 */

int scan(char *fileName) {
  Token *token = malloc(sizeof(Token));

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();

  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
  

  return 0;
}



