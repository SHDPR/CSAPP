/* 
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

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int r_idx, c_idx, r_blk, c_blk;
	int idx;
	int buffer[3];
	int buff[8];
	
	if(N != 64){
		buffer[2] = 0;
		// Blocking size : 32 * 8
		for(r_blk = 0; r_blk < N; r_blk += 32){
			for(c_blk = 0; c_blk < M; c_blk += 8){
				for(r_idx = r_blk; r_idx < r_blk + 32 && r_idx < N; r_idx++){
					for(c_idx = c_blk; c_idx < c_blk + 8 && c_idx < M; c_idx++){
						if(r_idx != c_idx){
							B[c_idx][r_idx] = A[r_idx][c_idx];
						}
						else{
							buffer[0] = A[r_idx][c_idx];
							buffer[1] = r_idx;
							buffer[2] = 1;
						}
					}
					if(buffer[2]){
						B[buffer[1]][buffer[1]] = buffer[0];
						buffer[2] = 0;
					}
				}
			}
		}
	}
	else{
		buffer[2] = 0;

		for(r_blk = 0; r_blk < N; r_blk += 8){
			for(c_blk = 0; c_blk < M; c_blk += 8){
				for(r_idx = r_blk; r_idx < r_blk + 4;  r_idx++){
					for(c_idx = c_blk; c_idx < c_blk + 4; c_idx++){
						if(r_idx != c_idx){
							B[c_idx][r_idx] = A[r_idx][c_idx];
						}
						else{
							buffer[0] = A[r_idx][c_idx];
							buffer[1] = r_idx;
							buffer[2] = 1;
						}
					}
					if(buffer[2]){
						B[buffer[1]][buffer[1]] = buffer[0];
						buffer[2] = 0;
					}
					
					for(c_idx = c_blk + 4; c_idx < c_blk + 8; c_idx++){
						if(r_idx != (c_idx-4)){
							B[c_idx-4][r_idx + 4] = A[r_idx][c_idx];
						}
						else{
							buffer[0] = A[r_idx][c_idx];
							buffer[1] = r_idx;
							buffer[2] = 1;
						}						
												
					}
					
					if(buffer[2]){
						B[buffer[1]][buffer[1] + 4] = buffer[0];
						buffer[2] = 0;
					}
				}
				
				for(r_idx = r_blk + 4; r_idx < r_blk + 8;  r_idx++){
					for(c_idx = c_blk; c_idx < c_blk + 4; c_idx++){
						if((r_idx-4) != c_idx){
							B[c_idx + 4][r_idx - 4] = A[r_idx][c_idx];
						}
						else{
							buffer[0] = A[r_idx][c_idx];
							buffer[1] = r_idx;
							buffer[2] = 1;
						}						
					}
					if(buffer[2]){
						B[buffer[1]][buffer[1]-4] = buffer[0];
						buffer[2] = 0;
					}				
					
					for(c_idx = c_blk + 4; c_idx < c_blk + 8; c_idx++){
						if(r_idx != c_idx){
							B[c_idx][r_idx] = A[r_idx][c_idx];
						}
						else{
							buffer[0] = A[r_idx][c_idx];
							buffer[1] = r_idx;
							buffer[2] = 1;
						}						
					}
					
					if(buffer[2]){
						B[buffer[1]][buffer[1]] = buffer[0];
						buffer[2] = 0;
					}
				}
				
				
				for(idx = 0; idx < 4; idx++){

					
					buff[4] = B[c_blk + 4 + idx][r_blk + 0];
					buff[5] = B[c_blk + 4 + idx][r_blk + 1];
					buff[6] = B[c_blk + 4 + idx][r_blk + 2];
					buff[7] = B[c_blk + 4 + idx][r_blk + 3];
					
					buff[0] = B[c_blk + idx][r_blk + 4];
					buff[1] = B[c_blk + idx][r_blk + 5];
					buff[2] = B[c_blk + idx][r_blk + 6];
					buff[3] = B[c_blk + idx][r_blk + 7];
					
					
					B[c_blk + idx][r_blk + 4] = buff[4];
					B[c_blk + idx][r_blk + 5] = buff[5];
					B[c_blk + idx][r_blk + 6] = buff[6];
					B[c_blk + idx][r_blk + 7] = buff[7];

					B[c_blk + 4 + idx][r_blk + 0] = buff[0];
					B[c_blk + 4 + idx][r_blk + 1] = buff[1];
					B[c_blk + 4 + idx][r_blk + 2] = buff[2];
					B[c_blk + 4 + idx][r_blk + 3] = buff[3];
				}
				
				
			}
		}
	}
	
	
	
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

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

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

