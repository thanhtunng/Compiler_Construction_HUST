/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    readChar();
}

void skipComment() {
    int state = 3;

    state3:
    while (charCodes[currentChar] != CHAR_TIMES) {
        readChar();
    };
    state = 4;

    while (charCodes[currentChar] == CHAR_TIMES) {
        readChar();
    };

    if (charCodes[currentChar] == CHAR_RPAR && state == 4) {
        readChar();
        return;
    }

    if (currentChar == EOF) {
        error(ERR_ENDOFCOMMENT, lineNo, colNo);
    }

    goto state3;
}

Token *readIdentKeyword(void) {
    Token *token;
    char ident[MAX_IDENT_LEN + 1];
    int count = 0;
    int ln = lineNo,
            cl = colNo;
    while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT) {
        ident[count] = currentChar;
        count++;
        readChar();

        if (count > MAX_IDENT_LEN + 1) {

            token = makeToken(TK_NONE, ln, cl);
            error(ERM_IDENTTOOLONG, ln, cl);
            return token;
        }
    }

    ident[count] = '\0';
    TokenType tokenType = checkKeyword(ident);
    if (tokenType == TK_NONE) {
        token = makeToken(TK_IDENT, ln, cl);
        strcpy(token->string, ident);
        return token;
    }

    token = makeToken(tokenType, ln, cl);

    strcpy(token->string, ident);
    return token;
}

Token *readNumber(void) {
    Token *token;
    char number[100];
    int count = 0;
    int cl = colNo,
            ln = lineNo;
    while (charCodes[currentChar] == CHAR_DIGIT) {
        number[count] = currentChar;
        count++;
        readChar();


    }

    number[count] = '\0';


    token = makeToken(TK_NUMBER, ln, cl);

    token->value = atoi(number);
    return token;
}

Token *readConstChar(void) {
    Token *token;
    token = makeToken(TK_CHAR, lineNo, colNo - 1);
    token->string[0] = currentChar;
    token->string[1] = '\0';

    readChar();

    if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
        readChar();
        return token;
    }


    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
}

Token *getToken(void) {
    Token *token;
    int ln = lineNo
    , cn = colNo;

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
        case CHAR_RPAR:
            token = makeToken(SB_RPAR, lineNo, colNo);
            readChar();
            return token;
        case CHAR_EQ:
            token = makeToken(SB_EQ, lineNo, colNo);
            readChar();
            return token;
        case CHAR_COMMA:
            token = makeToken(SB_COMMA, lineNo, colNo);
            readChar();
            return token;
        case CHAR_SEMICOLON:
            token = makeToken(SB_SEMICOLON, lineNo, colNo);
            readChar();
            return token;
        case CHAR_GT: // 22
            readChar();
            switch (charCodes[currentChar]) {
                case CHAR_EQ:
                    token = makeToken(SB_GE, ln, cn);
                    readChar();
                    return token;
                default:
                    token = makeToken(SB_GT, ln, cn);
                    return token;
            }
        case CHAR_LT: // 25
            readChar();
            switch (charCodes[currentChar]) {
                case CHAR_EQ:
                    token = makeToken(SB_LE, ln, cn);
                    readChar();
                    return token;
                default:
                    token = makeToken(SB_LT, ln, cn);
                    return token;
            }
        case CHAR_EXCLAIMATION: // 28
            readChar();
            switch (charCodes[currentChar]) {
                case CHAR_EQ:
                    token = makeToken(SB_NEQ, ln, cn);
                    readChar();
                    return token;
                default:
                    token = makeToken(TK_NONE, ln, cn);
                    error(ERR_INVALIDSYMBOL, ln, cn);
                    return token;
            }
        case CHAR_COLON: // 31
            readChar();
            switch (charCodes[currentChar]) {
                case CHAR_EQ:
                    token = makeToken(SB_ASSIGN, ln, cn);
                    readChar();
                    return token;
                default:
                    token = makeToken(SB_COLON, ln, cn);
                    return token;
            }
        case CHAR_PERIOD: // 19
            readChar();
            switch (charCodes[currentChar]) {
                case CHAR_RPAR:
                    token = makeToken(SB_RSEL, ln, cn);
                    readChar();
                    return token;
                default:
                    token = makeToken(SB_PERIOD, ln, cn);
                    return token;
            }
        case CHAR_LPAR: // 2
            readChar();
            switch (charCodes[currentChar]) {
                case CHAR_TIMES:
                    skipComment();
                    return getToken();
                case CHAR_PERIOD:
                    token = makeToken(SB_LSEL, ln, cn);
                    readChar();
                    return token;
                default:
                    token = makeToken(SB_LPAR, ln, cn);
                    return token;
            }
        case CHAR_SINGLEQUOTE: // 34
            readChar();
            return readConstChar();

        default:
            token = makeToken(TK_NONE, ln, cn);
            error(ERR_INVALIDSYMBOL, ln, cn);
            readChar();
            return token;
    }
}


