#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include"algebra.h"

float ***lu_decomposition(float **srs_matrix, int matrix_size);
float calculateDeterminant(float **lower, float **upper, int matrix_size);
int linearSystemSolutionSet(float ***lu, float **result, int size);

typedef struct matrix{
    float **m;
    int rows;
    int columns;
}matrix_t;

matrix_t *matrix_g = NULL;

Matrix matrixNew(TreeNode root){
    matrix_t *m = malloc(sizeof(matrix_t));
    if(!m)
        { goto ERROR; }

    TreeNode auxl = root, auxr;
    
    m->m = calloc(MATRIX_MAX_SIZE, sizeof(float*));
    if(!m->m)
        { goto ERROR; }

    for(int i=0;i<MATRIX_MAX_SIZE;i++){
        m->m[i] = calloc(MATRIX_MAX_SIZE, sizeof(float*));
        if(!m->m[i])
            { goto ERROR; }
    }
    m->rows = 0;
    m->columns = 0;

    int i=0, j=0;
    while(auxl){
        auxr = auxl;
        while(auxr){
            if(i >= MATRIX_MAX_SIZE || j >= MATRIX_MAX_SIZE){
                printf("\nERROR: Matrix limits out of boundaries.\n\n");
                goto ERROR;
            }
            m->m[i][j] = nodeGetValue(auxr);
            j++;
            if(j > m->columns)
                m->columns = j;
            auxr = nodeGetRight(auxr);
        }
        i++;
        m->rows = i;
        j=0;
        auxl = nodeGetLeft(auxl);
    }

    deleteTree(root);

    return m;

ERROR:
    matrixDelete(m);
    deleteTree(root);
    return NULL;

}

void matrixGlobalSet(Matrix m){
    matrix_t *mat = m;
    if(!mat)
        { return; }
    if(matrix_g)
        { matrixDelete(matrix_g); }

    matrix_g = mat;
}
int matrixSetElement(Matrix mat, int row, int column, float num){
    matrix_t *m = mat;
    if(row <= m->rows || column <= m->columns){
        m->m[row][column] = num;
        return 0;
    }else{
        return -1;
    }
}
int matrixSetRows(Matrix mat, int rows){
    matrix_t *m = mat;
    if(rows > 0 && rows < 10){
        m->rows = rows;
        return 0;
    }
    return -1;
}
int matrixSetColumns(Matrix mat, int columns){
    matrix_t *m = mat;
    if(columns > 0 && columns < 10){
        m->columns = columns;
        return 0;
    }
    return -1;
}

float matrixSolveDeterminant(Matrix m){
    matrix_t *mat = m;
    if(!mat){
        if(!matrix_g){
            printf("\nNo Matrix defined!\n\n");
            return 0.0;
        }
        mat = matrix_g;
    }

    if(mat->rows != mat->columns){
        printf("\nMatrix format incorrect!\n\n");
        return 0.0;
    }

    float ***lu = lu_decomposition(mat->m, mat->rows);
    if(!lu){
        printf("\nAllocation error: Solve determinant\n\n");
        return 0.0;
    }

    float result = calculateDeterminant(lu[0], lu[1], mat->rows);

    printf("\n%.6f\n\n", result);

    /* Área para desalocação das matrizes */
    for(int i=0;i < mat->rows;i++){
        free(lu[0][i]);
        free(lu[1][i]);
    }
    free(lu[0]);
    free(lu[1]);
    free(lu);

    return result;
}
void matrixSolveLinearSystem(Matrix m){
    matrix_t *mat = m;
    float ***lu = NULL;
    float *x = NULL;
    float *y = NULL;

    if(!mat){
        if(!matrix_g){
            printf("\nNo Matrix defined!\n\n");
            return;
        }
        mat = matrix_g;
    }

    /* checagem do formato da matriz */
    if(mat->columns != mat->rows + 1){
        printf("\nMatrix format incorrect!\n\n");
        return;
    }

    lu = lu_decomposition(mat->m, mat->rows);
    if(!lu){ 
        printf("\nAllocation error: solve linear system\n\n");
        goto DEALLOC; 
    }

    /* checa o conjunto solução */
    int sol_set = linearSystemSolutionSet(lu, mat->m, mat->rows);
    if(sol_set == -2){
        printf("\nSI - The Linear System has no solution\n\n");
        goto DEALLOC;
    }else if(sol_set == -1){
        printf("\nSPI - The Linear System has infinetly many solutions\n\n");
        goto DEALLOC;
    }

    /* resolve o sistema, caso seja possível e determinado */

    // L*Y = B, L=Lower, Y= U*X, B=vetor resultado
    y = malloc(mat->rows * sizeof(float));
    if(!y){
        printf("\nAllocation error: solve linear system\n\n");
        goto DEALLOC;
    }

    for(int i=0;i < mat->rows;i++){
        y[i] = mat->m[i][mat->columns-1];
        for(int j=0;j < i;j++){
            y[i] -= lu[0][i][j] * y[j];
        }
    }

    // U*X = Y, U=Upper, X=vetor variáveis, Y=vetor resultante
    x = malloc(mat->rows * sizeof(float));
    if(!x){
        printf("\nAllocation error: solve linear system\n\n");
        goto DEALLOC;
    }
    
    x[mat->rows - 1] = y[mat->rows - 1] / lu[1][mat->rows - 1][mat->rows - 1];
    for(int i = mat->rows - 2;i>=0;i--){
        x[i]=0;
        for(int j=i+1;j < mat->rows;j++){  
            x[i] -= lu[1][i][j] * x[j];
        }
        x[i] = (x[i] + y[i]) / lu[1][i][i];
    }

    /* imprime o vetor X */
    printf("\n");
    for(int i=0;i<mat->rows;i++){
        printf("% .6f\n", x[i]);
    }
    printf("\n\n");

/* limpeza de memória na condição de erro de alocação */
DEALLOC:
    free(x);
    free(y);
    if(lu){
        for(int i=0;i < mat->rows;i++){
            free(lu[0][i]);
            free(lu[1][i]);
        }
        free(lu[0]);
        free(lu[1]);
    }
    free(lu);
    return;
}

