#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#define BLOCK_LOW(id,p,n) ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)
#define BLOCK_OWNER(index,p,n) (((p)*((index)+1)-1)/(n))

#define SWAP(T,a,b) {T t = a; a = b; b = t;}

#define STRINGIFY(a) #a
#define DIAGONAL_ITERATION(width,height,fn) \
  int diag, i, max_diag, min_diag; \
  int n_diags = width + height - 1; \
  for(diag = 1; diag <= n_diags; diag++) { \
    max_diag = min(width,diag); \
    min_diag = max(1, diag - height + 1); \
    _Pragma(STRINGIFY(omp parallel for private(i))) \
    for(i = min_diag; i <= max_diag; i++) { \
      fn(i,-i + diag + 1); \
    } \
  }


enum TAG {
  FATHER_TO_CHILDREN_TAG, 
  LCS_SIZE_TAG,
  SUB_LCS_TAG,
  CURRENT_Y_TAG,
  X_PART_TAG
};

#define max(a,b) (a > b)? a : b
#define min(a,b) (a < b)? a : b

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
    inputInfo->size_x;
    inputInfo->size_y;
    inputInfo->X = (char*)malloc((inputInfo->size_x+1)*sizeof(char));
    inputInfo->Y = (char*)malloc((inputInfo->size_y+1)*sizeof(char)); 
  
    while((fscanf(file,"%s%s",inputInfo->X,inputInfo->Y)) != EOF) {

    }
    fclose(file);

    // keep the longest sequence on X to reduce comunication
    if(inputInfo->size_x < inputInfo->size_y) {
      SWAP(int, inputInfo->size_x, inputInfo->size_y);
      SWAP(char*, inputInfo->X, inputInfo->Y);
    }

  } else {
    printf("cannot open file: %s", fileName);
    exit(0);
  }
  return inputInfo;
}

// void print_x_part(char * x_part, int pid, int n_procs, int size_x) {
//   char * x_part_out = (char*)malloc(sizeof(char)*(BLOCK_SIZE(pid, n_procs, inputInfo->size_x)+1));
//   memcpy(x_part_out, x_part, BLOCK_SIZE(pid, n_procs, inputInfo->size_x));
//   x_part_out[BLOCK_SIZE(pid, n_procs, inputInfo->size_x)] = '\0';
// }

typedef short unsigned CellVal;

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

short cost(int x) {
  int i, n_iter = 20;
  double dcost = 0;
  for(i = 0; i < n_iter; i++)
  dcost += pow(sin((double) x),2) + pow(cos((double) x),2);
  return (short) (dcost / n_iter + 0.1);
}

//short cost(int x) { return 1; }

// calculate the value for the matrix cell at (x,y)
void calc(int x, int y, CellVal **matrix, char * X, char * Y) {
  if (x == 0 || y == 0)
    //matrix[x][y] = 0;
    printf("should not write on x == 0 nor on y == 0");
  else if (X[x-1] == Y[y-1])
    matrix[x][y] = matrix[x-1][y-1] + cost(y);
  else
    matrix[x][y] = max(matrix[x-1][y], matrix[x][y-1]);
}

void print(CellVal ** mat, int y_low, int size_x, int y_high){
  int i,j;
  if(y_low == 1) {
    y_low = 0;
  }
  for (j=0; j<=size_x; j++){
    for (i=y_low; i<=y_high; i++){
      printf("%d ", mat[j][i]);
    }
    printf("\n");
  }
  fflush(stdout);
} 


// sequence calc funcs
typedef struct lcs_sub_result
{
  short unsigned last_y;
  char *sub_lcs;
  short  unsigned sub_lcs_size;
} LcsSubResult;


// calculates the lcs based on the previeously filled matrix
LcsSubResult *  sub_lcs(CellVal ** mat_part, char * X, char *Y, int x_size, int current_y) {
  char * lcs = (char*)malloc((x_size+1) * sizeof(char));
  lcs[x_size] = '\0';
  short unsigned x = x_size;
  short unsigned y = current_y;
  short unsigned l = x_size;
  //printf("y: %d\n",current_y);
  while(x > 0 && y > 0) {
    if(X[x-1] == Y[y-1]) {
      lcs[l-1] = X[x-1];
      x--; y--; l--;
    } else if(mat_part[x-1][y] > mat_part[x][y-1]) {
      x--;
    } else {
      y--;
    }
  }
  LcsSubResult * result = (LcsSubResult*)malloc(sizeof(LcsSubResult));
  //printf("%d\n",y);
  fflush(stdout);
  result->last_y = y;
  result->sub_lcs = &lcs[l];
  result->sub_lcs_size = x_size - l;
  return result;
}