/******************************************************************/

void printToken(Token *token) {

    printf("%d-%d:", token->lineNo, token->colNo);

    switch (token->tokenType) {
        case TK_NONE:
            printf("TK_NONE\n");
            break;
        case TK_IDENT:
            printf("TK_IDENT(%s)\n", token->string);
            break;
        case TK_NUMBER:
            printf("TK_NUMBER(%d)\n", token->value);
            break;
        case TK_CHAR:
            printf("TK_CHAR(\'%s\')\n", token->string);
            break;
        case TK_EOF:
            printf("TK_EOF\n");
            break;

        case KW_PROGRAM:
            printf("KW_PROGRAM\n");
            break;
        case KW_CONST:
            printf("KW_CONST\n");
            break;
        case KW_TYPE:
            printf("KW_TYPE\n");
            break;
        case KW_VAR:
            printf("KW_VAR\n");
            break;
        case KW_INTEGER:
            printf("KW_INTEGER\n");
            break;
        case KW_CHAR:
            printf("KW_CHAR\n");
            break;
        case KW_ARRAY:
            printf("KW_ARRAY\n");
            break;
        case KW_OF:
            printf("KW_OF\n");
            break;
        case KW_FUNCTION:
            printf("KW_FUNCTION\n");
            break;
        case KW_PROCEDURE:
            printf("KW_PROCEDURE\n");
            break;
        case KW_BEGIN:
            printf("KW_BEGIN\n");
            break;
        case KW_END:
            printf("KW_END\n");
            break;
        case KW_CALL:
            printf("KW_CALL\n");
            break;
        case KW_IF:
            printf("KW_IF\n");
            break;
        case KW_THEN:
            printf("KW_THEN\n");
            break;
        case KW_ELSE:
            printf("KW_ELSE\n");
            break;
        case KW_WHILE:
            printf("KW_WHILE\n");
            break;
        case KW_DO:
            printf("KW_DO\n");
            break;
        case KW_FOR:
            printf("KW_FOR\n");
            break;
        case KW_TO:
            printf("KW_TO\n");
            break;

        case SB_SEMICOLON:
            printf("SB_SEMICOLON\n");
            break;
        case SB_COLON:
            printf("SB_COLON\n");
            break;
        case SB_PERIOD:
            printf("SB_PERIOD\n");
            break;
        case SB_COMMA:
            printf("SB_COMMA\n");
            break;
        case SB_ASSIGN:
            printf("SB_ASSIGN\n");
            break;
        case SB_EQ:
            printf("SB_EQ\n");
            break;
        case SB_NEQ:
            printf("SB_NEQ\n");
            break;
        case SB_LT:
            printf("SB_LT\n");
            break;
        case SB_LE:
            printf("SB_LE\n");
            break;
        case SB_GT:
            printf("SB_GT\n");
            break;
        case SB_GE:
            printf("SB_GE\n");
            break;
        case SB_PLUS:
            printf("SB_PLUS\n");
            break;
        case SB_MINUS:
            printf("SB_MINUS\n");
            break;
        case SB_TIMES:
            printf("SB_TIMES\n");
            break;
        case SB_SLASH:
            printf("SB_SLASH\n");
            break;
        case SB_LPAR:
            printf("SB_LPAR\n");
            break;
        case SB_RPAR:
            printf("SB_RPAR\n");
            break;
        case SB_LSEL:
            printf("SB_LSEL\n");
            break;
        case SB_RSEL:
            printf("SB_RSEL\n");
            break;
    }
}

int scan(char *fileName) {
    Token *token;

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



