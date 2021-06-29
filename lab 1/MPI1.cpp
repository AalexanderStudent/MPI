#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <Windows.h>
#include <time.h>

int main(int* argc, char** argv)
{
	int numprocs, rank;
	int mes = 0;

	bool finished = false;
	MPI_Status status;
	srand(time(NULL));

	MPI_Init(argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	if (rank == 0)
	{
		std::cout << "Num of all processes: " << numprocs << std::endl;
	}

	while (!finished)
	{
		if (rank == 0)
		{
			static int counter = 0;
			MPI_Recv(&mes, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			if (mes == -1)
			{
				std::cout << "Proc " << rank << " got " << mes << " from " << status.MPI_SOURCE
					<< " Counter: " << counter << " Proc 0 finished" << std::endl;
				finished = true;
				for (int i = 1; i < numprocs; i++)
					MPI_Send(&mes, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}

			else
			{
				counter++;
				std::cout << "Proc " << rank << " got " << mes << " from " << status.MPI_SOURCE 
					<< " Counter: " << counter << std::endl;
				MPI_Send(&mes, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			}
		}

		else
		{
			mes = rand() % 10 - 1;
			MPI_Send(&mes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Recv(&mes, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if (mes == -1)
			{
				finished = true;
				std::cout << "Proc " << rank << " finished" << std::endl;
			}
			Sleep(1);
		}
	}
	MPI_Finalize();
	return 0;
}
