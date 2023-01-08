/*
 * Name: 马江岩
 * ID: 2000012707
 * 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    int i, j, k, l;
    int a0, a1, a2, a3, a4, a5, a6, a7;

    if (M == 32 && N == 32)
        for (i = 0; i < 32; i += 8)
            for (j = 0; j < 32; j += 8)
            {
                for (k = 0; k < 8; k++)
                {
                    a0 = A[i + k][j];
                    a1 = A[i + k][j + 1];
                    a2 = A[i + k][j + 2];
                    a3 = A[i + k][j + 3];
                    a4 = A[i + k][j + 4];
                    a5 = A[i + k][j + 5];
                    a6 = A[i + k][j + 6];
                    a7 = A[i + k][j + 7];
                    B[j + k][i] = a0;
                    B[j + k][i + 1] = a1;
                    B[j + k][i + 2] = a2;
                    B[j + k][i + 3] = a3;
                    B[j + k][i + 4] = a4;
                    B[j + k][i + 5] = a5;
                    B[j + k][i + 6] = a6;
                    B[j + k][i + 7] = a7;
                }
                for (k = 0; k < 8; k++)
                    for (l = k + 1; l < 8; l++)
                    {
                        a0 = B[j + k][i + l];
                        B[j + k][i + l] = B[j + l][i + k];
                        B[j + l][i + k] = a0;
                    }
            }
    
    else if (M == 64 && N == 64)
        for (i = 0; i < 64; i += 8)
        {
            for (j = 0; j < 4; j++)
                for (k = 0; k < 8; k++)
                    B[i + j][8 * (i == 0) + k] = A[i + j + 4][i + k];
            for (j = 0; j < 4; j++)
            {
                a0 = A[i + j][i];
                a1 = A[i + j][i + 1];
                a2 = A[i + j][i + 2];
                a3 = A[i + j][i + 3];
                a4 = A[i + j][i + 4];
                a5 = A[i + j][i + 5];
                a6 = A[i + j][i + 6];
                a7 = A[i + j][i + 7];
                B[i + j][i] = a0;
                B[i + j][i + 1] = a1;
                B[i + j][i + 2] = a2;
                B[i + j][i + 3] = a3;
                B[i + j][i + 4] = a4;
                B[i + j][i + 5] = a5;
                B[i + j][i + 6] = a6;
                B[i + j][i + 7] = a7;
            }
            for (k = i; k < i + 4; k++)
                for (l = k + 1; l < i + 4; l++)
                {
                    a0 = B[k][l];
                    B[k][l] = B[l][k];
                    B[l][k] = a0;
                    a1 = B[k][l + 4];
                    B[k][l + 4] = B[l][k + 4];
                    B[l][k + 4] = a1;
                }
            for (k = i; k < i + 4; k++)
                for (l = k - i + 1; l < 4; l++)
                {
                    a0 = B[k][l + 8 * (i == 0)];
                    a1 = B[i + l][k - i + 8 * (i == 0)];
                    B[k][l + 8 * (i == 0)] = a1;
                    B[i + l][k - i + 8 * (i == 0)] = a0;
                    a2 = B[k][l + 8 * (i == 0) + 4];
                    a3 = B[i + l][k - i + 8 * (i == 0) + 4];
                    B[k][l + 8 * (i == 0) + 4] = a3;
                    B[i + l][k - i + 8 * (i == 0) + 4] = a2;
                }
            for (k = i; k < i + 4; k++)
                for (l = 0; l < 4; l++)
                {
                    a0 = B[k][l + 8 * (i == 0)];
                    a1 = B[k][i + 4 + l];
                    B[k][l + 8 * (i == 0)] = a1;
                    B[k][i + 4 + l] = a0;
                }
            for (k = i; k < i + 4; k++)
                for (l = 0; l < 8; l++)
                    B[k + 4][i + l] = B[k][l + 8 * (i == 0)];
            for (j = 0; j < 64; j += 8)
            {
                if (j == i)continue;
                for (k = 0; k < 4; k++)
                    for (l = 0; l < 4; l++)
                    {
                        B[i + l][j + k] = A[j + k][i + l];
                        B[i + l][j + k + 4] = A[j + k][i + l + 4];
                    }
                for (k = 0; k < 4; k++)
                {
                    a0 = A[j + 4][i + k];
                    a1 = A[j + 5][i + k];
                    a2 = A[j + 6][i + k];
                    a3 = A[j + 7][i + k];
                    a4 = B[i + k][j + 4];
                    a5 = B[i + k][j + 5];
                    a6 = B[i + k][j + 6];
                    a7 = B[i + k][j + 7];
                    B[i + k][j + 4] = a0;
                    B[i + k][j + 5] = a1;
                    B[i + k][j + 6] = a2;
                    B[i + k][j + 7] = a3;
                    B[i + k + 4][j] = a4;
                    B[i + k + 4][j + 1] = a5;
                    B[i + k + 4][j + 2] = a6;
                    B[i + k + 4][j + 3] = a7;
                }
                for (k = 4; k < 8; k++)
                {
                    B[i + 4][j + k] = A[j + k][i + 4];
                    B[i + 5][j + k] = A[j + k][i + 5];
                    B[i + 6][j + k] = A[j + k][i + 6];
                    B[i + 7][j + k] = A[j + k][i + 7];
                }
            }
        }
    
    else if (M == 60 && N == 68)
    {
        for (i = 0; i < 68; i += i % 44 ? 20 : 24)
            for (j = 0; j < 60; j += 4)
                for (k = i; k < i + (i % 44 ? 20 : 24) && k < 68; k += 2)
                {
                    a0 = A[k][j];
                    a1 = A[k][j + 1];
                    a2 = A[k][j + 2];
                    a3 = A[k][j + 3];
                    a4 = A[k + 1][j];
                    a5 = A[k + 1][j + 1];
                    a6 = A[k + 1][j + 2];
                    a7 = A[k + 1][j + 3];
                    B[j][k] = a0;
                    B[j + 1][k] = a1;
                    B[j + 2][k] = a2;
                    B[j + 3][k] = a3;
                    B[j][k + 1] = a4;
                    B[j + 1][k + 1] = a5;
                    B[j + 2][k + 1] = a6;
                    B[j + 3][k + 1] = a7;
                }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


