#include <stdio.h>
#include <math.h>
#include "mex.h"

typedef struct{
		int nDimension;/* Number of dimensions*/
		int *nPoints;   /* number of points along each direction */
		}ND_INFO;

/*******************************************/
/*    Creation of integer vector 	   */
/*******************************************/
int *intVector(int n){
	int *vec = (int*)malloc(n*sizeof(int));
	return(vec);
	}

/*********************************************/
/* 	Create a double Vector		     */
/*********************************************/
double *doubleVector(int n){
	double *vec = (double*)malloc(n*sizeof(double));
	return(vec);
	}
/*******************************************/
/*    Creation of integer MATRIX 	   */
/*******************************************/
int **intMatrix(int n,int m){
	int i;
	int **mat = (int**) malloc(n*sizeof(int*));
	for(i=0;i<n;i++)
		mat[i] = (int*) malloc(m*sizeof(int));
	return(mat);
	}

/*********************************************/
/* 	Create a double MATRIX		     */
/*********************************************/
double **doubleMatrix(int n,int m){
	int i;
	double **mat = (double**) malloc(n*sizeof(double*));
	for(i=0;i<n;i++)
		mat[i] = (double*) malloc(m*sizeof(double));
	return(mat);
	}


/*********************************************/
/*  	Free integer matrix			  */
/*********************************************/
void freeIntMat(int **mat,int n,int m){
	/* the column size is not used but is required only
		for debugging purpose
	*/
	int i;
	for(i=0;i<n;i++)
		free(mat[i]);
	free(mat);
	}

/*********************************************/
/*   	Free double matrix			  */
/*********************************************/
void freeDoubleMat(double **mat,int n,int m){
	/* the column size is not used but is required only
		for debugging purpose
	*/
	int i;
	for(i=0;i<n;i++)
		free(mat[i]);
	free(mat);
	}

/*********************************************/
/*    Print out error and exit program       */
/*********************************************/
void ErrMsg(char *m){
	mexErrMsgTxt(m);
	}

/************************************************/
/*    Get the indices of the hyper cube in the  */
/*    grid in which the point lies              */
/************************************************/

int **getHyperCube(double **X,double *V,ND_INFO ndinfo){
	int **indexMatrix = intMatrix(ndinfo.nDimension,2);
                     /*indexMatrix[i][0] => Lower, ...[1]=>Higher*/
	int i,j;
	int indexMax;
	double x,xmax,xmin;

	for(i=0;i<ndinfo.nDimension;i++){
		indexMax = ndinfo.nPoints[i]; /* Get the total # of points in this dimension */
		xmax = X[i][indexMax-1];	 /* Get the upper bound along this axis */
		xmin = X[i][0];			/* Get the lower bound along this axis */

		/**************************************************************************** 
			It has been assumed that the gridpoints are monotonically increasing
			the zero index is the minimum and the max-1 is the maximum.
		*****************************************************************************/

		/****************************************************************************
        		Get the ith component in the vector V, the point at which we want to 
        		interpolate
		****************************************************************************/
		x = V[i];
        /* THIS LAST PART OF THIS FUNCTION HAS BEEN MODIFIED IN ORDER TO CAN MANAGE HIGH AoA AND SIDESLIPS*/
        
		/* Check to see if this point is within the bound */
        
		if(x<xmin ){
            
            printf("\n Point lies out data grid (in getHyperCube)");
			ErrMsg("Point lies out data grid (in getHyperCube)");

            x = xmin;
        }
        else if( x>xmax){
            printf("\n Point lies out data grid (in getHyperCube)");
			ErrMsg("Point lies out data grid (in getHyperCube)");
            x = xmax;
        }
        else{}
        
        /* Now we assign the index*/
        for(j=0;j<indexMax-1;j++){
            if(x==X[i][j]){
                indexMatrix[i][0] = indexMatrix[i][1] = j;
                break;
            }
            if(x==X[i][j+1]){
                indexMatrix[i][0] = indexMatrix[i][1] = j+1;
                break;
            }
            if(x>X[i][j] && x<X[i][j+1] ){
                indexMatrix[i][0] = j;
                indexMatrix[i][1] = j+1;
                break;
            }
        }/*End of for(j=...) */
        

        
		}/* End of for(i= ...) */
		return(indexMatrix);
	}/* End of function */

