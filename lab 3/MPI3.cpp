#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <Windows.h>
#include <time.h>

int main(int* argc, char** argv)
{
	int numprocs, rank;
	bool finished = false;
	int counter = 0; 

	MPI_Init(argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	srand(time(NULL) + rank); // so every process will get different value of mes

	if (rank == 0)
	{
		std::cout << "Num of all processes: " << numprocs << std::endl;
	}

	int* large_data_storage = new int[numprocs];
	int* small_data_storage = new int();

	while (!finished)
	{
		if (rank == 0)
		{
			MPI_Gather(&small_data_storage[0], 1, MPI_INT,
				large_data_storage, 1, MPI_INT, 0, MPI_COMM_WORLD); // is &[] ness?
			int i = 1;
			while (large_data_storage[i] != -1 && i < numprocs)
			{
				counter++;
				//std::cout << "Proc 0. Received: " << large_data_storage[i]
				//	<< " on " << i << "..." << std::endl;
				i++;
			}
			// std::cout << i << std::endl;
			if (large_data_storage[i] == -1)
			{
				//std::cout << "Proc 0. Received: -1 on " << i << std::endl;
				finished = true;
				std::cout << "Proc " << rank << ". Got -1.\nProc finished" 
					<< std::endl;
				for (int i = 0; i < numprocs; i++)
				{
					large_data_storage[i] = -1;
				}
			}
			else 
			{
				std::cout << "Proc 0. Did not receive -1 " << std::endl;
			}
			MPI_Scatter(large_data_storage, 1, MPI_INT,
				&small_data_storage[0], 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
			//std::cout << "Barrier." << std::endl;
		}
		else
		{
			*small_data_storage = rand() % 10 - 1;
			MPI_Gather(&small_data_storage[0], 1, MPI_INT,
				large_data_storage, 1, MPI_INT, 0, MPI_COMM_WORLD);
			std::cout << "Proc " << rank << ". Send " << *small_data_storage
				<< std::endl;
			MPI_Scatter(large_data_storage, 1, MPI_INT,
				&small_data_storage[0], 1, MPI_INT, 0, MPI_COMM_WORLD);
			//std::cout << "Proc " << rank << " received back. Small: " <<
			//	*small_data_storage << std::endl;
			if (*small_data_storage == -1)
			{
				finished = true;
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