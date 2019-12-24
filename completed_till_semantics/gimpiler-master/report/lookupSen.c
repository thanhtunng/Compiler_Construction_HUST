Object *lookupObject(char *name) {
    Object *ret = NULL;

    Scope *currScope = symtab->currentScope;
    // find object in program from current Scope to outside
    while (currScope != NULL) {
        ret = findObject(currScope->objList, name);
        if (ret)
            return ret;
        currScope = currScope->outer;
    }

    // if object not found, find it in global object list
    ret = findObject(symtab->globalObjectList, name);
    if (ret)
        return ret;

    return NULL;
}