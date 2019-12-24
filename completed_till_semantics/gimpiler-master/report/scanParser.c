void scan(void) {
    Token *tmp = currentToken;
    currentToken = lookAhead;
    lookAhead = getValidToken();
    free(tmp);
}