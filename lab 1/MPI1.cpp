// MPI1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <Windows.h>
#include <time.h>

int main(int* argc, char** argv)
{
	//Variables init
	int numtasks, rank;
	int mes = 0, procNum = 0;

	bool finished = false;
	MPI_Status status;
	srand(time(NULL));

	//MPI starts
	MPI_Init(argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	//Main proccess show how many tasks used
	if (rank == 0)
	{
		std::cout << "Num of all tasks: " << numtasks << std::endl;
	}

	//Proccesses cycle
	while (!finished)
	{
		//Main proccess part
		if (rank == 0)
		{
			static int counter = 0;
			MPI_Recv(&mes, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			//finish counter
			if (mes == -1)
			{
				std::cout << "Proc " << rank << " Get: " << mes << " From: " << status.MPI_SOURCE << " Counter: " << counter << " Proc finished" << std::endl;
				finished = true;
				//rend reply to kill proccess
				for (int i = 1; i < numtasks; i++)
					MPI_Send(&mes, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}

			//Increment and show value of counter
			else
			{
				counter++;
				std::cout << "Proc " << rank << " Get: " << mes << " From: " << status.MPI_SOURCE << " Counter: " << counter << std::endl;
				MPI_Send(&mes, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			}
		}

		//Other proccesses part
		else
		{
			//random message
			mes = rand() % 10 - 1;
			//send to counter
			MPI_Send(&mes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			//wait reply
			MPI_Recv(&mes, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			//if reply == -1 finish proccess
			if (mes == -1)
			{
				finished = true;
				std::cout << "Proc " << rank << " finished" << std::endl;
			}
			//delay
			Sleep(1);
		}
	}
	MPI_Finalize();
	return 0;
}
