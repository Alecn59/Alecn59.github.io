/*********************************************************************************
* Alec Narkizian
* 2022 Winter CSE101 PA#4
* Matrix.c
* Calculator in C, capable of performing fast matrix operations that exploits the (expected) 
* sparseness of its matrix operand
*********************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include"Matrix.h"
#include"List.h"


//builds matrix obj struc
typedef struct MatrixObj{
	List* rows;
	int size;
	int nonzero;
} MatrixObj;


typedef struct EntryObj{
	int col;
	double val;
} EntryObj;

typedef EntryObj* Entry;


//populates a new matrix
Matrix newMatrix(int n){
	Matrix M = malloc(sizeof(MatrixObj));
	assert(M!=NULL);
	M->rows = malloc(sizeof(List) * n);
	for(int x=0; x<n; x++){
		M->rows[x] = newList();
	}
	M->size = n;
	M->nonzero = 0;
	return(M);
}


//frees matrix
void freeMatrix(Matrix* pM){
	if(pM!=NULL && *pM!=NULL){
		makeZero(*pM);
		for(int x=0; x<size(*pM); x++){
			freeList(&((*pM)->rows[x]));
		}
		free((*pM)->rows);
		free(*pM);
		*pM = NULL;
	}
}


Entry newEntry(int c, double v){
	Entry E = malloc(sizeof(EntryObj));
	E->col = c;
	E->val = v;
	return(E);
}

//returns size of matrix
int size(Matrix M){
	if (M==NULL){
		printf("Matrix Error\n");
		exit(1);
	}
	return M->size;
}

//Return the number of non-zero elements in M.
int NNZ(Matrix M){
	if (M==NULL){
		printf("Matrix Error\n");
		exit(1);
	}
	int c =  M->nonzero;
	return c;
	
}

void freeEntry(Entry* pE){
   if (pE!=NULL && *pE!=NULL){
      free(*pE);
      *pE = NULL;
   }
}


// equals()
// Return true (1) if matrices A and B are equal, false (0) otherwise.
int equals(Matrix A, Matrix B){
	if(A==NULL || B==NULL){
   		printf("Error\n");
   		exit(1);
	}
	if(A->size != B->size){
		return(0);
	}
	return(1);
}


// Manipulation procedures
// makeZero()
// Re-sets M to the zero Matrix state.
void makeZero(Matrix M){
	M->nonzero=0;
	for(int x=0; x<size(M); x++){
		M->rows[x] = newList();
	}
}

// changeEntry()
// Changes the ith row, jth column of M to the value x.
// Pre: 1<=i<=size(M), 1<=j<=size(M)
void changeEntry(Matrix M, int i, int j, double x){
	if (1<=i && i <= size(M) && 1<=j && j <= size(M)){
		List newList = M-> rows[i-1];
		moveFront(newList);
		Entry E = newEntry(j,x);

		int y = 0;
		if(x!=0){
			if(length(newList) == 0){
				append(newList,E);
				M->nonzero++;
			}else{
				moveFront(newList);
				while(index(newList) != -1){
					y++;
            		Entry S = get(newList);
					double d = S->col;
    				if(d > j){
    					insertBefore(newList,E);
    					M->nonzero++;
    					break;
    				}
    				else if (d == j){
    					set(newList,E);
    					break;
    				}else if (length(newList) == y){
    					if(d <j){
    						append(newList,E);
    						M->nonzero++;
    						break;
    					}
    				}
					moveNext(newList);
				}
			}
		}else if (x==0)	{
			moveFront(newList);
			while(index(newList) != -1){
				y++;
            	Entry S = get(newList);
				double d =S->col;
   	    		if (d == j){
   	    			delete(newList);
   	    			M->nonzero--;
   	    			break;
   	    		}
   	    		moveNext(newList)	;
			}
		}
	}else{
		printf("changeEntry Error:\n");
	}

}


// Matrix Arithmetic operations
// copy()
// Returns a reference to a new Matrix object having the same entries as A.
Matrix copy(Matrix A){
    if(A == NULL){
        fprintf(stderr, "ERROR");
        exit(1);
    } 
    Matrix new = newMatrix(size(A));
    new->nonzero = A->nonzero;
    for(int i = 0; i < size(A) ; i++) {
        new->rows[i] = copyList(A->rows[i]);
    }
    return new;
}

// transpose()
// Returns a reference to a new Matrix object representing the transpose
// of A.
Matrix transpose(Matrix A){
	Matrix new = newMatrix(size(A));
	for (int x = 0; x<size(A); x++){
		List a = A->rows[x];
		for(moveFront(a);  index(a)>-1; moveNext(a)){
			Entry S = get(a);
			changeEntry(new,S->col,x+1,(double)S->val);
		}
	}
	return new;
}



//helper function for sum
List addList1( List L , List N){
	List newL=newList();
	moveFront(L);
    moveFront(N);
    int p =0;
	while(p!=8){
		if(index(L)==-1 || index(N)==-1){
				break;
		}
	 	Entry A = get(L);
     	Entry B = get(N);
		if(A->col==B->col){
				double x = (double)B->val + (double)A->val;
				if ( x != 0){
					append(newL,newEntry(A->col, (double)x));
					moveNext(N);
					moveNext(L);
				}
				if(x == 0){ 
					moveNext(N);
					moveNext(L);
				}
		}else if(A->col>B->col){
			append(newL,newEntry(B->col, (double)B->val));
			moveNext(N);		
		}
    	else if (A->col<B->col){
			append(newL,newEntry(A->col, (double)A->val));
			moveNext(L);
		}
	}
	//checks to see if cursor is still on either of the matrixs
	while(index(L)>-1 ){
		Entry B = get(L);
		append(newL,newEntry(B->col, (double)B->val));
		moveNext(L);
	}while(index(N)>-1 ){
		Entry B = get(N);
		append(newL,newEntry(B->col, (double)B->val));
		moveNext(N);
	}
	return newL;
}



// scalarMult()
// Returns a reference to a new Matrix object representing xA.
Matrix scalarMult(double x, Matrix A){
	if (A == NULL){
        printf("Matrix Error: NULL\n");
        exit(1);
    }
	Matrix N = newMatrix(size(A));
	for(int i=0; i<size(A); i++){
		List a = A->rows[i];
		int v = 0;
		for(moveFront(a); v<length(a); moveNext(a)){
			Entry e = (Entry)get(a);
			changeEntry(N, i+1, e->col, (double)e->val*x);
			v++;
		}
	}
	return(N);
}





// sum()
// Returns a reference to a new Matrix object representing A+B.
// pre: size(A)==size(B)
Matrix sum(Matrix A, Matrix B){
	if( A==NULL){
   		printf("Matrix Error:\n");
   		exit(1);
	}if(B==NULL){
   		printf("Matrix Error:\n");
   		exit(1);
	}if(size(A) != size(B)){
        fprintf(stderr, "ERROR: cannot call sum() on matricies of different sizes");
        exit(EXIT_FAILURE);
    }
	List newL;
	int d = 0;
	Matrix M = newMatrix(size(A));
	if (size(A)==size(B)){
		while(d<size(A)){
			
			List a = A->rows[d];
			List b = B->rows[d];
			if(A == B){
				M = scalarMult(2.0, A);
				return M;
   			}
   			newL =addList1(a,b);
		    M->rows[d] = newL;
		    M->nonzero += length(M->rows[d]);
		    d++;
		}		
	}
	return M;
}



// diff()
// Returns a reference to a new Matrix object representing A-B.
// pre: size(A)==size(B)
Matrix diff(Matrix A, Matrix B){
	if( A==NULL){
   		printf("Matrix Error:\n");
   		exit(1);
	}if(B==NULL){
   		printf("Matrix Error:\n");
   		exit(1);
	}if(size(A) != size(B)){
        fprintf(stderr, "ERROR:");
        exit(1);
    }
	Matrix M = newMatrix(size(A));
	if (A==B){
    	makeZero(M);
    	return M;
    }
    //negates sexonf matrix then adds them
	if (size(A)==size(B)){
		B = scalarMult(-1.0,B);
		B = sum(B,A);  
	}
	return B;
}

// dot()
// pre: size(A)==size(B)
double dot(List L, List N){
	moveFront(L);
    moveFront(N);
    double value = 0;
    double value1 = 0;
    while(index(L)>-1 && index(N)>-1){
       	Entry A = get(L);
     	Entry B = get(N);
        if(A->col==B->col){
			value1 = A->val * B->val;
			value = value1+value;	
			moveNext(N);
			moveNext(L);
		}else if(A->col>B->col){
			moveNext(N);		
		}
    	else if (A->col<B->col){
			moveNext(L);
		}
	}
   return value;
}

// product()
// Returns a reference to a new Matrix object representing AB
// pre: size(A)==size(B)
Matrix product(Matrix A, Matrix B){
   double value;
	if( A==NULL){
   		printf("Matrix Error:\n");
   		exit(1);
	}
	if(B==NULL){
   		printf("Matrix Error:\n");
   		exit(1);
	}
	if(size(A) != size(B)){
        fprintf(stderr, "ERROR");
        exit(1);
    }
    Matrix C = newMatrix(A->size);
    C = transpose(B);
    Matrix M = newMatrix(A->size);
    int d = 0;
    while(d<size(A)){
    	List a = A->rows[d];
        for(int j=0; j<M->size; j++){
     		List b = C->rows[j];
            value = dot(a,b);
            if(value != 0){
            	changeEntry(M, d + 1, j + 1, value);
            }
         }
         	d++;
   }
   return(M);
}


// printMatrix()
// Prints a string representation of Matrix M to filestream out. Zero rows
// are not printed. Each non-zero row is represented as one line consisting
// of the row number, followed by a colon, a space, then a space separated
// list of pairs "(col, val)" giving the column numbers and non-zero values
// in that row. The double val will be rounded to 1 decimal point.
void printMatrix(FILE* out, Matrix M){
	if( M==NULL){
   		printf("Matrix Error:\n");
   		exit(1);
	}
    int d = 0;
    while(d<size(M)){
    	List a = M->rows[d];
        d++;
        if (length(a) > 0) {
            fprintf(out,"%d: ", d);
            moveFront(a);
            while(index(a) != -1){
                Entry E = get(a);            
                fprintf(out, "(%d, %.1f) ", E->col, E->val);
                moveNext(a);
            }
            fprintf(out, "\n");
        }
        
     }
 }
