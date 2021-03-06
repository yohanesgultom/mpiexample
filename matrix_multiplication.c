#include"stdio.h"
#include"stdlib.h"
#include"mpi.h"
#define X 4
#define N 1
#define Y 4
#define filename "matrix_multiplication.txt"

/**
 * Print multidimensional array
 */
void print_array(int **matrix, int row, int col)
{
  int i, j = 0;
  for(i = 0; i < row; i++)
  {
    for(j = 0; j < col; j++)
    {
      printf("%d\t",matrix[i][j]);
    }
    printf("\n");
  }
}

/**
 * multiply two arrays
 */
void multiply_two_arrays(int x, int n, int y, int size, int rank)
{
  int i, j, k, sum=0;
  int **matrix1;  //declared matrix1[x][n]
  int **matrix2; //declare matrix2[n][y]
  int **mat_res; //resultant matrix become mat_res[x][y]
  double t1,t2,result;
  MPI_Status status;

  /*----------------------------------------------------*/
  //create array of pointers(Rows)
  matrix1 =(int **)malloc(x * sizeof(int*));
  matrix2 =(int **)malloc(n * sizeof(int*));
  mat_res=(int **)malloc(x * sizeof(int*));
  /*----------------------------------------------------*/

  /*--------------------------------------------------------------------------------*/
  //allocate memory for each Row pointer
  for(i = 0; i < x; i++)
  {
    matrix1[i]=(int *)malloc(n * sizeof(int));
    mat_res[i]=(int *)malloc(y * sizeof(int));
  }

  for(i = 0; i < n; i++)
  matrix2[i]=(int *)malloc(y*sizeof(int));
  /*--------------------------------------------------------------------------------*/

  for(i = 0; i < x; i++)
  {
    for(j = 0; j < n; j++)
    {
      matrix1[i][j] = 1; //initialize 1 to matrix1 for all processes
    }
  }

  for(i = 0; i < n; i++)
  {
    for(j = 0; j < y; j++)
    {
      matrix2[i][j] = 2;//initialize 2 to matrix2 for all processes
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0)
  t1=MPI_Wtime();
  // size = number of process. so each process will only calculate once (one row * one column)
  for(i = rank; i < x; i = i+size)
  // divide the task in multiple processes
  // yohanes: each process[i] will calculate result[i][j] = matrix1.row[i] * matrix1.column[i]
  {
    for(j = 0; j < y; j++)
    {
      sum=0;
      for(k = 0; k < n; k++)
      {
        sum = sum + matrix1[i][k] * matrix2[k][j];
      }
      mat_res[i][j] = sum;
    }
  }

  if(rank != 0)
  {
    for(i = rank; i < x; i = i+size)
    MPI_Send(&mat_res[i][0], y, MPI_INT, 0, 10+i, MPI_COMM_WORLD);//send calculated rows to process with rank 0
  }

  if(rank == 0)
  {
    for(j = 1; j < size; j++)
    {
      for(i = j; i < x; i = i+size)
      {
        MPI_Recv(&mat_res[i][0], y, MPI_INT, j, 10+i, MPI_COMM_WORLD, &status);//receive calculated rows from respective process
      }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0)
  t2 = MPI_Wtime();
  result = t2 - t1;
  if(rank == 0)
  {
    // print input matrices
    print_array(matrix1, x, n);
    printf("\n*\n\n");
    print_array(matrix2, n, y);
    // print result
    printf("\n=\n\n");
    print_array(mat_res, x, y);
  }
  if(rank == 0)
  printf("\nTime taken = %f seconds\n",result); //time taken
}

int main(int argc , char **argv)
{
  // read from file
  int x, n, y = 0;
  int size,rank = 0;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  FILE *fp = fopen(filename, "r");

  while (!feof(fp))
  {
    if (fscanf(fp, "%d %d %d", &x, &n, &y) == 3)
    {
        printf("X = %d N = %d Y= %d\n", x, n, y);
        multiply_two_arrays(x, n, y, size, rank);
        printf("\n==========\n");
    }
  }

  MPI_Finalize();
  return 0;
}
