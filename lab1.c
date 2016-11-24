#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

#define ISIZE 6000
#define JSIZE 6000

#define BUFSIZE 100

void compareFiles(int k)
{
	FILE *fp1, *fp2;
	char buf1[BUFSIZE] = {""};
	char buf2[BUFSIZE] = {""};

	if (NULL == (fp1 = fopen("result.txt", "r")) || NULL == (fp2 = fopen("result_2.txt", "r"))){
		printf("Files 1 and %d are Not identical\n", k);
	}

	while (!feof(fp1) && !feof(fp2)){
		fgets(buf1, BUFSIZE - 1, fp1);
		fgets(buf2, BUFSIZE - 1, fp2);
		if (0 != strcmp(buf1, buf2)){
			printf("Files 1 and %d are Not identical\n", k);
			fclose(fp1);
			fclose(fp2);
			return;
		} else {
			continue;
		}
	}
	//printf("Files 1 and %d are identical\n", k);
	fclose(fp1);
	fclose(fp2);
	return;
}

int main(int argc, char** argv)
{

	double start_time, calculation_time;

	double* a = calloc(ISIZE * JSIZE, sizeof(double));
	double* x;
	x = calloc(ISIZE * JSIZE, sizeof(double));
	int i, j;
	FILE *ff;
	int size = omp_get_max_threads();
	double time[size];
	double acceleration[size - 1];
	FILE* file;

	/***task zero***/
	for(int k = 1; k < size + 1; k++){
		#pragma omp parallel num_threads(k)
		{
			#pragma omp master
			{
				start_time = omp_get_wtime();
			}

			#pragma omp for collapse(2)
			for (i=0; i<ISIZE; i++){
				for (j=0; j<JSIZE; j++){
					a[i * ISIZE + j] = 10*i +j;
				}
			}

			#pragma omp for collapse(2)
			for (i=0; i<ISIZE; i++){
				for (j=0; j<JSIZE; j++){
					a[i * ISIZE + j] = sin(0.00001*a[i * ISIZE + j]);
				}
			}

			#pragma omp master
			{
				calculation_time = omp_get_wtime() - start_time;
				printf("%d threads worked %f\n", k, calculation_time);
				time[k - 1] = calculation_time;
				if(k != 1){
					acceleration[k - 2] = time[0] / calculation_time;
				}	

				if(k == 1){
					ff = fopen("result.txt","w");
				} else {
					ff = fopen("result_2.txt", "w");
				}

				for(i=0; i < ISIZE; i++){
					for (j=0; j < JSIZE; j++){
						fprintf(ff,"%f ",a[i * ISIZE + j]);
					}
					fprintf(ff,"\n");
				}
				fclose(ff);	

				if(k != 1){
					//compareFiles(k);
				}
			}
		}
	}
	
	file = fopen("analys0.txt", "w");
	for (int i = 0; i < size - 1; i++){
		fprintf(file, "%f\n", acceleration[i]);	
	}
	fclose(file);

	/***task 1d***/
	for(int k = 1; k < size + 1; k++){
		#pragma omp parallel num_threads(k)
		{
			#pragma omp master
			{
				start_time = omp_get_wtime();
			}

			#pragma omp for collapse(2)
			for (i=0; i<ISIZE; i++){
				for (j=0; j<JSIZE; j++){
					a[i * ISIZE + j] = 10*i +j;
				}
			}

			/*#pragma omp master
			for (i=1; i<ISIZE - 1; i++){
				for (j = 6; j < JSIZE -	1; j++){
					a[i][j] = sin(0.00001*a[i+1][j - 6]); 
				}
			}*/

			#pragma omp for collapse(2)
			for (i=1; i<ISIZE - 1; i++){
				for (j = 6; j < JSIZE -	1; j++){
					x[i * ISIZE + j] = sin(0.00001*a[(i+1) * ISIZE + (j-6)]); 
				}
			}

			#pragma omp for collapse(2)
			for (i=1; i<ISIZE - 1; i++){
				for (j = 6; j < JSIZE -	1; j++){
					a[i * ISIZE + j] = x[i * ISIZE + j];
				}
			}

			#pragma omp master
			{
				calculation_time = omp_get_wtime() - start_time;
				printf("%d threads worked %f\n", k, calculation_time);
				time[k - 1] = calculation_time;
				if(k != 1){
					acceleration[k - 2] = time[0] / calculation_time;
				}	

				if(k == 1){
					ff = fopen("result.txt","w");
				} else {
					ff = fopen("result_2.txt", "w");
				}

				for(i=0; i < ISIZE; i++){
					for (j=0; j < JSIZE; j++){
						fprintf(ff,"%f ",a[i * ISIZE + j]);
					}
					fprintf(ff,"\n");
				}
				fclose(ff);	

				if(k != 1){
					//compareFiles(k);
				}
			}
		}
	}

	file = fopen("analys1.txt", "w");
	for (int i = 0; i < size - 1; i++){
		fprintf(file, "%f\n", acceleration[i]);	
	}
	fclose(file);
	free(a);
	free(x);

#define _ISIZE 6000
#define _JSIZE 6000
	double* _a = calloc(_ISIZE * _JSIZE, sizeof(double));
	double* _b = calloc(_ISIZE * _JSIZE, sizeof(double));
	double* y = calloc(_ISIZE * _JSIZE, sizeof(double));

	/***task 3g***/
	for(int k = 1; k < size + 1; k++){
		#pragma omp parallel num_threads(k)
		{
			#pragma omp master
			{
				start_time = omp_get_wtime();
			}

			#pragma omp for collapse(2)
			for (i=0; i< _ISIZE; i++){
				for (j=0; j< _JSIZE; j++){
					_a[i * _ISIZE + j] = 10*i +j;
					_b[i * _ISIZE + j] = 0.;
				}
			}

			#pragma omp for collapse(2)
			for (i=0; i<_ISIZE; i++){
				for (j = 0; j < _JSIZE; j++){
					_a[i * _ISIZE + j] = sin(0.00001*_a[i * _ISIZE + j]); 
				}
			}
					
			#pragma omp for collapse(2)
			for (i=5; i<_ISIZE; i++){
				for (j = 0; j < _JSIZE - 2; j++){
					x[i * _ISIZE + j] = _a[(i-5) * _ISIZE + (j+2)]*1.5;
				}
			}

			#pragma omp for collapse(2)
			for (i=5; i<_ISIZE; i++){
				for (j = 0; j < _JSIZE - 2; j++){
					_b[i * _ISIZE + j] = x[i * _ISIZE + j]; 
				}
			}

			#pragma omp master
			{
				calculation_time = omp_get_wtime() - start_time;
				printf("%d threads worked %f\n", k, calculation_time);
				time[k - 1] = calculation_time;
				if(k != 1){
					acceleration[k - 2] = time[0] / calculation_time;
				}	

				if(k == 1){
					ff = fopen("result.txt","w");
				} else {
					ff = fopen("result_2.txt", "w");
				}

				for(i=0; i < _ISIZE; i++){
					for (j=0; j < _JSIZE; j++){
						fprintf(ff,"%f ",a[i * _ISIZE + j]);
					}
					fprintf(ff,"\n");
				}
				fclose(ff);	

				if(k != 1){
					//compareFiles(k);
				}
			}
		}
	}

	file = fopen("analys2.txt", "w");
	for (int i = 0; i < size - 1; i++){
		fprintf(file, "%f\n", acceleration[i]);	
	}
	fclose(file);
	free(_a);
	free(_b);
	free(y);

	return 0;
}

//gcc lab1.c -o test.out -lm -fopenmp
//./test.out
//python res.py