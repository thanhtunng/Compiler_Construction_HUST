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