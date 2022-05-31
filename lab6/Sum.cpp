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


void customedSystemPause()
{
	std::string t;
	std::cout << "Press any button to continue...\n";
	std::getline(std::cin, t);
	return;
}

int main()
{
	HANDLE pipe;
	double N;
	size_t array_size;
	double* arr;

	std::wstring pipe_name;
	std::cout << "Enter server name: ";
	std::getline(std::wcin, pipe_name);
	pipe_name = L"\\\\" + pipe_name + L"\\pipe\\my pipe";

	pipe = CreateFile(pipe_name.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (pipe == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Can't open pipe\n";
		customedSystemPause();
		return GetLastError();
	}


	std::cout << "Waiting for data...\n";
	if (!ReadFile(pipe, &N, sizeof(double), NULL, NULL) ||
		!ReadFile(pipe, &array_size, sizeof(size_t), NULL, NULL))
	{
		std::cerr << "Can't read from pipe\n";
		customedSystemPause();
		CloseHandle(pipe);
		return GetLastError();
	}

	arr = new double[array_size];

	if (!ReadFile(pipe, arr, array_size * sizeof(double), NULL, NULL))
	{
		std::cerr << "Can't read from pipe\n";
		customedSystemPause();
		delete[] arr;
		CloseHandle(pipe);
		return GetLastError();
	}

	std::cerr << "Got all info!\n";
	std::cerr << "N = " << N << std::endl;
	std::cerr << "Values more than N:\n";
	system("mkdir ..\\\"\"");
	double sum = AllSumActions(N, arr, array_size);
	std::cerr << "Sum of elements more than N: " << sum << std::endl;
	std::cout << "Squared array:\n";
	for (size_t i = 0; i < array_size; ++i)
		std::cout << arr[i] << std::endl;

	if (!WriteFile(pipe, &sum, sizeof(double), NULL, NULL) ||
		!WriteFile(pipe, arr, array_size * sizeof(double), NULL, NULL))
	{
		std::cerr << "Can't write to pipe\n";
		customedSystemPause();
		delete[] arr;
		CloseHandle(pipe);
		return GetLastError();
	}

	std::cout << "Done!\n";
	customedSystemPause();

	CloseHandle(pipe);
	delete[] arr;
	return 0;
}