#include"types.h"

const int compatibility_table[29][6] = {
                            /*  INT/INT  CHAR/CHAR  INT/CHAR  PNT/INT  PNT/CHAR PNT/PNT  */
    /* PLUS_COMP         */ {      1,        1,        1,        1,       1,       0      },
    /* MINUS_COMP        */ {      1,        1,        1,        2,       2,       0      },
    /* MUL_COMP          */ {      1,        1,        1,        0,       0,       0      },
    /* DIV_COMP          */ {      1,        1,        1,        0,       0,       0      },
    /* REMAINDER_COMP    */ {      1,        1,        1,        0,       0,       0      },
    /* BIT_AND_COMP      */ {      1,        1,        1,        0,       0,       0      },
    /* BIT_OR_COMP       */ {      1,        1,        1,        0,       0,       0      },
    /* BIT_XOR_COMP      */ {      1,        1,        1,        0,       0,       0      },
    /* LOG_AND_COMP      */ {      1,        1,        1,        1,       1,       1      },
    /* LOG_OR_COMP       */ {      1,        1,        1,        1,       1,       1      },
    /* EUQAL_COMP        */ {      1,        1,        1,        3,       3,       3      },
    /* DIFF_COMP         */ {      1,        1,        1,        3,       3,       3      },
    /* LESS_COMP         */ {      1,        1,        1,        3,       3,       3      },
    /* GREAT_COMP        */ {      1,        1,        1,        3,       3,       3      },
    /* LEQ_COMP          */ {      1,        1,        1,        3,       3,       3      },
    /* GEQ_COMP          */ {      1,        1,        1,        3,       3,       3      },
    /* RSHIFT_COMP       */ {      1,        1,        1,        4,       4,       0      },
    /* LSHIFT_COMP       */ {      1,        1,        1,        4,       4,       0      },
    /* ASSIGN_COMP       */ {      1,        1,        1,        1,       1,       5      },
    /* ADD_ASSIGN_COMP   */ {      1,        1,        1,        6,       6,       0      },
    /* SUB_ASSIGN_COMP   */ {      1,        1,        1,        6,       6,       0      },
                            /*    INT       CHAR      PNT                                 */
    /* UN_PLUS_COMP      */ {      1,        1,        0,        0,       0,       0      },
    /* UN_MINUS_COMP     */ {      1,        1,        0,        0,       0,       0      },
    /* UN_DEFERENCE_COMP */ {      0,        0,        1,        0,       0,       0      },
    /* UN_INC_COMP       */ {      1,        1,        1,        0,       0,       0      },
    /* UN_DEC_COMP       */ {      1,        1,        1,        0,       0,       0      },
    /* UN_BIT_NOT_COMP   */ {      1,        1,        0,        0,       0,       0      },
    /* UN_LOG_NOT_COMP   */ {      1,        1,        1,        0,       0,       0      },
    /* UN_ADDRESS_COMP   */ {      1,        1,        1,        0,       0,       0      },
};
const int types_fusion[3][3] = {
    { INT_INT,      INT_CHAR,    POINTER_INT     },
    { INT_CHAR,     CHAR_CHAR,   POINTER_CHAR    },
    { POINTER_INT, POINTER_CHAR, POINTER_POINTER },
};

Error_list checkTypeMissmatch(int result_to_analyze, struct var_type *var1, struct var_type *var2, int var1_type, int var2_type){
    Error_list error = NO_ERROR;
    switch(result_to_analyze){
        case 2: //pointer/(int/char) subtraction

            break;
        case 3: //pointer/(int/char/pointer) comparison

            break;
        case 4: //pointer/(int/char) shift

            break;
        case 5: //pointer/pointer assignment

            break;
        case 6: //pointer/(int/char) (add/sub)assign

            break;
    }

    return error;
}
Error_list matchTypes(int operation, struct var_type *var1, struct var_type *var2){
    int var1_type, var2_type, result;
    if(var1->pointers)
        { var1_type = POINTER_COMP; }
    else if(var1->type == TIPOS_INT)
        { var1_type = INT_COMP; }
    else
        { var1_type = CHAR_COMP; }

    if(var2->pointers)
        { var2_type = POINTER_COMP; }
    else if(var2->type == TIPOS_INT)
        { var2_type = INT_COMP; }
    else
        { var2_type = CHAR_COMP; }

    if(operation >= UN_PLUS_COMP){
        result = compatibility_table[operation][var1_type];
    }else{
        result = compatibility_table[operation][types_fusion[var1_type][var2_type]];
    }
    switch(result){
        case  0: return 0/*SOME_ERROR*/;
        case  1: return NO_ERROR;
        default: return checkTypeMissmatch(result, var1, var2, var1_type, var2_type);
    }
}
