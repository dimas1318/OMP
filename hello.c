#include <stdio.h>
#include <omp.h>

int main(int argc, char const *argv[])
{
	int id;
	//omp_set_num_threads(27);

	#pragma omp parallel 
	{
		id = omp_get_thread_num();
		printf("Hello, Dima!, said %d\n", id);
	}
	return 0;
}