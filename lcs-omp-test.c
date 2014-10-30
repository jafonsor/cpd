#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

// square
typedef struct square {
	int tabX;
	int tabY;
	int x_min;
	int x_max;
	int y_min;
	int y_max;
} Square;

void cell_limits(int tab_coord, int max_mat_coord, int max_tab_coord, int  *coord_min, int *coord_max) {
	int remainder = max_mat_coord % max_tab_coord;
	int  n_mat_coords_per_cell = max_mat_coord / max_tab_coord;
	*coord_min = n_mat_coords_per_cell * tab_coord;
	*coord_max = (*coord_min) + n_mat_coords_per_cell;
	if(tab_coord == max_tab_coord - 1) {
		*coord_max += remainder;
	}
}

Square * new_square(int x, int y, int matx_max, int maty_max, int tabx_max, int taby_max) {
	Square * new_square = (Square*)malloc(sizeof(Square));
	new_square->tabX = x;
	new_square->tabY = y;
	cell_limits(x,matx_max, tabx_max, &new_square->x_min, &new_square->x_max);
	cell_limits(y,maty_max, taby_max, &new_square->y_min, &new_square->y_max);
	return new_square;
}

// stack
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





char * Y;

int ** allocArray(int xdim, int ydim) {
	int **array = (int**)malloc(sizeof(int*)*xdim);
	int x;
	for(x = 0; x < xdim; x++) {
		array[x] = (int*)malloc(sizeof(int)*ydim);
		memset(array[x], (unsigned char)0, sizeof(int) * ydim);
	}
	return array;
}

void reverse(char * str) {
	int len = strlen(str);
	int i;
	char tmp;
	for(i = 0; i < len / 2; i++) {
		tmp = str[i];
		str[i] = str[len-1-i];
		str[len-1-i] = tmp;
	}
}

int max(int a, int b){
	return (a > b)? a : b;
}

int min(int a, int b){
	return (a < b)? a : b;
}





Square * squareWith2(int iniX, int iniY, int **table, int tabx_max, int taby_max, int matx_max, int maty_max) {
	int diagonal, i, j;
	for(diagonal = iniX+iniY; diagonal < tabx_max + taby_max - 1; diagonal++)
		for( i = max(0, diagonal - tabx_max + 1); i < min(diagonal+1, taby_max); i++)
		{
			int c = i;
			int l = diagonal - i;
			if(table[l][c] == 2) {
				return new_square(l, c, matx_max, maty_max, tabx_max, taby_max);
			}
		}
	return NULL;
}

int notFinishedTab(int **table, int tabx_max, int taby_max) {
	int i,j;
	for(i = 0; i < tabx_max; i++) {
		for(j = 0; j < taby_max; j++) {
			if(table[i][j] != 3)
				return 1;
		}
	}
	return 0;
}




void print(int **m, int x_max, int y_max) {
	int i, j;   
	for(i = 0; i < x_max; i++) {
		for(j = 0; j < y_max; j++) {
			printf("%d ", m[i][j]);
		}
		printf("\n");
	}
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

	int i,j;

	Y = inputInfo->Y;

	int **table;
	int tabx_max, taby_max;
	int **matrix = allocArray(matx_max, maty_max);

	tabx_max = matx_max / 150;
	taby_max = maty_max / 150;

	// check cell limits
	// //cell_limits(int tab_coord, int max_mat_coord, int max_tab_coord, int  *coord_min, int *coord_max)
	// int min, max;
	// for(i = 0; i < tabx_max; i++) {
	// 	cell_limits(i, inputInfo->size_x, tabx_max, &min, &max);
	// 	printf("%d-%d ", min, max);
	// }
	// printf("\n");
	// for(i = 0; i < taby_max; i++) {
	// 	cell_limits(i, inputInfo->size_y, taby_max, &min, &max);
	// 	printf("%d-%d ", min, max);
	// }

	table = allocArray(tabx_max, taby_max);

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

	Stack * stack = new_stack(max(tabx_max, taby_max));
	push(stack, new_square(0, 0, matx_max, maty_max, tabx_max, taby_max));

	// fill matrix
	Square * square = NULL;
	int x,y;
	int n_processors;
	int notFinished = 1; // false
	int tid;
	int parallel_counter = 0;
	int last_tx, last_ty = 0;

#pragma omp parallel private(square, x, y, notFinished, tid, last_tx, last_ty)
{	
	tid = omp_get_thread_num();
	notFinished = 1;
	while(notFinished) {
		square = NULL;
		last_tx = 0;
		last_ty = 0;
		while(square == NULL && notFinished) {
			#pragma omp critical
			{					
				//square = squareWith2(last_tx, last_ty, table, tabx_max, taby_max, matx_max, maty_max);
				//last_tx = 0;
				//last_ty = 0;
				if(!empty(stack)) {
					square = pop(stack);


					//printf("%d -> [%d][%d]\n", tid, square->tabX, square->tabY );
					table[square->tabX][square->tabY]++;
					parallel_counter++;
				}
			}
			if(square == NULL)
				notFinished = notFinishedTab(table, tabx_max, taby_max);
		}
		
		if(!notFinished)
			break;

		last_tx = square->tabX;
		last_ty = square->tabY;
		for(x = square->x_min; x < square->x_max; x++) {
			for(y = square->y_min; y < square->y_max; y++) {
				calc(x,y, matrix, inputInfo->X, inputInfo->Y);
			}
		}

		#pragma omp critical
		{
			//printf("%d - f\n", tid);
			parallel_counter--;
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
			//printf("%d\n", tid );
			//print(table, tabx_max, taby_max);
			//printf("%d - %d\n", tid, parallel_counter);
			//print(table, tabx_max, taby_max);
		}

		free(square);
		notFinished = notFinishedTab(table, tabx_max, taby_max);

		
		//print(table, tabx_max, taby_max);
		// print(matrix, matx_max, maty_max);
		//printf("\n");
	}
}
	// calc lcs
	
	//print(matrix, matx_max, maty_max);
	lcs_result = lcs(matrix, inputInfo->X, inputInfo->Y, matx_max, maty_max);
	//printf("%d  %d\n", matx_max, maty_max);
	printf("%d\n", matrix[matx_max-1][maty_max-1]);
	printf("%s\n", lcs_result);
	//print(matrix, matx_max, maty_max);
	return 0;
}