int main(int argc, char *argv[]) {
  double secs; // time of execution
  int n_procs, rank, i; 
  char * lcs_result = NULL;
  InputInfo * inputInfo = NULL;
  char * x_part = NULL;
  CellVal **mat_part = NULL;
  LcsSubResult * subResult;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Barrier (MPI_COMM_WORLD);
  secs = - MPI_Wtime();

  if(rank == 0) {
    // read input
    inputInfo = readInput(argv[1]);
    if(inputInfo == NULL) {
      printf("input info is NULL\n");
      return -1;
    }
  } else {
    inputInfo = (InputInfo*)malloc(sizeof(InputInfo));
  }

  MPI_Bcast(&inputInfo->size_x, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&inputInfo->size_y, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // initialize proc own info about its x sequence sub part
  int x_low  = BLOCK_LOW( rank, n_procs, inputInfo->size_x);
  int x_size = BLOCK_SIZE(rank, n_procs, inputInfo->size_x);

  x_part = (char*)malloc(sizeof(char)*x_size+1);
  if(rank != 0) {
    // alloc array on receiving processes of the Y sequence
    inputInfo->Y = (char*)malloc(sizeof(char)*inputInfo->size_y);
  }

  MPI_Status status;
  MPI_Request request;

  // distribute the X parts to the other processes
  int x_part_low, x_part_size;
  if(rank == 0) {
    memcpy(x_part, inputInfo->X, x_size);
    for(i = 1; i < n_procs; i++) {
      x_part_low  = BLOCK_LOW( i, n_procs, inputInfo->size_x);
      x_part_size = BLOCK_SIZE(i, n_procs, inputInfo->size_x);
      MPI_Isend(&inputInfo->X[x_part_low], x_part_size, MPI_CHAR, i, X_PART_TAG, MPI_COMM_WORLD, &request);
    }
  } else {
    MPI_Recv(x_part, x_size, MPI_SHORT, 0, X_PART_TAG, MPI_COMM_WORLD, &status);
  }

  x_part[x_size] = '\0';

  // broadcast the Y sequence
  MPI_Bcast(  inputInfo->Y, inputInfo->size_y, MPI_CHAR, 0, MPI_COMM_WORLD);
  // printf("%d: Y=%s\n", rank, inputInfo->Y);
  // printf("%d: X=%s\n", rank, x_part);
  // fflush(stdout);


  mat_part = allocArray(x_size+1, inputInfo->size_y + 1);



  int y_size = 40;
  int n_cols = inputInfo->size_y / y_size;
  int col_y_low, col_y_high,col_y_size,c;

  for(c = 0; c < n_cols; c++){

    col_y_low  = BLOCK_LOW( c, n_cols, inputInfo->size_y) +1;
    col_y_size = BLOCK_SIZE(c, n_cols, inputInfo->size_y);
    col_y_high = BLOCK_HIGH(c, n_cols, inputInfo->size_y) +1;

    //-- fill matrix part --
    if(rank != 0) {
      MPI_Recv(&mat_part[0][col_y_low],    col_y_size, MPI_SHORT, (rank - 1), FATHER_TO_CHILDREN_TAG, MPI_COMM_WORLD, &status);
    }

    // printf("%d: col=%d, low=%d, high=%d\n", rank, c, col_y_low, col_y_high);
    // fflush(stdout);
    int x, y;

    #define CALC_SAWP(x,y) calc(y,x, mat_part, x_part, inputInfo->Y)
    printf("antes\n");
    fflush(stdout);
    DIAGONAL_ITERATION(col_y_high, x_size, CALC_SAWP);
    printf("antes\n");
    fflush(stdout);

    //printf("id: %d\n", rank);
    //fflush(stdout);
    //print(mat_part, col_y_low, x_size, col_y_high);

    //printf("id: %d - a\n", rank);
    if(rank != n_procs - 1) {
      MPI_Isend(&mat_part[x_size][col_y_low], col_y_size, MPI_SHORT, rank + 1, FATHER_TO_CHILDREN_TAG, MPI_COMM_WORLD, &request);
    }
    //printf("id: %d - b\n", rank);
    //fflush(stdout);
  }

  //-- calc sequence --
  short unsigned lcs_total_size;
  short unsigned last_y, current_y = inputInfo->size_y;
  char * lcs_parts[n_procs-1]; // used by rank = 0 to store the sub parts of the sequence

  if(rank==0){
    // init squence part receiver
    for(i = 1; i < n_procs; i++) {
      lcs_parts[i-1] = (char*)malloc(sizeof(char)*(BLOCK_SIZE(i, n_procs, inputInfo->size_x)+1));
    }
    MPI_Recv(&lcs_total_size, 1, MPI_SHORT, n_procs-1, LCS_SIZE_TAG, MPI_COMM_WORLD, &status);  
    for(i=n_procs-1; i > 0; i--)
      MPI_Recv(lcs_parts[i-1], BLOCK_SIZE(i, n_procs, inputInfo->size_x)+1, MPI_CHAR, i, SUB_LCS_TAG, MPI_COMM_WORLD, &status);      
  }

  if(rank == n_procs-1) {
    MPI_Isend(&mat_part[x_size][inputInfo->size_y], 1, MPI_SHORT, 0, LCS_SIZE_TAG, MPI_COMM_WORLD, &request);
  }

  if(rank != n_procs - 1) {
    MPI_Recv(&current_y, 1, MPI_SHORT, rank+1, CURRENT_Y_TAG, MPI_COMM_WORLD, &status);
  }

  //calcular senquencia
  subResult = sub_lcs(mat_part, x_part, inputInfo->Y, x_size, current_y);

  //printf("rank: %d, last_y: %d, sub_lcs_size: %d, sub_lcs: %s\n", rank, subResult->last_y, subResult->sub_lcs_size, subResult->sub_lcs);
  fflush(stdout);
  if(rank != 0) {
    MPI_Isend(subResult->sub_lcs, subResult->sub_lcs_size + 1, MPI_CHAR, 0, SUB_LCS_TAG, MPI_COMM_WORLD, &request);
    MPI_Isend(&subResult->last_y, 1, MPI_SHORT, rank - 1, CURRENT_Y_TAG, MPI_COMM_WORLD, &request);
  } else {
    printf("%d\n", lcs_total_size);
    printf(subResult->sub_lcs);
    for(i = 1; i < n_procs; i++) {
      printf("%s", lcs_parts[i-1]);
    }
    printf("\n");
    fflush(stdout);
  }

  MPI_Barrier (MPI_COMM_WORLD);
  secs += MPI_Wtime();

  if(rank == 0) {
    printf("time: %f\n", secs);
  }

  MPI_Finalize();

  //printf("id: %d - c\n", rank);
  fflush(stdout);
  return 0;

}