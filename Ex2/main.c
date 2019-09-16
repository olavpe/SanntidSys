#include <stdlib.h>
#include <stdio.h>

#include "array.h"

int main(int argc, char* argv[])
{
	/*
	long xy_size    = 1000*1000*1000;       // 8 GB (sizeof(long) = 8 bytes) 
	long x_dim      = 10000; 
	long y_dim      = xy_size/x_dim;    
	long** matrix   = malloc(y_dim*sizeof(long*)); 

	for(long y = 0; y < y_dim; y++){     
		matrix[y] = malloc(x_dim*sizeof(long)); 
		memset(matrix[y], 0, x_dim*sizeof(long));
	} 
	
	printf("Allocation complete (press any key to continue...)\n"); getchar(); 
	*/

	Array newArray = array_new(3);
	
	array_insertBack(&newArray,5);
	array_insertBack(&newArray,4);
	array_insertBack(&newArray,5);
	array_print(newArray);
	array_insertBack(&newArray,6);
	array_print(newArray);
	printf("ARRAY SIZE FINAL: %ld\n",newArray.capacity);
	array_destroy(newArray);
	return 0;
}