void matrixShow(Matrix m){
    matrix_t *mat = m;
    if(!mat){
        mat = matrix_g;
        if(!mat){
            printf("\nNo Matrix defined!\n\n");
            return;
        }
    }

    printf("+--");
    for(int i=0;i < (mat->columns * 14)-2;i++)
        { printf(" "); }
    printf("--+\n");

    for(int i=0;i < mat->rows;i++){
        printf("| ");
        for(int j=0;j < mat->columns; j++){
            printf("% .6e ", mat->m[i][j]);
        }
        printf(" |");
        printf("\n");
    }

    printf("+--");
    for(int i=0;i < (mat->columns * 14)-2;i++)
        { printf(" "); }
    printf("--+\n");

}
void matrixDelete(Matrix mat){
    matrix_t *m = mat;
    if(!m)
        { return; }
    for(int i=0;i<MATRIX_MAX_SIZE;i++)
        free(m->m[i]);
    free(m);
}

/* Funções auxiliares */
float calculateDeterminant(float **lower, float **upper, int matrix_size){
    float result = 1;
    for(int i=0;i < matrix_size;i++){
        result *= (lower[i][i]) * (upper[i][i]);
    }
    return result;
}
float*** lu_decomposition(float** src, int n){
    float ***r = NULL;
    float** upper = calloc(n, sizeof(float*));
    if(!upper)
        { return NULL; }
    float** lower = calloc(n, sizeof(float*));
    if(!lower)
        { goto ALLOC_ERROR; }
    for(int i=0;i<n;i++){
        upper[i] = calloc(n, sizeof(float));
        lower[i] = calloc(n, sizeof(float));
        if(!upper[i] || !lower[i])
            { goto ALLOC_ERROR; }
        lower[i][i] = 1.0;
    }

    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            upper[i][j] = src[i][j];

    float pivot = 0;
    for(int row=0;row<n;row++){
        for(int i=row+1;i<n;i++){
            pivot = upper[i][row] / upper[row][row];
            lower[i][row] = pivot;
            for(int j=0;j<n;j++){
                upper[i][j] = upper[i][j] - (pivot * upper[row][j]);
            }
        }
    }
    r = malloc(2*sizeof(float**));
    if(!r)
        { goto ALLOC_ERROR; }
    r[0] = lower;
    r[1] = upper;

    return r;

ALLOC_ERROR:
    printf("\nAllocation error: LU decomposition\n\n");
    free(r);
    for(int i=0;i<n;i++){
        free(upper[i]);
        free(lower[i]);
    }
    free(upper);
    free(lower);
    return NULL;
}

// retorna -2 se SI
// retorna -1 se SPI
// retorna  0 se SPD
int linearSystemSolutionSet(float ***lu, float **result, int size){
    int qt_zeroes;

    for(int i=0;i<size;i++){
        qt_zeroes = 0;
        for(int j=0;j<size;j++){
            if(lu[1][i][j] == 0 || isinff(lu[1][i][j]) || isnanf(lu[1][i][j]))
                { qt_zeroes++; }
            // if(isinff(lu[1][i][j]) || isnanf(lu[1][i][j]))
            //     { return -2; }
        }
        if(qt_zeroes == size){
            float cont = result[i][size];
            for(int c=0;c<i;c++){
                cont = cont - (result[c][size] * lu[0][i][c]);
            }
            if(cont == 0)
                { return -1; }
            return -2;
        }
    }
    return 0;
}

void matrixGlobalDelete(){
    if(matrix_g){
        matrixDelete(matrix_g);
    }
}
