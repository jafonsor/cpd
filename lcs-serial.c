
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

void imprimir(int size_line, int size_column, int **v);  

int max(int a, int b);

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


int **allocate_matrix_lcs(int size_line, int size_column){ 
 
 int **array = (int**)malloc(sizeof(int*)*size_line);
	int x;
	for(x = 0; x < size_line; x++) {
		array[x] = (int*)malloc(sizeof(int)*size_column);		
	}
	return array; 
}



int lcs_size(char *X, char *Y, int size_x, int size_y,char * lcs, int **C)
{
   int i,j;
      
   // fill matrix;  
    for (i=0; i<size_x; i++){
        for (j=0; j<size_y; j++){
	        if (i == 0 || j == 0)
	  	        C[i][j] = 0;    
	        else if ((i>0 && j>0)&&(X[i-1] == Y[j-1]))
	  	        C[i][j] = C[i-1][j-1] + cost(j);
	        else if ((i>0 && j>0)&&(C[i-1][j] != C[i][j-1]))
	  	        C[i][j] = max(C[i-1][j], C[i][j-1]);	  
	        else
	    	    C[i][j] = C[i][j-1]; 
	    }    	 
    }    
   return C[size_x-1][size_y-1];
}
 

int max(int a, int b){
    return (a > b)? a : b;
}


void lcs_max(char *X, char *Y, int size_x, int size_y, char * lcs, int **C){
	
	int k,p,l;
	
	// calc lcs
    k = size_x-1; 
	p = size_y-1;	
	l = 0;
    while (k>0 && p>0){
		if (X[k-1] == Y[p-1]){
        	lcs[l] = X[k-1]; 
		  	k--; p--; l++;		 		                
		} else if(C[k-1][p] >= C[k][p-1]) 
        	k--;
		else 
			p--;
   }
   lcs[l] = '\0';
   
   //call fuction
   reverse(lcs);
}

void imprimir(int size_line, int size_column, int **v){
  
  int i=0,j=0;
  printf("\n\n");
  for (i=0; i<size_line; i++){
    for (j=0; j<size_column; j++){
	  printf("%d ", v[i][j]);
    }
    printf("\n");
  } 
} 


int main(int argc, char *argv[]){ 

   FILE *file = NULL;
   int size_line=0, size_column=0;	
   char *s_X, *s_Y;
   char *s_Max;
   int ** matrix_lcs;
    
   if((file = fopen(argv[1],"r")) != NULL){        	
    
	fscanf(file,"%d %d",&size_line,&size_column);	
	s_X = (char*)malloc(size_line*sizeof(char));	
	s_Y = (char*)malloc(size_column*sizeof(char));	
    s_Max = (char*)malloc(max(size_line,size_column) * sizeof(char));    
    matrix_lcs = allocate_matrix_lcs(size_line,size_column);
         
	while((fscanf(file,"%s%s",s_X,s_Y)) != EOF){
              
	}	
		
	//size sequence
    printf("%d\n", lcs_size(s_X,s_Y,size_line,size_column,s_Max,matrix_lcs)); 
	 
	lcs_max(s_X,s_Y,size_line,size_column,s_Max,matrix_lcs);
	 
    //sequence
    printf("%s\n",s_Max); 
          		              
         
  } 
  return 0;
}
