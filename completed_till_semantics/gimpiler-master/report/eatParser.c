void eat(TokenType tokenType) {
    if (lookAhead->tokenType == tokenType) {
        printToken(lookAhead);
        scan();
    } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}