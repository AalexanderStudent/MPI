#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <windows.h>
#include <time.h>
#include <memory>
#include <vector>
#include <string>
#include <complex>
#define _USE_MATH_DEFINES
#include <math.h> 

using namespace std;

typedef complex<double> base;

void fastFourierTransform(vector<base>& a, bool invert) {
	int n = (int)a.size();
	if (n == 1) return;

	vector<base> a0(n / 2), a1(n / 2);
	for (int i = 0, j = 0; i < n; i += 2, ++j) {
		a0[j] = a[i];
		a1[j] = a[i + 1];
	}
	fastFourierTransform(a0, invert);
	fastFourierTransform(a1, invert);

	double ang = 2 * M_PI / n * (invert ? -1 : 1);
	base w(1), wn(cos(ang), sin(ang));
	for (int i = 0; i < n / 2; ++i) {
		a[i] = a0[i] + w * a1[i];
		a[i + n / 2] = a0[i] - w * a1[i];
		if (invert)
			a[i] /= 2, a[i + n / 2] /= 2;
		w *= wn;
	}
}

void multiply(const vector<int>& a, const vector<int>& b, vector<int>& res) {
	vector<base> fa(a.begin(), a.end()), fb(b.begin(), b.end());
	size_t n = 1;
	while (n < max(a.size(), b.size())) n <<= 1;
	n <<= 1;
	fa.resize(n), fb.resize(n);

	fastFourierTransform(fa, false), fastFourierTransform(fb, false);
	for (size_t i = 0; i < n; ++i)
		fa[i] *= fb[i];
	fastFourierTransform(fa, true);

	res.resize(n);
	for (size_t i = 0; i < n; ++i)
		res[i] = int(fa[i].real() + 0.5);

	int carry = 0;
	for (size_t i = 0; i < n; ++i) {
		res[i] += carry;
		carry = res[i] / 10;
		res[i] %= 10;
	}
}

string displayNumber(int* number, int size)
{
	string str = "";
	bool isStartNulls = true;
	for (int i = size - 1; i >= 0; i--)
	{
		if (isStartNulls && number[i] == 0)
		{
		}
		else
		{
			isStartNulls = false;
			str += to_string(number[i]);
		}
	}
	if (isStartNulls)
	{
		str += "0";
	}
	return str + "\n";
}

void copyNumbers(int* first, int* second, int size)
{
	for (int i = 0; i < size; i++)
	{
		first[i] = second[i];
	}
}

string getNeighbors(int count, int* array)
{
	string result = "";
	for (int i = 0; i < count; i++)
	{
		result += to_string(array[i]) + " ";
	}
	return result;
}

int main(int* argc, char** argv)
{
	int numtasks, rank;
	int i;
	int* A, * B, * Z;

	MPI_Status status;
	MPI_Init(argc, &argv);
	//int size = atoi(argv[2]);
	int size = 3;

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	int index[] = { 1,2,3,4,6,9,12 };
	int edges[] = { 5,5,6,6,5,6,0,1,4,2,3,4 };

	MPI_Comm FOURIER_COMM;
	MPI_Graph_create(MPI_COMM_WORLD, 7, index, edges, 1, &FOURIER_COMM);
	MPI_Comm_rank(FOURIER_COMM, &rank);

	MPI_Datatype MPI_LONG_NUMBER;
	MPI_Type_contiguous(size * numtasks, MPI_INT, &MPI_LONG_NUMBER);
	MPI_Type_commit(&MPI_LONG_NUMBER);

	srand(time(NULL) + rank);

	int neighborsCount;
	MPI_Graph_neighbors_count(FOURIER_COMM, rank, &neighborsCount);

	int* neighbors = new int[neighborsCount];
	MPI_Graph_neighbors(FOURIER_COMM, rank, neighborsCount, neighbors);

	if (neighborsCount == 1)
	{
		A = new int[size];
		B = new int[size];
		Z = new int[size * 2];

		for (i = 0; i < size; i++)
		{
			A[i] = rand() % 9 + 1;
			B[i] = rand() % 9 + 1;
			Z[i] = 0;
		}

		vector<int> Av(A, A + size);
		vector<int> Bv(B, B + size);
		vector<int> Zv(Z, Z + size);

		multiply(Av, Bv, Zv);

		copyNumbers(Z, Zv.data(), size * 2);

		cout << to_string(rank) + "\n\tA:\t" + displayNumber(A, size)
			+ "\n\tB:\t" + displayNumber(B, size)
			+ "\n\tZ:\t" + displayNumber(Z, size * 2) + "\n";

		MPI_Send(Z, 1, MPI_LONG_NUMBER, neighbors[0], 0, FOURIER_COMM);
	}

	Sleep(100);

	if (neighborsCount == 3)
	{
		size *= 2;

		A = new int[size];
		B = new int[size];
		Z = new int[size * 2];

		MPI_Recv(A, 1, MPI_LONG_NUMBER, neighbors[0], 0, FOURIER_COMM, &status);
		MPI_Recv(B, 1, MPI_LONG_NUMBER, neighbors[1], 0, FOURIER_COMM, &status);

		vector<int> Av(A, A + size);
		vector<int> Bv(B, B + size);
		vector<int> Zv(Z, Z + size);

		multiply(Av, Bv, Zv);

		copyNumbers(Z, Zv.data(), size * 2);

		cout << to_string(rank) + "\n\tA:\t" + displayNumber(A, size)
			+ "\n\tB:\t" + displayNumber(B, size)
			+ "\n\tZ:\t" + displayNumber(Z, size * 2) + "\n";

		MPI_Send(Z, 1, MPI_LONG_NUMBER, neighbors[2], 0, FOURIER_COMM);
	}

	Sleep(100);

	if (neighborsCount == 2)
	{
		size *= 4;
		A = new int[size];
		B = new int[size];
		Z = new int[size * 2];

		MPI_Recv(A, 1, MPI_LONG_NUMBER, neighbors[0], 0, FOURIER_COMM, &status);
		MPI_Recv(B, 1, MPI_LONG_NUMBER, neighbors[1], 0, FOURIER_COMM, &status);

		vector<int> Av(A, A + size);
		vector<int> Bv(B, B + size);
		vector<int> Zv(Z, Z + size);

		multiply(Av, Bv, Zv);

		copyNumbers(Z, Zv.data(), size * 2);

		cout << to_string(rank) + "\n\tA:\t" + displayNumber(A, size)
			+ "\n\tB:\t" + displayNumber(B, size)
			+ "\n\tZ:\t" + displayNumber(Z, size * 2) + "\n";
	}

	//Sleep(100);

	MPI_Finalize();

	return 0;
}