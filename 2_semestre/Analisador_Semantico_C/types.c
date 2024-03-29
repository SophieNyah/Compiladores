#include<stddef.h>
#include<stdlib.h>
#include"ast_symbols.h"
#include"types.h"
#include"errors.h"

const int TIPOS_POINTER_SIZE = 32;
const int TIPOS_INT_SIZE     = 32;
const int TIPOS_CHAR_SIZE    =  8;
const int TIPOS_VOID_SIZE    =  8;

const int compatibility_table[29][10] =
{
                            /*  INT/INT  CHAR/CHAR  INT/CHAR  PNT/INT  PNT/CHAR PNT/PNT  VOID/INT  VOID/CHAR  VOID/POINTER  VOID/VOID  */
    /* PLUS_COMP         */ {      1,        1,        1,        1,       1,       0,       -1,       -1,         -1,          -1      },
    /* MINUS_COMP        */ {      1,        1,        1,        2,       2,       0,       -1,       -1,         -1,          -1      },
    /* MUL_COMP          */ {      1,        1,        1,        0,       0,       0,       -1,       -1,         -1,          -1      },
    /* DIV_COMP          */ {      1,        1,        1,        0,       0,       0,       -1,       -1,         -1,          -1      },
    /* REMAINDER_COMP    */ {      1,        1,        1,        0,       0,       0,       -1,       -1,         -1,          -1      },
    /* BIT_AND_COMP      */ {      1,        1,        1,        0,       0,       0,       -1,       -1,         -1,          -1      },
    /* BIT_OR_COMP       */ {      1,        1,        1,        0,       0,       0,       -1,       -1,         -1,          -1      },
    /* BIT_XOR_COMP      */ {      1,        1,        1,        0,       0,       0,       -1,       -1,         -1,          -1      },
    /* LOG_AND_COMP      */ {      1,        1,        1,        1,       1,       1,       -1,       -1,         -1,          -1      },
    /* LOG_OR_COMP       */ {      1,        1,        1,        1,       1,       1,       -1,       -1,         -1,          -1      },
    /* EUQAL_COMP        */ {      1,        1,        1,        3,       3,       3,       -1,       -1,         -1,          -1      },
    /* DIFF_COMP         */ {      1,        1,        1,        3,       3,       3,       -1,       -1,         -1,          -1      },
    /* LESS_COMP         */ {      1,        1,        1,        3,       3,       3,       -1,       -1,         -1,          -1      },
    /* GREAT_COMP        */ {      1,        1,        1,        3,       3,       3,       -1,       -1,         -1,          -1      },
    /* LEQ_COMP          */ {      1,        1,        1,        3,       3,       3,       -1,       -1,         -1,          -1      },
    /* GEQ_COMP          */ {      1,        1,        1,        3,       3,       3,       -1,       -1,         -1,          -1      },
    /* RSHIFT_COMP       */ {      1,        1,        1,        4,       4,       0,       -1,       -1,         -1,          -1      },
    /* LSHIFT_COMP       */ {      1,        1,        1,        4,       4,       0,       -1,       -1,         -1,          -1      },
    /* ASSIGN_COMP       */ {      1,        1,        1,        7,       7,       5,       -1,       -1,         -1,          -1      },
    /* ADD_ASSIGN_COMP   */ {      1,        1,        1,        6,       6,       7,       -1,       -1,         -1,          -1      },
    /* SUB_ASSIGN_COMP   */ {      1,        1,        1,        6,       6,       7,       -1,       -1,         -1,          -1      },
                            /*    INT       CHAR      PNT       VOID      --       --       --        --          --           --      */
    /* UN_PLUS_COMP      */ {      1,        1,        8,        -1,      0,       0,       0,        0,          0,            0      },
    /* UN_MINUS_COMP     */ {      1,        1,        9,        -1,      0,       0,       0,        0,          0,            0      },
    /* UN_DEFERENCE_COMP */ {      0,        0,        1,        -1,      0,       0,       0,        0,          0,            0      },
    /* UN_INC_COMP       */ {      1,        1,        1,        -1,      0,       0,       0,        0,          0,            0      },
    /* UN_DEC_COMP       */ {      1,        1,        1,        -1,      0,       0,       0,        0,          0,            0      },
    /* UN_BIT_NOT_COMP   */ {      1,        1,        0,        -1,      0,       0,       0,        0,          0,            0      },
    /* UN_LOG_NOT_COMP   */ {      1,        1,        1,        -1,      0,       0,       0,        0,          0,            0      },
    /* UN_ADDRESS_COMP   */ {      1,        1,        1,        -1,      0,       0,       0,        0,          0,            0      },
};
const int types_fusion[4][4] =
{
    { INT_INT,      INT_CHAR,    POINTER_INT,     VOID_INT     },
    { INT_CHAR,     CHAR_CHAR,   POINTER_CHAR,    VOID_CHAR    },
    { POINTER_INT, POINTER_CHAR, POINTER_POINTER, VOID_POINTER },
    { VOID_INT,     VOID_CHAR,   VOID_POINTER,    VOID_VOID    },
};

