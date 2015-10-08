#include"stdio.h"
#include"stdlib.h"
#include"mpi.h"
#define N 4
#define X 3
#define Y 2
int main(int argc , char **argv)
{
  int size,rank,sum=0,i,j,k;
  int **matrix1;  //declared matrix1[X][N]
  int **matrix2; //declare matrix2[N][Y]
  int **mat_res; //resultant matrix become mat_res[X][Y]
  double t1,t2,result;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /*----------------------------------------------------*/
  //create array of pointers(Rows)
  matrix1 =(int **)malloc(X * sizeof(int*));
  matrix2 =(int **)malloc(N * sizeof(int*));
  mat_res=(int **)malloc(X * sizeof(int*));
  /*----------------------------------------------------*/


  /*--------------------------------------------------------------------------------*/
  //allocate memory for each Row pointer
  for(i = 0; i < X; i++)
  {
    matrix1[i]=(int *)malloc(N * sizeof(int));
    mat_res[i]=(int *)malloc(Y * sizeof(int));
  }

  for(i = 0; i < N; i++)
  matrix2[i]=(int *)malloc(Y*sizeof(int));
  /*--------------------------------------------------------------------------------*/

  for(i = 0; i < X; i++)
  {
    for(j = 0; j < N; j++)
    {
      matrix1[i][j] = 1; //initialize 1 to matrix1 for all processes
    }
  }

  for(i = 0; i < N; i++)
  {
    for(j = 0; j < Y; j++)
    {
      matrix2[i][j] = 2;//initialize 2 to matrix2 for all processes
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0)
  t1=MPI_Wtime();
  // size = number of process. so each process will only calculate once (one row * one column)
  for(i = rank; i < X; i = i+size)
  // divide the task in multiple processes
  // yohanes: each process[i] will calculate result[i][j] = matrix1.row[i] * matrix1.column[i]
  {
    for(j = 0; j < Y; j++)
    {
      sum=0;
      for(k = 0; k < N; k++)
      {
        sum = sum + matrix1[i][k] * matrix2[k][j];
      }
      mat_res[i][j] = sum;
    }
  }

  if(rank != 0)
  {
    for(i = rank; i < X; i = i+size)
    MPI_Send(&mat_res[i][0], Y, MPI_INT, 0, 10+i, MPI_COMM_WORLD);//send calculated rows to process with rank 0
  }

  if(rank == 0)
  {
    for(j = 1; j < size; j++)
    {
      for(i = j; i < X; i = i+size)
      {
        MPI_Recv(&mat_res[i][0], Y, MPI_INT, j, 10+i, MPI_COMM_WORLD, &status);//receive calculated rows from respective process
      }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0)
  t2 = MPI_Wtime();
  result = t2 - t1;
  if(rank == 0)
  {
    for(i = 0; i < X; i++)
    {
      for(j = 0; j < Y; j++)
      {
        printf("%d\t",mat_res[i][j]); //print the result
      }
      printf("\n");
    }
  }
  if(rank == 0)
  printf("Time taken = %f seconds\n",result); //time taken
  MPI_Finalize();
  return 0;
}