/*********************************************************************************
 indexVector contains the co-ordinate of a point in the ndimensional grid
 the indices along each axis are assumed to begin from zero
 *********************************************************************************/
int getLinIndex(int *indexVector,ND_INFO ndinfo){
	int linIndex=0;
	int i,j,P;
	for(i=0;i<ndinfo.nDimension;i++){
		P=1;
		for(j=0;j<i;j++)
			P=P*ndinfo.nPoints[j];
		linIndex = linIndex + P*indexVector[i];
		}
	return(linIndex);
	}

double linearInterpolate(double *T,double *V,double **X,ND_INFO ndinfo){
 int n,m,i,j,k,nVertices;
 double *oldT,*newT;
 int mask,val;
 int *indexVector = intVector(ndinfo.nDimension);
 int index1,index2;
 double f1,f2,lambda,result;
 int dimNum;

 n=ndinfo.nDimension;
 nVertices = 1<<n;

 oldT = doubleVector(nVertices);
 for(i=0;i<nVertices;i++)
	oldT[i] = T[i];

 dimNum = 0;
 while(n>0){
	m=n-1;
	nVertices = (1<<m);
	newT = doubleVector(nVertices);
	for(i=0;i<nVertices;i++){
		for(j=0;j<m;j++){
			mask = (1<<j);
			indexVector[j] =  (mask & i) >> j;
			}/*End of for j*/
		index1 = 0;
		index2 = 0;
		for(j=0;j<m;j++){
			index1 = index1 + (1<<(j+1))*indexVector[j];
			index2 = index2 + (1<<j)*indexVector[j];
			}/*End of for j*/
		f1 = oldT[index1];
		f2 = oldT[index1+1];
		if(X[dimNum][0]!=X[dimNum][1]){
			lambda = (V[dimNum]-X[dimNum][0])/(X[dimNum][1]-X[dimNum][0]);
			newT[index2] = lambda*f2 + (1-lambda)*f1;
			}
		else
			newT[index2] = f1;
		}/*End of for i*/
	free(oldT);
	oldT = doubleVector(nVertices);
	for(i=0;i<nVertices;i++)
		oldT[i] = newT[i];
	free(newT);
	n=m;
	dimNum++;
	}/* End of while*/
 result = oldT[0];
 free(oldT);
 free(indexVector);
 return(result);
}/* End of function */





double interpn(double **X,double *Y,double *x,ND_INFO ndinfo){

	double **xPoint,*T;
	double result;

	int i,j,high,low,counter;
	int mask,val,P,index,nVertices,nDimension;
	int **indexMatrix,*indexVector;

	indexVector = intVector(ndinfo.nDimension);
	xPoint = doubleMatrix(ndinfo.nDimension,2);

	/* Get the indices of the hypercube containing the point in argument */
	indexMatrix = getHyperCube(X,x,ndinfo);

	nVertices = (1<<ndinfo.nDimension);
	T = doubleVector(nVertices);

	nDimension = ndinfo.nDimension;
	

	/* Get the co-ordinates of the hyper cube */
	for(i=0;i<nDimension;i++){
			low  = indexMatrix[i][0];
			high = indexMatrix[i][1];
			xPoint[i][0] = X[i][low];
			xPoint[i][1] = X[i][high];
			}

	for(i=0;i<nVertices;i++){
		for(j=0;j<nDimension;j++){
			mask = 1<<j;
			val = (mask & i) >> j;
			indexVector[j] = indexMatrix[j][val];
			}
		index = getLinIndex(indexVector,ndinfo);
		T[i] = Y[index];
		}
	result = linearInterpolate(T,x,xPoint,ndinfo);
	free(indexVector);
	free(T);
	freeIntMat(indexMatrix,nDimension,2);
	freeDoubleMat(xPoint,nDimension,2);
	return(result);
	}
