/*
	Autor: Pedro Mbote
	Nº: 79803
	Disciplina: CPD
	ex1.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h> 

void imprimir(int size_line, int size_column, int size_thread, int **v);  
int max(int a, int b);

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


int lcs_size(char *X, char *Y, int size_x, int size_y, char * lcs)
{
   int i,j,k=0,p=0,l;
   int C[size_x+1][size_y+1];
   
   // fill matrix;
   
    //#pragma omp parallel for private(i,j) schedule(dynamic,4)
    for (i=0; i<=size_x; i++){
        for (j=0; j<=size_y; j++){
	        if (i == 0 || j == 0)
	  	        C[i][j] = 0;    
	        else if ((i>0 && j>0)&&(X[i-1] == Y[j-1]))
	  	        C[i][j] = C[i-1][j-1] + 1;    
	        else if ((i>0 && j>0)&&(C[i-1][j] != C[i][j-1]))
	  	        C[i][j] = max(C[i-1][j], C[i][j-1]);	  
	        else
	    	    C[i][j] = C[i][j-1]; 
	    }
	    	 
    }  

	// calc lcs
    k = size_x; 
	p = size_y;
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
	
   return C[size_x-1][size_y-1];
}
 

int max(int a, int b){
    return (a > b)? a : b;
}

void imprimir(int size_line, int size_column, int size_thread, int **v){
  
  int i=0,j=0;
  int m = (int) (size_line/size_thread);
  int n = (int) (size_column/size_thread);
  printf("\n\n");
  for (i=0; i<m; i++){
    for (j=0; j<n; j++){
	  printf("%d ", v[i][j]);
    }
    printf("\n");
  } 
} 

  
int **allocate_matrix_lcs_aux (int size_line, int size_column, int size_thread){
 
  int **v  = NULL;  
  int   i; 
  int m = (int) (size_line/size_thread);
  int n = (int) (size_column/size_thread); 
   
  if (size_line < 1 || size_column < 1) { 
    printf ("** Erro: Parametro invalido **\n");
    return (NULL);
  }
  
  //allocate line
  v = (int **) malloc (m*sizeof(int*));
  
  if (v == NULL) {
    printf ("** Erro: Memoria Insuficiente **");
    return (NULL);
  }
  
  //allocate column
  for ( i = 0; i <= m; i++ ) {
    v[i] = (int*) malloc (n*sizeof(int));
    if (v[i] == NULL) {
      printf ("** Erro: Memoria Insuficiente **");
      return (NULL);
    }
  }
  return v; 
}   

  
int **release_matrix_lcs_aux (int size_line, int size_column, int size_thread, int ** v){

  int i; 
  int m = (int) (size_line / size_thread);
  
  if (size_line < 1 || size_column < 1) { 
    printf ("** Erro: Parametro invalido **\n");
    return (NULL);
  }
  
  if (v == NULL) {
    printf ("** Erro: Memoria Insuficiente **");
    return (v);
  }
  
  //realease line
  for (i=0; i<m; i++) 
    free(v[i]); 
   
  //realease column 
  free(v);
  
  return (NULL);
 }  
  
void  initialize_matrix_lcs_aux(int size_line, int size_column, int size_thread, int **v){
	
   int p,q;
   int m = (int) (size_line/size_thread);
   int n = (int) (size_column/size_thread);
   
   for(p=0; p<m; p++){   	
     for(q=0; q<n; q++){ 
       if(p==0 && q==0)  
         v[p][q]=2;
       else if(p==0 || q==0)
         v[p][q]=1;
       else
         v[p][q]=0;          
     }
   } 
}
 
 
int main(int argc, char *argv[]){ 

   FILE *file;
   int size_line, size_column, i;	
   char *s_X, *s_Y, *s_Max;
   int ** matrix_lcs_aux;
   int size_thread = 2;  // omp_get_num_threads();
   //int size_line_aux, size_column_aux;
    
   if((file = fopen(argv[1],"r")) != NULL){        	
    
	fscanf(file,"%d %d",&size_line,&size_column);	
	s_X = (char*)malloc(size_line*sizeof(char*));
	s_Y = (char*)malloc(size_column*sizeof(char*)); 
	
    s_Max = (char*)malloc(max(size_line,size_column) * sizeof(char));
    matrix_lcs_aux = allocate_matrix_lcs_aux(size_line,size_column,size_thread);
         
	while((fscanf(file,"%s%s",s_X,s_Y)) != EOF){
              
	}	
	 //size sequence
	 printf("%d\n", lcs_size(s_X,s_Y,size_line,size_column,s_Max)); 	         		              
     fclose(file);     
  }
  
  //sequence
  printf("\n %s",s_Max); 
 
  //call matrix_aux
  initialize_matrix_lcs_aux(size_line,size_column,size_thread,matrix_lcs_aux);
  
  // imprimir matrix_aux
  imprimir(size_line,size_column,size_thread,matrix_lcs_aux);
  
  //realese variable
  free(s_X);
  free(s_Y);
  free(s_Max);
  //matrix_lcs_aux = release_matrix_lcs_aux(size_line,size_column,size_thread,matrix_lcs_aux);
  
  getchar();
  return 0;
}
