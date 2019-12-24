struct SymTab_ {
    Object *program; // the program object.
    Scope *currentScope; // current scope of symbol table.
    ObjectNode *globalObjectList; // store global objects.
};