// A Dynamic Programming based solution for 0-1 Knapsack problem
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"
// A utility function that returns maximum of two integers
int max(int a, int b) { return (a > b)? a : b; }


//subblock size
#define ROW 64
#define COL 512

int min(int i, int j) {
        return (i<j) ? i : j;
}
// Returns the maximum value that can be put in a knapsack of capacity W
static void knapSack(long int W, long int N, int weight[], int profit[], int rank_mpi, int nb_process, int *vectorFinal[])
{
//int K[N+1][W+1];
//int K[50000][100];

long int i, w;
int valueFinal = 0;
MPI_Status status,status2;
MPI_Request send_request,recv_request;
 int recv_rank = (rank_mpi-1)%nb_process;   //rank to receive data
 int send_rank = (rank_mpi+1)%nb_process; // rank to send data
long int size_block = 0;
long int inicioColum = 0;
if(rank_mpi != nb_process-1)
{
  size_block = (W/nb_process);  
}
else
  size_block = (W/nb_process) + (W%nb_process);  


int collumn = 0;
	if(rank_mpi == 0)
	{
		if((collumn)+size_block < W)
			collumn = (collumn)+size_block;
	}
	if(rank_mpi != 0)
	{
		MPI_Recv((void*)&collumn, 1, MPI_INT, recv_rank, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		inicioColum = collumn;
		if((collumn)+size_block < W-1)
			collumn = (collumn)+size_block;
		else
			collumn = W+1;
	}
	if(rank_mpi != nb_process-1)
	{
			MPI_Send((void*)&collumn, 1, MPI_INT, send_rank, 1, MPI_COMM_WORLD);	
	}
	
//if((collumn)+size_block < W)
//		collumn = (collumn)+size_block;
//		collumn = W;
printf("Collumn Start: %i \n", inicioColum);
printf("Collumn fim: %i \n", collumn);
for (i = 0; i <= N; i++)
{
	  recv_rank = (rank_mpi-1)%nb_process;   //rank to receive data
 	  send_rank = (rank_mpi+1)%nb_process; // rank to send data
 	  if(recv_rank < 0)
 	  {
 	  	recv_rank = nb_process-1;
 	  }
	

	if(i > 0)
	{
//	printf("Processador: %i esperando receber de %i \n", rank_mpi, recv_rank);
	MPI_Recv(vectorFinal[i-1], W+1, MPI_INT, recv_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//	printf("Processador: %i recebeu de %i \n", rank_mpi, recv_rank);
	 MPI_Wait(&send_request, &status2);
	}
	for (w = inicioColum; w < collumn; w++)
	{
		//	printf("\n Iniciou calculo com Vetor! \n ");
		//	for (i = 0; i < N; ++i)
		//	{
		//		printf("%d  - \n", vectorFinal[i]);
		//		printf("P %d  weight %d <= W:  %d- \n", profit[i-1],weight[i-1], w);
				
			//	printf("%ld:%ld:%d", W, N, K1[N][W]);
		//	}
		if (i==0 || w==0)
		{
			//K[i][w] = 0;
			vectorFinal[i][w]= 0;
	//		printf("Em i = %i w=%i Log: %i - processou: %i \n", i,w, rank_mpi, vectorFinal[i][w]);
		}
		else if (weight[i-1] <= w)
		{			
		//	printf("Log else 1: valor : %i - peso: %i\n", profit[i-1], weight[i-1]);
		//		printf("\n");
		//		printf("nova soma: %i \n", vectorFinal[i-1][w-weight[i-1]]);
				vectorFinal[i][w] = max(profit[i-1] + vectorFinal[i-1][w-weight[i-1]], vectorFinal[i-1][w]);				
		//		printf("Em i = %i w=%i Log: %i - weight[i-1]: %i profit %i > %i \n", i, w, rank_mpi, weight[i-1], profit[i-1], vectorFinal[i-1][w]);				
		}
		else 
		{
	//			printf("valor : %i - peso: %i\n", profit[i-1], weight[i-1]);
				vectorFinal[i][w] = vectorFinal[i-1][w];
	//			printf("Em i = %i w=%i Log: %i - processou: %i \n", i, w, rank_mpi, vectorFinal[i-1][w]);
		}

	//	if(w==W && i==N )
	//		printf("%ld:%ld: valor final : %i \n",W,N, vectorFinal[i][w] );

	}
	
	MPI_Isend(vectorFinal[i], collumn, MPI_INT, send_rank, 0, MPI_COMM_WORLD,&send_request);
	//MPI_Isend( buffer,count,type,dest,tag,comm,request)
//	printf("Processador: %i enviou para %i \n", rank_mpi, send_rank);


	
}
if(rank_mpi == 0)
{
//	printf("Processador: %i esperando receber de %i \n", rank_mpi, recv_rank);
	MPI_Recv(vectorFinal[i-1], W+1, MPI_INT, recv_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("Processador: %i recebeu de %i \n", rank_mpi, recv_rank);
	 MPI_Wait(&send_request, &status2);
}

//MPI_Gather(vectorParticionado[2],(size_block), MPI_INT,vectorFinal[2],(size_block),MPI_INT, 0, MPI_COMM_WORLD);

//if(rank_mpi == 0)
//{ 
//	printf("Vector Final: \n");
//for(i = 0; i < N; i++)
//{
///	for(w = 0; w < W; w++)
///	{
//		printf(" %i : ", vectorFinal[i][w]);
//	}
//		printf(" \n");
//}
///printf("HOLA3\n");
// Build table K[][] in bottom up manner
//for (i = 0; i <= N; i++)
//{	
//		for (w =  rank_mpi*size_block; w <(rank_mpi+1)*size_block; w++)
		//for (w =  0; w <=W ; w++)
//		{
//			if (i==0 || w==0)
//				K[i][w] = 0;
//			else if (wt[i-1] <= w)
			//{					
//				//	if(rank_mpi != 0)
				//	{
	//				K[i][w] = max(val[i-1] + K[i-1][w-wt[i-1]], K[i-1][w]);																
									//	}					
////			else
//				//	if(rank_mpi != 0)
				//	{
	//					K[i][w] = K[i-1][w];														
				//	}					
//			}
		//	if(rank_mpi != 0)
		//	{
		//		MPI_Send ( &K[i][w], 1, MPI_INT, 0, 1, MPI_COMM_WORLD );	
		//	}
		//	else
		//	{
		//		for ( i = 1; i < nb_process; i++ ) 
    	//		{
		//			MPI_Recv ( &valueFinal, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status );
		//		}
		//	}
	//	}
	//MPI_Barrier(MPI_COMM_WORLD);
		
// // MPI_Recv ( &K[i][W], 1, MPI_INT, MPI_ANY_SOURCE, 0, 
  //      MPI_COMM_WORLD, MPI_STATUS_IGNORE );


 
	//printf("%ld:%ld:%d", W, N, valueFinal);
//return K1[N][W];
}

int main(int argc, char **argv){

    FILE *test_file;
    
    int *val, *wt;   // width and cost values
    int **vectorFinal;
    
    long int Nitems; // Number of items
    long int Width;  // Max. load to carry
    long int cont;    // counter
    double tpivot1=0,tpivot2=0,tpivot3=0; //time counting
    struct timeval tim;
    int i;
	int rank_mpi, nb_process;

	
	if (argc!=2) {
	  printf("\n\nError, mising parameters:\n");
	  printf("format: test_file \n");
	  return 1;
	} 
	
    //Capture first token time - init execution
    gettimeofday(&tim, NULL);
    tpivot1 = tim.tv_sec+(tim.tv_usec/1000000.0);
    
	if (!(test_file=fopen(argv[1],"r"))) {
	  printf("Error opening Value file: %s\n",argv[1]);
	  return 1;
	}
	
//	if(rank_mpi == 0)
//	{   
  	  //Reading number of items and Maximum width
		fscanf(test_file,"%ld %ld\n",&Nitems, &Width);    
		val = (int *)malloc(Nitems*sizeof(int)); //values for each element
		wt = (int *)malloc(Nitems*sizeof(int)); //width  for each element
	  	vectorFinal =  (int**) malloc((Nitems+1)*sizeof(int*));//vector Resultados	  	
		for (i=0;i<=Nitems;i++)
    		vectorFinal[i] = (int*)malloc((Width+1)*sizeof(int));
	 
		//Reading value and width for each element
		for (cont=0;cont<Nitems;cont++)
		{
			fscanf(test_file,"%d,%d\n",&val[cont],&wt[cont]);
		}
//	}

// 
//  The master process broadcasts the computed initial values 
//  to all the other processes.
//
 // MPI_Bcast ( val, Nitems, MPI_INT, 0, MPI_COMM_WORLD );
//  MPI_Bcast ( wt, Nitems, MPI_INT, 0, MPI_COMM_WORLD );

    gettimeofday(&tim, NULL);
    tpivot2 = (tim.tv_sec+(tim.tv_usec/1000000.0));
	MPI_Init(&argc, &argv); 

	MPI_Comm_rank(MPI_COMM_WORLD, &rank_mpi); 
	MPI_Comm_size(MPI_COMM_WORLD, &nb_process);
	
	knapSack(Width,Nitems, wt, val, rank_mpi, nb_process,vectorFinal);                    
    
  //  printf("num pros: %i", nb_process);
   
   MPI_Barrier(MPI_COMM_WORLD);
   
if(rank_mpi == 0)
	{
	printf("%ld:%ld: %i ",Width,Nitems, vectorFinal[Nitems][Width-1] );
    gettimeofday(&tim, NULL);
    tpivot3 = (tim.tv_sec+(tim.tv_usec/1000000.0));
 	printf(":%.6lf:%.6lf\n", tpivot3-tpivot2,tpivot3-tpivot1);    
	free(val);
	free(wt);
	free(vectorFinal);
	fclose(test_file);
    }


	MPI_Finalize();

	return 0;
}
