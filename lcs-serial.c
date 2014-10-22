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

void imprimir(int x, int y);
  
struct s_sequencia{
 char tipo; 
};

int max(int a, int b);
int C[11][16];
int k1=0;

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

int LCS_SIZE(char *X, char *Y, int size_x, int size_y, char * lcs)
{
   int i,j,k=0,p=0, l;

   // fill matrix;
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

	imprimir(size_x, size_y);

	// calc lcs
    k = size_x; 
	p = size_y;
	l = 0;
    while (k>0 && p>0){
		if (X[k-1] == Y[p-1]){
        	lcs[l] = X[k-1];  //
		  	k--; p--; l++;		 		                
		} else if(C[k-1][p] >= C[k][p-1]) 
        	k--;
		else 
			p--;
   }
   lcs[l] = '\0';
   reverse(lcs);
	
   return C[size_x-1][size_y-1];
}
 

int max(int a, int b){
    return (a > b)? a : b;
}

void imprimir(int m, int n){
  int i,j,k=0,p=0, pos;

  for (j=0; j<=n; j++){
    for (i=0; i<=m; i++){
	  printf("%d ", C[i][j]);
    }
    printf("\n");
  }
} 
  
  
  
int main(int argc, char *argv[]){ 

   FILE *file;
   int m,n, i;	
   char *sX, *sY;
   char * sZ;
   
   
   
  
   if((file = fopen(argv[1],"r")) != NULL){        	
    
	fscanf(file,"%d %d",&m,&n);
	printf("%d, %d\n", m,n); 
	sX = (char*)malloc(m*sizeof(char*));
	sY = (char*)malloc(n*sizeof(char*)); 
    sZ = (char*)malloc(max(m,n) * sizeof(char));
    
    while((fscanf(file,"%s%s",sX,sY)) != EOF){
              
	}	
	 printf("%d\n", LCS_SIZE(sX,sY,m,n,sZ)); 
	 imprimir(m,n);	          		              
     fclose(file);     
  }
   printf("%s", sZ);  
  
  getchar();
  return 0;
}
