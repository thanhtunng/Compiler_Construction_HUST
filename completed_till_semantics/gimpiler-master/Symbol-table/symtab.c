/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object *obj);

void freeScope(Scope *scope);

void freeObjectList(ObjectNode *objList);

void freeReferenceList(ObjectNode *objList);

SymTab *symtab;
Type *intType;
Type *charType;

/******************* Type utilities ******************************/

Type *makeIntType(void) {
    Type *type = (Type *) malloc(sizeof(Type));
    type->typeClass = TP_INT;
    return type;
}

Type *makeCharType(void) {
    Type *type = (Type *) malloc(sizeof(Type));
    type->typeClass = TP_CHAR;
    return type;
}

Type *makeArrayType(int arraySize, Type *elementType) {
    Type *type = (Type *) malloc(sizeof(Type));
    type->typeClass = TP_ARRAY;
    type->arraySize = arraySize;
    type->elementType = elementType;
    return type;
}

Type *duplicateType(Type *type) {
    Type *ret = (Type *) malloc(sizeof(Type));

    if (ret->typeClass == TP_ARRAY) {
        ret->elementType = duplicateType(type->elementType);
        ret->arraySize = type->arraySize;
    }

    ret->typeClass = type->typeClass;
    return ret;
}

int compareType(Type *type1, Type *type2) {
    // check type1 & type2 class equal ?
    if (type1->typeClass != type2->typeClass) {
        // not same type
        return 0;
    }

    // check typeClass is not array
    if (type1->typeClass != TP_ARRAY) {
        // same type but not array
        return 1;
    }

    // is array
    // check array size
    if (type1->arraySize != type2->arraySize) {
        // not same size
        return 0;
    }


    // compare recursive elementType
    return compareType(type1->elementType, type2->elementType);

}


void freeType(Type *type) {
    if(type == NULL)
        return;
    //check if type is array
    if (type->typeClass == TP_ARRAY) {
        // freeType recursive elementType
        freeType(type->elementType);
    }

    free(type);
}

/******************* Constant utility ******************************/

ConstantValue *makeIntConstant(int i) {
    ConstantValue *ret = (ConstantValue *) malloc(sizeof(ConstantValue));
    ret->type = TP_INT;
    ret->intValue = i;
    return ret;
}

ConstantValue *makeCharConstant(char ch) {
    ConstantValue *ret = (ConstantValue *) malloc(sizeof(ConstantValue));
    ret->type = TP_CHAR;
    ret->intValue = ch;
    return ret;
}

ConstantValue *duplicateConstantValue(ConstantValue *v) {
    ConstantValue *ret = (ConstantValue *) malloc(sizeof(ConstantValue));
    ret->type = v->type;
    ret->intValue = v->intValue;
    ret->charValue = v->charValue;
    return ret;
}

/******************* Object utilities ******************************/

Scope *createScope(Object *owner, Scope *outer) {
    Scope *scope = (Scope *) malloc(sizeof(Scope));
    scope->objList = NULL;
    scope->owner = owner;
    scope->outer = outer;
    return scope;
}

Object *createProgramObject(char *programName) {
    Object *program = (Object *) malloc(sizeof(Object));
    strcpy(program->name, programName);
    program->kind = OBJ_PROGRAM;
    program->progAttrs = (ProgramAttributes *) malloc(sizeof(ProgramAttributes));
    program->progAttrs->scope = createScope(program, NULL);
    symtab->program = program;

    return program;
}

Object *createConstantObject(char *name) {
    Object *constantObj = (Object *) malloc(sizeof(Object));
    strcpy(constantObj->name, name);
    constantObj->kind = OBJ_CONSTANT;
    constantObj->constAttrs = (ConstantAttributes *) malloc(sizeof(ConstantAttributes));
    return constantObj;
}

Object *createTypeObject(char *name) {
    Object *typeObj = (Object *) malloc(sizeof(Object));
    strcpy(typeObj->name, name);
    typeObj->kind = OBJ_TYPE;
    typeObj->typeAttrs = (TypeAttributes *) malloc(sizeof(TypeAttributes));
    return typeObj;
}

Object *createVariableObject(char *name) {
    Object *varObj = (Object *) malloc(sizeof(Object));
    strcpy(varObj->name, name);
    varObj->kind = OBJ_VARIABLE;
    varObj->varAttrs = (VariableAttributes *) malloc(sizeof(VariableAttributes));
    varObj->varAttrs->scope = symtab->currentScope;
    return varObj;
}

Object *createFunctionObject(char *name) {
    Object *FuncObj = (Object *) malloc(sizeof(Object));
    strcpy(FuncObj->name, name);
    FuncObj->kind = OBJ_FUNCTION;
    FuncObj->funcAttrs = (FunctionAttributes *) malloc(sizeof(FunctionAttributes));
    FuncObj->funcAttrs->scope = createScope(FuncObj, symtab->currentScope);
    FuncObj->funcAttrs->paramList = (ObjectNode *) malloc(sizeof(ObjectNode));
    return FuncObj;
}

