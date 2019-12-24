/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"
#include "token.h"

extern SymTab *symtab;
extern Token *currentToken;

Object *lookupObject(char *name) {
    Object *ret = NULL;

    Scope *currScope = symtab->currentScope;
    while (currScope != NULL) {
        ret = findObject(currScope->objList, name);
        if (ret)
            return ret;
        currScope = currScope->outer;
    }

    ret = findObject(symtab->globalObjectList, name);
    if (ret)
        return ret;

    return NULL;
}

void checkFreshIdent(char *name) {
    Object *loopObj = findObject(symtab->currentScope->objList, name);
    if (loopObj)
        error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
}

Object *checkDeclaredIdent(char *name) {
    Object *found = lookupObject(name);
    if (found)
        return found;

    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
}

Object *checkDeclaredConstant(char *name) {
    Object *constantObj = lookupObject(name);
    if (!constantObj || constantObj->kind != OBJ_CONSTANT)
        error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
    return constantObj;
}

Object *checkDeclaredType(char *name) {
    Object *typeObj = lookupObject(name);
    if (!typeObj || typeObj->kind != OBJ_TYPE)
        error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
    return typeObj;
}

Object *checkDeclaredVariable(char *name) {
    Object *varObj = lookupObject(name);
    if (!varObj || varObj->kind != OBJ_VARIABLE)
        error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
    return varObj;
}

Object *checkDeclaredFunction(char *name) {
    Object *funcObj = lookupObject(name);
    if (!funcObj || funcObj->kind != OBJ_FUNCTION)
        error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);
    return funcObj;
}

Object *checkDeclaredProcedure(char *name) {
    Object *procObj = lookupObject(name);
    if (!procObj || procObj->kind != OBJ_PROCEDURE)
        error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
    return procObj;
}

Object *checkDeclaredLValueIdent(char *name) {
    Object *valueObj = lookupObject(name);
    if (!valueObj)
        error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
    if (valueObj->kind != OBJ_FUNCTION)
        if (valueObj->kind != OBJ_VARIABLE)
            if (valueObj->kind != OBJ_PARAMETER)
                error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

    return valueObj;
}