Type_matching verifyTypes(Var_type var1, Var_type var2){
    Type_matching error = MATCH;

    if(var1.pointers == var2.pointers && var1.type == var2.type){ //variáveis compatíveis
        error = MATCH;
    }else if(!var1.pointers != !var2.pointers){     //uma, e apenas uma, é ponteiro
        error = PTR_AND_NON_PTR;
    }else if(var1.pointers > 0){    // ambas são ponteiros
        if(var1.pointers != var2.pointers){ // diferença de ponteiros
            error = PTR_MISSMATCH;
        }else if(var1.type != var2.type){ //diferença da base dos ponteiros
            error = BASE_PTR_MISSMATCH;
        }
    }else{  //nenhuma é ponteiro, mas os tipos diferem
        error = TYPE_MISSMATCH;
    }

    return error;
}

Error_list checkTypeMissmatch(int result_to_analyze, Var_type var1, Var_type var2, int var1_type, int var2_type){
    Error_list error = NO_ERROR;
    Type_matching matching = verifyTypes(var1, var2);

    switch(result_to_analyze){
        case -1: //operação com void
            error = VOID_NOT_IGNORED;
            break;
        case 3: //pointer/(int/char/pointer) comparison
            if(matching == PTR_MISSMATCH || matching == BASE_PTR_MISSMATCH){
                error = WRONG_TYPE_COMPARISON;
            }else if(matching == PTR_AND_NON_PTR){
                error = WRONG_TYPE_COMPARISON_W;
            }
            break;
        case 5: //pointer/pointer assignment
            if(matching != MATCH){
                error = INCOMPATIBLE_ASSIGNMENT;
            }
            break;
        case 2: //pointer/(int/char) subtraction
        case 4: //pointer/(int/char) shift
            if(var1.pointers == 0){ //se var esquerda é valor, e var direita é ponteiro
                error = INVALID_BIN_OPERANDS;
            }
            break;
        case 6: //pointer/(int/char) (add/sub)assign
            if(var1.pointers == 0){ //se var esquerda é valor, e var direita é ponteiro
                error = INCOMPATIBLE_ASSIGNMENT;
            }
            break;
        case 7: error = INCOMPATIBLE_ASSIGNMENT; break;
        case 8: error = WRONG_ARG_UNR_PLUS;      break;
        case 9: error = WRONG_ARG_UNR_MINUS;     break;
    }

    return error;
}

Error_list matchTypes(int operation, Var_type var1, Var_type var2){
    int var1_type, var2_type, result;
    if(var1.pointers)
        { var1_type = POINTER_COMP; }
    else if(var1.type == TIPOS_INT)
        { var1_type = INT_COMP; }
    else if(var1.type == TIPOS_CHAR)
        { var1_type = CHAR_COMP; }
    else
        { var1_type = VOID_COMP; }

    if(var2.pointers)
        { var2_type = POINTER_COMP; }
    else if(var2.type == TIPOS_INT)
        { var2_type = INT_COMP; }
    else if(var2.type == TIPOS_CHAR)
        { var2_type = CHAR_COMP; }
    else
        { var2_type = VOID_COMP; }

    if(operation >= UN_PLUS_COMP){
        result = compatibility_table[operation][var1_type];
    }else{
        result = compatibility_table[operation][types_fusion[var1_type][var2_type]];
    }
    switch(result){
        case  0:
            if(operation >= UN_PLUS_COMP){
                return INVALID_UNR_OPERAND;
            }else{
                return INVALID_BIN_OPERANDS;
            }
        case  1: return NO_ERROR;
        default: return checkTypeMissmatch(result, var1, var2, var1_type, var2_type);
    }
}

Func_type_state matchReturnType(Symbol *func, Command_list *commands){
    Command_list *aux = commands;
    Func_type_state state = { .error=NO_ERROR };
    bool is_void_func_type = isVoid(func->type);
    bool has_return = false;

    for(;aux;aux=aux->next){
        if(aux->com_type == COM_RETURN){
            state.func = malloc(sizeof(Expression));
            if(!aux->com.return_com && !is_void_func_type){
                state.error = RETURN_WO_VALUE_IN_NONVOID_FUNC;
                state.func->line = aux->line;
                state.func->column = aux->column;
                return state;
            }else if(aux->com.return_com && is_void_func_type){
                state.error = VALUE_RETURN_IN_VOID_FUNC;
                state.func->line = func->line;
                state.func->column = func->column;
                return state;
            }else if(aux->com.return_com && !is_void_func_type){
                if(verifyTypes(func->type, aux->com.return_com->exp_type) != MATCH){
                    state.error = INCOMPATIBLE_RETURN_TYPE;
                    state.func->line = aux->line;
                    state.func->column = aux->column;
                    state.expected_type = func->type;
                    state.received_type = aux->com.return_com->exp_type;
                    return state;
                }
            }

            has_return = true;
        }
    }

    if(!is_void_func_type && !has_return){
        state.error = NO_RETURN_IN_NONVOID_FUNC; 
        state.func = malloc(sizeof(Expression));
        state.func->line = func->line;
        state.func->column = func->column;
        return state;
    }

    return state;
}

bool isVoid(Var_type type){
    if(type.type == TIPOS_VOID && type.pointers == 0){
        return true;
    }else{
        return false;
    }
}
