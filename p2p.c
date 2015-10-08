#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main (int argc, char* argv[]) {
  int my_rank, p, tag = 0;
  char msg[20];
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (my_rank == 0) {
    strcpy(msg, "Hello ");
    MPI_Send(msg, strlen(msg)+1, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
    printf("Process #%i sent message: %s\n", my_rank, msg);
  }

  if (my_rank == 1) {
    MPI_Recv(msg, 20, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
    printf("Process #%i received message: %s\n", my_rank, msg);
  }

  MPI_Finalize();

  return 0;
}
