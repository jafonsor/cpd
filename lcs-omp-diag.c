#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

int max(int a, int b){
	return (a > b)? a : b;
}

int min(int a, int b){
	return (a < b)? a : b;
}

int ** allocArray(int xdim, int ydim) {
	int **array = (int**)malloc(sizeof(int*)*xdim);
	int x;
	for(x = 0; x < xdim; x++) {
		array[x] = (int*)malloc(sizeof(int)*ydim);
		memset(array[x], (unsigned char)0, sizeof(int) * ydim);
	}
	return array;
}

// read input
typedef struct file_info {
	int size_x;
	int size_y;
	char * X;
	char * Y;
} InputInfo;

InputInfo * readInput(char * fileName) {
	FILE * file = NULL;
	InputInfo * inputInfo = NULL;
	
	if((file = fopen(fileName,"r")) != NULL) {            
		inputInfo = (InputInfo*)malloc(sizeof(InputInfo));
		fscanf(file,"%d %d", &inputInfo->size_x, &inputInfo->size_y);
		inputInfo->size_x++;
		inputInfo->size_y++;
		inputInfo->X = (char*)malloc((inputInfo->size_x+1)*sizeof(char));
		inputInfo->Y = (char*)malloc((inputInfo->size_y+1)*sizeof(char)); 
	
		while((fscanf(file,"%s%s",inputInfo->X,inputInfo->Y)) != EOF) {

		}
		fclose(file);
	} else {
		printf("cannot open file: %s", fileName);
		exit(0);
	}
	return inputInfo;
}

short cost(int x) {
	int i, n_iter = 20;
	double dcost = 0;
	for(i = 0; i < n_iter; i++)
	dcost += pow(sin((double) x),2) + pow(cos((double) x),2);
	return (short) (dcost / n_iter + 0.1);
}

void calc(int x, int y, int **matrix, char * X, char * Y) {
	if (x == 0 || y == 0)
		matrix[x][y] = 0;    
	else if (X[x-1] == Y[y-1])
		matrix[x][y] = matrix[x-1][y-1] + cost(y);
	else
		matrix[x][y] = max(matrix[x-1][y], matrix[x][y-1]);
}

char * lcs(int **matrix, char * X, char * Y, int matx_max, int maty_max) {
	int lcs_len = matrix[matx_max-1][maty_max-1];
	char * lcs = (char*)malloc((lcs_len +1) * sizeof(char));
	lcs[lcs_len] = '\0';
	int x = matx_max - 1;
	int y = maty_max - 1;
	int l = lcs_len;
	while(x > 0 && y > 0) {
		if(X[x-1] == Y[y-1]) {
			lcs[l-1] = X[x-1];
			x--; y--; l--;
		} else if(matrix[x-1][y] > matrix[x][y-1]) {
			x--;
		} else {
			y--;
		}
	}
	return lcs;
}

int main(int argc, char **argv) {
	char * lcs_result = NULL;
	InputInfo * inputInfo = NULL;
	
	// read input
	inputInfo = readInput(argv[1]);
	if(inputInfo == NULL) {
		printf("input info is NULL\n");
		return -1;
	}

	int matx_max = inputInfo->size_x;
	int maty_max = inputInfo->size_y;

	int **matrix = allocArray(matx_max, maty_max);

	int diagonal, i, j, c, l;
	int min_lim, max_lim, chunck;
	for(diagonal = 0; diagonal < matx_max + maty_max - 1; diagonal++) {
		min_lim = max(0, diagonal - matx_max + 1);
		max_lim = min(diagonal+1, maty_max);
		chunck = ((max_lim - min_lim) / 4);
		#pragma omp parallel for private(i, c, l) schedule (guided , 5)
		for( i = min_lim; i < max_lim; i++) {
			c = i;
			l = diagonal - i;
			calc(l, c, matrix, inputInfo->X, inputInfo->Y);
		}
	}



	//print(matrix, matx_max, maty_max);
	lcs_result = lcs(matrix, inputInfo->X, inputInfo->Y, matx_max, maty_max);
	//printf("%d  %d\n", matx_max, maty_max);
	printf("%d\n", matrix[matx_max-1][maty_max-1]);
	printf("%s\n", lcs_result);
	//print(matrix, matx_max, maty_max);
	return 0;
}