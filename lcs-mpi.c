#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define BLOCK_LOW(id,p,n) ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)
#define BLOCK_OWNER(index,p,n) (((p)*((index)+1)-1)/(n))

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
	} else {
		printf("cannot open file: %s", fileName);
		exit(0);
	}
	return inputInfo;
}


int main(int argc, char *argv[]){

  int n_procs, pid, i; 
  char * lcs_result = NULL;
  InputInfo * inputInfo = NULL;
  char * x_part;
  
  // read input
	inputInfo = readInput(argv[1]);
	if(inputInfo == NULL) {
		printf("input info is NULL\n");
		return -1;
	}

	printf("%d\n", inputInfo->size_x);
  MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);	
	 
	//MPI_Bcast(inputInfo->Y, inputInfo->size_y, MPI_CHAR, 0, MPI_COMM_WORLD);
	x_part = &inputInfo->X[BLOCK_LOW(pid, n_procs, inputInfo->size_x)];
	
	char * x_part_out = (char*)malloc(sizeof(char)*(BLOCK_SIZE(pid, n_procs, inputInfo->size_x)+1));
	memcpy(x_part_out, x_part, BLOCK_SIZE(pid, n_procs, inputInfo->size_x));
	x_part_out[BLOCK_SIZE(pid, n_procs, inputInfo->size_x)] = '\0';
	
	printf("%d - %d, %d, %d\n", pid, n_procs, inputInfo->size_x); 
	printf("%d - \ty:%s\n\tx:%d-%d-%d-%s\n",pid,inputInfo->Y, BLOCK_LOW(pid, n_procs, inputInfo->size_x),BLOCK_HIGH(pid, n_procs, inputInfo->size_x),BLOCK_SIZE(pid, n_procs, inputInfo->size_x), x_part_out);	

  MPI_Finalize();


  return 0;

}
