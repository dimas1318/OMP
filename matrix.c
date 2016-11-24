#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define DEFAULT_SIZE 10
#define DEFAULT_NUM_THREADS 4

int isRight(int* arr, int size){
	for(int i = 0; i < size; i++){
		for(int j = 0; j < size; j++){
			if(*(arr + i*size + j) != size){
				return 0;
			}
		}
	}
	return 1;
}

int main(int argc, char* argv[]){
	int size = DEFAULT_SIZE;
	int num_threads = DEFAULT_NUM_THREADS;

	if(argc > 1){
		size = atoi(argv[1]);
		if(argc > 2){
			num_threads = atoi(argv[2]);
			if(num_threads > omp_get_max_threads()){
				num_threads = omp_get_max_threads();
			}
		} 
	}

	int A[size][size];
	int B[size][size];
	int C[size][size];
	int D[size][size];
	int E[size][size];

	int i, j, k;
	double start_time, calculation_time;
	int sum;

	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){
			A[i][j] = 1;
			B[i][j] = 1;
			C[i][j] = 0;
			D[i][j] = 0;
			E[i][j] = 0;
		}
	}

	#pragma omp parallel private(i, j, k, sum) num_threads(num_threads)
	{		
		#pragma omp master
		{
			start_time = omp_get_wtime();
		}

		#pragma omp for collapse(2)
		for(i = 0; i < size; i++){
			for(j = 0; j < size; j++){
				sum = 0;
				for(k = 0; k < size; k++){
					sum += A[i][k] * B[k][j];
				}
				C[i][j] = sum;
			}
		}

		#pragma omp master
		{
			calculation_time = omp_get_wtime() - start_time;
			printf("calculation time is %f sec(parallel 2*n^3)\n", calculation_time);		
		}	
	}	

	int new_size = size / 2;
	int A11[new_size][new_size], A12[new_size][new_size], A21[new_size][new_size], 
		A22[new_size][new_size], B11[new_size][new_size], B12[new_size][new_size],
		B21[new_size][new_size], B22[new_size][new_size];
	int M1[new_size][new_size], M2[new_size][new_size], M3[new_size][new_size], 
		M4[new_size][new_size], M5[new_size][new_size], M6[new_size][new_size], 
		M7[new_size][new_size];
	for(int i = 0; i < new_size; i++){
		for(int  j = 0; j < new_size; j++){
			A11[i][j] = A[i][j];
			A12[i][j] = A[i + new_size][j];
			A21[i][j] = A[i][j + new_size];
			A22[i][j] = A[i + new_size][j + new_size];
			B11[i][j] = B[i][j];
			B12[i][j] = B[i + new_size][j];
			B21[i][j] = B[i][j + new_size];
			B22[i][j] = B[i + new_size][j + new_size];
		}
	}
	for(int i = 0; i < new_size; i++){
		for(int  j = 0; j < new_size; j++){
			M1[i][j] = 0;
			M2[i][j] = 0;
			M3[i][j] = 0;
			M4[i][j] = 0;
			M5[i][j] = 0;
			M6[i][j] = 0;
			M7[i][j] = 0;
		}
	}

	#pragma omp parallel private(i, j, k) num_threads(num_threads)
	{
		#pragma omp master
		{
			start_time = omp_get_wtime();
		}

		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for(j = 0; j < new_size; j++){
				for(k = 0; k < new_size; k++){
					M1[i][j] += (A11[i][k] + A22[i][k]) * (B11[k][j] + B22[k][j]);
				} 
			}
		}
		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for(j = 0; j < new_size; j++){
				for(k = 0; k < new_size; k++){
					M2[i][j] += (A21[i][k] + A22[i][k]) * B11[k][j];
				} 
			}
		}
		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for(j = 0; j < new_size; j++){
				for(k = 0; k < new_size; k++){
					M3[i][j] += A11[i][k] * (B12[k][j] - B22[k][j]);
				} 
			}
		}
		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for(j = 0; j < new_size; j++){
				for(k = 0; k < new_size; k++){
					M4[i][j] += A22[i][k] * (B21[k][j] - B11[k][j]);
				} 
			}
		}
		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for(j = 0; j < new_size; j++){
				for(k = 0; k < new_size; k++){
					M5[i][j] += (A11[i][k] + A12[i][k]) * B22[k][j];
				} 
			}
		}
		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for(j = 0; j < new_size; j++){
				for(k = 0; k < new_size; k++){
					M6[i][j] += (A21[i][k] - A11[i][k]) * (B11[k][j] + B12[k][j]);
				} 
			}
		}
		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for(j = 0; j < new_size; j++){
				for(k = 0; k < new_size; k++){
					M7[i][j] += (A12[i][k] - A22[i][k]) * (B21[k][j] + B22[k][j]);
				} 
			}
		}

	//#pragma omp barrier

		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for (j = 0; j < new_size; j++){
				E[i][j] += M1[i][j] +  M4[i][j] - M5[i][j] + M7[i][j];
			}
		}

		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for (j = 0; j < new_size; j++){
				E[i + new_size][j] += M3[i][j] +  M5[i][j];
			}
		}

		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for (j = 0; j < new_size; j++){
				E[i][j + new_size] += M2[i][j] + M4[i][j];
			}
		}

		#pragma omp for collapse(2)
		for(i = 0; i < new_size; i++){
			for (j = 0; j < new_size; j++){
				E[i + new_size][j + new_size] += M1[i][j] - M2[i][j] + M3[i][j] + M6[i][j];
			}
		}

		#pragma omp master
		{
			calculation_time = omp_get_wtime() - start_time;
			printf("calculation time is %f sec(parallel n^2.8074)\n", calculation_time);		
		}
	}
	
    #pragma omp master
	{
		start_time = omp_get_wtime();
		for(i = 0; i < size; i++){
			for(j = 0; j < size; j++){
				sum = 0;
				for(k = 0; k < size; k++){
					sum += A[i][k] * B[k][j];
				}
				D[i][j] = sum;
			}
		}
		calculation_time = omp_get_wtime() - start_time;
		printf("calculation time is %f sec\n", calculation_time);
	}

	#pragma omp master
	{
		if(isRight(&C[0][0], size)){
			printf("all right(parallel)\n");
		} else {
			printf("wrong answer\n");
		}

		if(isRight(&E[0][0], size)){
			printf("all right(strassen)\n");
		} else {
			printf("wrong answer\n");
		}

		if(isRight(&D[0][0], size)){
			printf("all right(single)\n");
		} else {
			printf("wrong answer\n");
		}
	}
	/*#pragma omp master
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){
				printf("%d ", E[i][j]);
		}
		printf("\n");
	}*/
	
	return 0;
}