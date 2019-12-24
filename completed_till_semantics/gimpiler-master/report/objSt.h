struct Object_ {
    char name[MAX_IDENT_LEN];
    enum ObjectKind kind;
    union {
        ConstantAttributes *constAttrs;
        VariableAttributes *varAttrs;
        TypeAttributes *typeAttrs;
        FunctionAttributes *funcAttrs;
        ProcedureAttributes *procAttrs;
        ProgramAttributes *progAttrs;
        ParameterAttributes *paramAttrs;
    };
};