Object *createProcedureObject(char *name) {
    Object *procedureObj = (Object *) malloc(sizeof(Object));
    strcpy(procedureObj->name, name);
    procedureObj->kind = OBJ_PROCEDURE;
    procedureObj->procAttrs = (ProcedureAttributes *) malloc(sizeof(ProcedureAttributes));
    procedureObj->procAttrs->scope = createScope(procedureObj, symtab->currentScope);
    procedureObj->procAttrs->paramList = (ObjectNode *) malloc(sizeof(ObjectNode));
    return procedureObj;
}

Object *createParameterObject(char *name, enum ParamKind kind, Object *owner) {
    Object *parameterObj = (Object *) malloc(sizeof(Object));
    strcpy(parameterObj->name, name);
    parameterObj->kind = OBJ_PARAMETER;
    parameterObj->paramAttrs = (ParameterAttributes *) malloc(sizeof(ParameterAttributes));
    parameterObj->paramAttrs->kind = kind;
    parameterObj->paramAttrs->function = owner;
    return parameterObj;
}

void freeObject(Object *obj) {
    if (obj == NULL)
        return;
    switch (obj->kind) {
        case OBJ_CONSTANT:
            free(obj->constAttrs);
            free(obj);
            break;
        case OBJ_VARIABLE:
            free(obj->varAttrs);
            free(obj);
            break;
        case OBJ_TYPE:
            free(obj->typeAttrs);
            free(obj);
            break;
        case OBJ_FUNCTION:
            free(obj->funcAttrs->scope);
            free(obj->funcAttrs->paramList);
            free(obj->funcAttrs);
            free(obj);
            break;
        case OBJ_PROCEDURE:
            free(obj->procAttrs->scope);
            free(obj->procAttrs->paramList);
            free(obj->procAttrs);
            free(obj);
            break;
        case OBJ_PARAMETER:
            free(obj->paramAttrs);
            free(obj);
            break;
        case OBJ_PROGRAM:
            free(obj->progAttrs->scope);
            free(obj->progAttrs);
            free(obj);
            break;
    }
}

void freeScope(Scope *scope) {
    if (scope == NULL)
        return;
    freeObjectList(scope->objList);
    freeObject(scope->owner);
    free(scope);
}

void freeObjectList(ObjectNode *objList) {
    if (objList == NULL)
        return;
    freeObject(objList->object);
    freeObjectList(objList->next);
}

void freeReferenceList(ObjectNode *objList) {
    if (objList == NULL)
        return;
    freeObject(objList->object);
    freeObjectList(objList->next);
}

void addObject(ObjectNode **objList, Object *obj) {
    ObjectNode *node = (ObjectNode *) malloc(sizeof(ObjectNode));
    node->object = obj;
    node->next = NULL;
    if ((*objList) == NULL)
        *objList = node;
    else {
        ObjectNode *n = *objList;
        while (n->next != NULL)
            n = n->next;
        n->next = node;
    }
}

Object *findObject(ObjectNode *objList, char *name) {
    if(objList == NULL)
        return  NULL;

    if(strcmp(objList->object->name, name) == 0) {
        return  objList->object;
    }

    return findObject(objList->next, name);
}

/******************* others ******************************/

void initSymTab(void) {
    Object *obj;
    Object *param;

    symtab = (SymTab *) malloc(sizeof(SymTab));
    symtab->globalObjectList = NULL;

    obj = createFunctionObject("READC");
    obj->funcAttrs->returnType = makeCharType();
    addObject(&(symtab->globalObjectList), obj);

    obj = createFunctionObject("READI");
    obj->funcAttrs->returnType = makeIntType();
    addObject(&(symtab->globalObjectList), obj);

    obj = createProcedureObject("WRITEI");
    param = createParameterObject("i", PARAM_VALUE, obj);
    param->paramAttrs->type = makeIntType();
    addObject(&(obj->procAttrs->paramList), param);
    addObject(&(symtab->globalObjectList), obj);

    obj = createProcedureObject("WRITEC");
    param = createParameterObject("ch", PARAM_VALUE, obj);
    param->paramAttrs->type = makeCharType();
    addObject(&(obj->procAttrs->paramList), param);
    addObject(&(symtab->globalObjectList), obj);

    obj = createProcedureObject("WRITELN");
    addObject(&(symtab->globalObjectList), obj);

    intType = makeIntType();
    charType = makeCharType();
}

void cleanSymTab(void) {
    freeObject(symtab->program);
    freeObjectList(symtab->globalObjectList);
    free(symtab);
    freeType(intType);
    freeType(charType);
}

void enterBlock(Scope *scope) {
    symtab->currentScope = scope;
}

void exitBlock(void) {
    symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object *obj) {
    if (obj->kind == OBJ_PARAMETER) {
        Object *owner = symtab->currentScope->owner;
        switch (owner->kind) {
            case OBJ_FUNCTION:
                addObject(&(owner->funcAttrs->paramList), obj);
                break;
            case OBJ_PROCEDURE:
                addObject(&(owner->procAttrs->paramList), obj);
                break;
            default:
                break;
        }
    }

    addObject(&(symtab->currentScope->objList), obj);
}


