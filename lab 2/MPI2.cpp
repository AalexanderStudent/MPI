#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <Windows.h>
#include <time.h>

int main(int* argc, char** argv)
{
	int numprocs, rank;
	bool finished = false;
	MPI_Status status;
	int mes = 1;
	int is_command;
	int counter = 0;

	MPI_Init(argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	srand(time(NULL) + rank); // so every process will get different value of mes

	if (rank == 0)
	{
		std::cout << "Num of all processes: " << numprocs << std::endl;
	}

	int* buf = new int();
	
	while (!finished)
	{
		if (rank == 0)
		{
			MPI_Reduce(&buf, &mes, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
			is_command = mes;
			// -1 -> send again though MPI_Send
			// else -> continue while
			MPI_Bcast(&is_command, 1, MPI_INT, 0, MPI_COMM_WORLD);
			if (mes == -1) // there's -1 in the batch
			{
				finished = true;
				std::cout << "Proc " << rank << ". Got " << mes << ".\nCounter: " 
					<< counter << ".\nProc finished" << std::endl;
				int* final_pieces = new int[numprocs - 1];
				for (int i = 1; i < numprocs; i++)
				{
					MPI_Recv(&final_pieces[i - 1], 1, MPI_INT, i, MPI_ANY_TAG,
						MPI_COMM_WORLD, &status);
					std::cout << "Left " << final_pieces[i - 1] << std::endl;
				}
				int i = 0;
				while (final_pieces[i] != -1 && i < numprocs)
				{
					counter ++;
					i++;
					// increase counter until we reach -1 message
				}
			}
			else // whole batch is not -1
			{
				counter += (numprocs - 1);
				std::cout << "Proc " << rank << ". Did not get -1. " 
					<< std::endl;
			}
			MPI_Barrier(MPI_COMM_WORLD);
			//std::cout << "Barrier." << std::endl;
		}
		else 
		{
			mes = rand() % 10 - 1;
			*buf = mes;
			
			MPI_Reduce(&buf[0], &mes, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
			std::cout << "Proc " << rank << ". Send " << mes << std::endl;
			MPI_Bcast(&is_command, 1, MPI_INT, 0, MPI_COMM_WORLD);
			if (is_command == -1)
			{
				finished = true;
				MPI_Send(&mes, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				std::cout << "Proc " << rank << " finished." << std::endl;
			}
			MPI_Barrier(MPI_COMM_WORLD);
			//std::cout << "Barrier." << std::endl;
		}
	}
	if (rank == 0)
	{
		std::cout << "\n Counter: " << counter << std::endl;
	}
	MPI_Finalize();
	
	return 0;
}
//Sleep(100);