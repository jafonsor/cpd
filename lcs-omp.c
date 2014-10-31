#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

typedef short unsigned CellVal;

#define max(a,b) (a > b)? a : b
#define min(a,b) (a < b)? a : b

// square. represents a block of the matrix
typedef struct square {
	int tabX;
	int tabY;
	int x_min;
	int x_max;
	int y_min;
	int y_max;
} Square;


void cell_limits(int tab_coord, int max_tab_coord, int extra_remainder, int n_mat_coords_per_cell, int *coord_min, int *coord_max) {
	int min = n_mat_coords_per_cell * tab_coord;
	*coord_min = max(n_mat_coords_per_cell * tab_coord, 1);
	*coord_max = min + n_mat_coords_per_cell;
	if(tab_coord == max_tab_coord - 1) {
		*coord_max += extra_remainder;
	}
}

int extra_remainderX;
int n_mat_coords_per_cellX;

void cell_limitX(int tab_coord, int max_tab_coord, int  *coord_min, int *coord_max) {
	cell_limits(tab_coord, max_tab_coord, extra_remainderX, n_mat_coords_per_cellX, coord_min, coord_max);
}

int extra_remainderY;
int n_mat_coords_per_cellY;

void cell_limitY(int tab_coord, int max_tab_coord, int  *coord_min, int *coord_max) {
	cell_limits(tab_coord, max_tab_coord, extra_remainderY, n_mat_coords_per_cellY, coord_min, coord_max);
}

// calculates the block limits based on the auxiliar table coordinates
Square * new_square(int x, int y, int matx_max, int maty_max, int tabx_max, int taby_max) {
	Square * new_square = (Square*)malloc(sizeof(Square));
	new_square->tabX = x;
	new_square->tabY = y;
	cell_limitX(x, tabx_max, &new_square->x_min, &new_square->x_max);
	cell_limitY(y, taby_max, &new_square->y_min, &new_square->y_max);
	return new_square;
}

// stack. used to store the blocks that can be processed
typedef struct stack {
	Square ** squares;
	int max_size;
	int cursor;
	int current_size;
} Stack;

Stack * new_stack(int max_size) {
	Stack * result = (Stack*)malloc(sizeof(Stack));
	result->max_size = max_size;
	result->cursor = 0;
	result->current_size = 0;
	result->squares = (Square**)malloc(sizeof(Square*)*max_size);
	return result;
}

void delete_stack(Stack * stack) {
	free(stack->squares);
	free(stack);
}

void push(Stack * stack, Square * elem) {
	if(stack->current_size == stack->max_size)
		printf("stack size exceeded\n");
	int cursor = stack->cursor;
	stack->squares[cursor] = elem;
	stack->cursor = (cursor + 1) % stack->max_size;
	stack->current_size++;
}

Square * pop(Stack * stack) {
	if(stack->current_size == 0)
		printf("pop with no elements on stack\n");
	stack->cursor = (stack->cursor - 1) % stack->max_size;
	stack->current_size--;
	return stack->squares[stack->cursor];
}

int empty(Stack * stack) {
	return !stack->current_size;
}

CellVal ** allocArray(int xdim, int ydim) {
	short unsigned **array = (CellVal**)malloc(sizeof(CellVal*)*xdim);
	int x;
	for(x = 0; x < xdim; x++) {
		array[x] = (CellVal*)malloc(sizeof(CellVal)*ydim);
		memset(array[x], 0, sizeof(CellVal) * ydim);
	}
	return array;
}

// Searches the auxiliar table for a value that is different from 3.
// If all the values of the table are 3 then the computation is finished.
int notFinishedTab(CellVal **table, int tabx_max, int taby_max) {
	int i,j;
	for(i = 0; i < tabx_max; i++) {
		for(j = 0; j < taby_max; j++) {
			if(table[i][j] != 3)
				return 1;
		}
	}
	return 0;
}


// InputInfo. Used to store the info present on the input file
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

int main(int argc, char **argv) {
	//printf("n_threads: %d\n", omp_get_num_threads());	

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

	int i,j;


	CellVal **matrix = allocArray(matx_max, maty_max);

	// define the number of cells on the auxiliar matrix
	int block_x_size = (matx_max > 20)?   20 : matx_max / 5;
	int block_y_size = (maty_max > 300)? 300 : maty_max / 5;
	int tabx_max =  matx_max / block_x_size;
	int taby_max =  maty_max / block_y_size;

	CellVal **table = allocArray(tabx_max, taby_max);

	// initialize table: 1 for the sides, 2 for the 1st square, 0 for the rest
	for(i = 0; i < tabx_max; i++) {
		for(j = 0; j < taby_max; j++) {
			if(i==0 && j==0)
				table[i][j] = 2;
			else if(i==0 || j==0)
				table[i][j] = 1;
			else
				table[i][j] = 0;
		}
	}

	// initialize the global vars that are used on the generation of new blocks
	int remainderX         = matx_max % block_x_size;
	int extra_per_cellX    = remainderX / tabx_max;
	extra_remainderX       = remainderX % tabx_max;
	n_mat_coords_per_cellX = block_x_size + extra_per_cellX;

	int remainderY         = maty_max % block_y_size;
	int extra_per_cellY    = remainderY / taby_max;
	extra_remainderY       = remainderY % taby_max;
	n_mat_coords_per_cellY = block_y_size + extra_per_cellY;

	// initialize the block stack with the first block to be processed
	Stack * stack = new_stack(max(tabx_max, taby_max));
	push(stack, new_square(0, 0, matx_max, maty_max, tabx_max, taby_max));
	
	Square * square = NULL; // holds the block for processing
	CellVal x,y;
	int notFinished = 1; // false

#pragma omp parallel private(square, x, y, notFinished)
{	
	notFinished = 1;
	while(notFinished) {
		square = NULL;

		// wait for a block to process
		while(square == NULL && notFinished) {
			#pragma omp critical
			{					
				if(!empty(stack)) {
					square = pop(stack);
					table[square->tabX][square->tabY]++;
				}
			}
			if(square == NULL)
				notFinished = notFinishedTab(table, tabx_max, taby_max);
		}
		
		if(!notFinished)
			break;

		// process the block
		for(x = square->x_min; x < square->x_max; x++) {
			for(y = square->y_min; y < square->y_max; y++) {
				calc(x,y, matrix, inputInfo->X, inputInfo->Y);
			}
		}

		// update the auxiliar table and add to the stack the blocks that now can be processed
		#pragma omp critical
		{
			if(square->tabX + 1 < tabx_max) {
				table[square->tabX + 1][square->tabY]++;
				if(table[square->tabX + 1][square->tabY] == 2) {
					push(stack, new_square(square->tabX + 1, square->tabY, matx_max, maty_max, tabx_max, taby_max));
				}
			}
			if(square->tabY +1 < taby_max) {
				table[square->tabX][square->tabY + 1]++;
				if(table[square->tabX][square->tabY + 1] == 2) {
					push(stack, new_square(square->tabX, square->tabY + 1, matx_max, maty_max, tabx_max, taby_max));
				}
			}
		}

		free(square);
		notFinished = notFinishedTab(table, tabx_max, taby_max);
	}
}
	free(table);
	delete_stack(stack);
	
	//
	lcs_result = lcs(matrix, inputInfo->X, inputInfo->Y, matx_max, maty_max);
	printf("%d\n", matrix[matx_max-1][maty_max-1]);
	printf("%s\n", lcs_result);
	return 0;
}
