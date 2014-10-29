#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ** allocArray(int xdim, int ydim) {
	int **array = (int**)malloc(sizeof(int*)*xdim);
	int x;
	for(x = 0; x < xdim; x++) {
		array[x] = (int*)malloc(sizeof(int)*ydim);
		memset(array[x], (unsigned char)0, sizeof(int) * ydim);
	}
	return array;
}

void cell_limits(int tab_coord, int max_mat_coord, int n_processors, int  *coord_min, int *coord_max) {
	int remainder = max_mat_coord % n_processors;
	int  n_mat_coords_per_cell = max_mat_coord / n_processors;
	*coord_min = n_mat_coords_per_cell * tab_coord;
	*coord_max = (*coord_min) + n_mat_coords_per_cell;
	if(tab_coord == n_processors - 1) {
		*coord_max += remainder;
	}
}

typedef struct square {
	int tabX;
	int tabY;
	int x_min;
	int x_max;
	int y_min;
	int y_max;
} Square;

Square * new_square(int x, int y, int matx_max, int maty_max, int n_processors) {
	Square * new_square = (Square*)malloc(sizeof(Square));
	new_square->tabX = x;
	new_square->tabY = y;
	cell_limits(x,matx_max, n_processors, &new_square->x_min, &new_square->x_max);
	cell_limits(y,maty_max, n_processors, &new_square->y_min, &new_square->y_max);
	return new_square;
}

Square * squareWith2(int **table, int tabx_max, int taby_max, int matx_max, int maty_max  ,int n_processors ) {
	int x,y;
	for(x = 0; x < tabx_max; x++) {
		for(y = 0; y < taby_max; y++) {
			if(table[x][y] == 2) {
				return new_square(x, y, matx_max, maty_max, n_processors);
			}
		}
	}
	return NULL;
}

#define X_MAX 13
#define Y_MAX 9

int notFinishedTab(int **table, int tabx_max, int taby_max) {
	int i,j;
	int number_of_3s = 0;
	for(i = 0; i < tabx_max; i++) {
		for(j = 0; j < taby_max; j++) {
			if(table[i][j] == 3)
				number_of_3s++;
		}
	}
	return (number_of_3s != tabx_max * taby_max)? 1 : 0;
}

void calc(int x, int y, int **matrix) {
	static int counter = 0;
	matrix[x][y] = counter;
	counter++;
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

int main() {
	int matx_max = X_MAX;
	int maty_max = Y_MAX;
	int n_processors = 4;

	int **matrix = allocArray(matx_max, maty_max);

	print(matrix, matx_max, maty_max);

	int tabx_max = n_processors;
	int taby_max = n_processors;
	printf("%d, %d\n", tabx_max, taby_max);

	int **table = allocArray(tabx_max, taby_max);

	int i,j;
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
	print(table, tabx_max, taby_max);

	Square * square = NULL;
	int x,y;
	while(notFinishedTab(table, tabx_max, taby_max)) {
		square = NULL;
		while(square == NULL) {
			//lock A {
				square = squareWith2(table, tabx_max, taby_max, matx_max, maty_max, n_processors);
			//}
		}
		//lock A {
			table[square->tabX][square->tabY]++;
		//}
		for(x = square->x_min; x < square->x_max; x++) {
			for(y = square->y_min; y < square->y_max; y++) {
				calc(x,y, matrix);
			}
		}
		//lock A {
			if(square->tabX + 1 < tabx_max) {
				table[square->tabX + 1][square->tabY]++;
			}
			if(square->tabY +1 < taby_max) {
				table[square->tabX][square->tabY + 1]++;
			}
		//}
		free(square);
		print(table, tabx_max, taby_max);
		print(matrix, matx_max, maty_max);
		printf("\n");
		
	}
	return 0;
}
