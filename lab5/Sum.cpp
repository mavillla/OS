#include <iostream>
#include <string>
#include <Windows.h>

double AllSumActions(double N, double* arr, size_t array_size)
{
	double sum = 0;
	for (size_t i = 0; i < array_size; ++i)
	{
		if (arr[i] - N >= DBL_EPSILON)
		{
			std::cerr << arr[i] << std::endl;
			arr[i] *= arr[i];
			sum += arr[i];
		}
		else
			arr[i] *= arr[i];
	}

	return sum;
}

int main()
{
	HANDLE hRpipe = GetStdHandle(STD_INPUT_HANDLE),
		hWpipe = GetStdHandle(STD_OUTPUT_HANDLE),
		event = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"event");

	if (event == NULL)
	{
		std::cerr << "Can't open event\n";
		return GetLastError();
	}

	if (hWpipe == INVALID_HANDLE_VALUE || hRpipe == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Can't get anonymous pipe\n";
		return GetLastError();
	}

	double N;
	size_t array_size;
	double* arr;


	std::cerr << "Waiting for data...\n";
	if (!ReadFile(hRpipe, &N, sizeof(double), NULL, NULL) ||
		!ReadFile(hRpipe, &array_size, sizeof(size_t), NULL, NULL))
	{
		std::cerr << "Can't read from pipe\n";
		CloseHandle(hRpipe);
		CloseHandle(hWpipe);
		CloseHandle(event);
		return GetLastError();
	}

	arr = new double[array_size];

	if(!ReadFile(hRpipe, arr, array_size * sizeof(double), NULL, NULL))
	{
		std::cerr << "Can't read from pipe\n";
		delete[] arr;
		CloseHandle(hRpipe);
		CloseHandle(hWpipe);
		CloseHandle(event);
		return GetLastError();
	}

	std::cerr << "Got all info!\n";
	std::cerr << "N = " << N << std::endl;
	std::cerr << "Values more than N:\n";
	double sum = AllSumActions(N, arr, array_size);
	std::cerr << "Sum of elements more than N: " << sum << std::endl;

	if (!WriteFile(hWpipe, &sum, sizeof(double), NULL, NULL) || 
		!WriteFile(hWpipe, arr, array_size * sizeof(double), NULL, NULL))
	{
		std::cerr << "Can't write to pipe\n";
		delete[] arr;
		CloseHandle(hRpipe);
		CloseHandle(event);
		CloseHandle(hWpipe);
		return GetLastError();
	}

	SetEvent(event);

	CloseHandle(hRpipe);
	CloseHandle(hWpipe);
	CloseHandle(event);
	delete[] arr;

	Sleep(30000);

	return 0;
}