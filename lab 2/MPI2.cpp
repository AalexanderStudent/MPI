#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <Windows.h>
#include <time.h>

void eqNOne(int* in, int* out, int* len, MPI_Datatype* dptr)
{
	Sleep(10);
	if (*in == -1)
	{
		*out = -1;
	}
	std::cout << "out:: " << *out << " " << *in << std::endl;
}

int main(int* argc, char** argv)
{
	//Variables init
	int numtasks, rank;
	int mes = 0, procNum = 0;

	bool finished = false;
	MPI_Status status;

	//MPI starts
	MPI_Init(argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	//Creating my own operation
	MPI_Op eqNOne;
	MPI_Op_create((MPI_User_function*)eqNOne, 1, &eqNOne);


	srand(time(NULL) + rank);
	//Main proccess show how many tasks used
	if (rank == 0)
	{
		std::cout << "Num of all tasks: " << numtasks << std::endl;
		Sleep(1);
	}

	int* buf = new int();
	*buf = 0;

	//Proccesses cycle
	while (!finished)
	{

		//Main proccess part
		if (rank == 0)
		{
			Sleep(100);
			static int counter = 0;
			MPI_Reduce(&buf, &mes, 1, MPI_INT, eqNOne, 0, MPI_COMM_WORLD);

			//finish counter
			if (mes == -1)
			{
				finished = true;
				//rend reply to kill proccess
				std::cout << "Proc " << rank << " Get: " << mes << " Counter: " << counter << " Proc finished" << std::endl;
			}

			//Increment and show value of counter
			else
			{
				counter += numtasks;
				std::cout << "Proc " << rank << " Get not 1 " << " Counter: " << counter << std::endl;
			}
			MPI_Bcast(&mes, 1, MPI_INT, 0, MPI_COMM_WORLD);
		}

		//Other proccesses part
		else
		{
			//random message
			mes = rand() % 50 - 1;
			*buf = mes;
			Sleep(rank * 10);
			MPI_Reduce(buf, &mes, 1, MPI_INT, eqNOne, 0, MPI_COMM_WORLD);
			std::cout << "Proc " << rank << " Send " << mes << std::endl;
			MPI_Bcast(&mes, 1, MPI_INT, 0, MPI_COMM_WORLD);

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
