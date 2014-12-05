
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

typedef short unsigned CellVal;

#define max(a,b) (a > b)? a : b
#define min(a,b) (a < b)? a : b

typedef struct file_info {
	int size_x;
	int size_y;
	char * X;
	char * Y;
} InputInfo;

short cost(int x) {
	int i, n_iter = 20;
	double dcost = 0;
	for(i = 0; i < n_iter; i++)
	dcost += pow(sin((double) x),2) + pow(cos((double) x),2);
	return (short) (dcost / n_iter + 0.1);
}

void reverse(char * str) {
	int len = strlen(str);
	int m = len / 2;
	int i;
	char tmp;
	for(i = 0; i < m; i++) {
		tmp = str[i];
		str[i] = str[len-1-i];
		str[len-1-i] = tmp;
	}
}

CellVal ** allocArray(int xdim, int ydim) {
	// alloc all memory for the 2D array
	CellVal *array_block = (CellVal*)malloc(sizeof(CellVal) * xdim * ydim);
	memset(array_block, 0, sizeof(CellVal) * xdim * ydim);

	// create a pointer array to the beginning of the lines
	CellVal **array = (CellVal**)malloc(sizeof(CellVal*)*xdim);
	
	int x;
	for(x = 0; x < xdim; x++) {
		array[x] = &array_block[x * ydim];
	}
	return array;
}

// calculate the value for the matrix cell at (x,y)
void calc(int x, int y, CellVal **matrix, char * X, char * Y) {
	if (x == 0 || y == 0)
		matrix[x][y] = 0;    
	else if (X[x-1] == Y[y-1])
		matrix[x][y] = matrix[x-1][y-1] + cost(y);
	else
		matrix[x][y] = max(matrix[x-1][y], matrix[x][y-1]);
}

// calculates the lcs based on the previeously filled matrix
char * lcs(CellVal **matrix, char * X, char * Y, int matx_max, int maty_max) {
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

int main(int argc, char *argv[]){ 

   char * lcs_result = NULL;
   InputInfo * inputInfo = NULL;
   CellVal x,y;
   
   	// read input
	inputInfo = readInput(argv[1]);
	if(inputInfo == NULL) {
		printf("input info is NULL\n");
		return -1;
	} 

	int matx_max = inputInfo->size_x;
    int maty_max = inputInfo->size_y;    
    CellVal **matrix = allocArray(matx_max, maty_max);
    
   	for(x = 0; x < matx_max; x++) {
	   for(y = 0; y < maty_max; y++) {
		   calc(x,y, matrix, inputInfo->X, inputInfo->Y);
	   }
	}   
   
    lcs_result = lcs(matrix, inputInfo->X, inputInfo->Y, matx_max, maty_max);
    printf("%d\n", matrix[matx_max-1][maty_max-1]);
    printf("%s\n", lcs_result);     		              
         
   
  return 0;
}
