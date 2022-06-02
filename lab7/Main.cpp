#include <math.h>
#include <time.h>
#include "MonitorStack.cpp"
#include <stack>

using namespace std;

CRITICAL_SECTION criticalSection;
HANDLE empty1;
HANDLE full;

struct TransmitedParametrs {
	MonitorStack* stack;
	int number;
	int threadNumder;

	TransmitedParametrs(MonitorStack*& st, int num, int threadNum) {
		stack = st;
		number = num;
		threadNumder = threadNum;
	}
};

struct ThreadData {
	HANDLE handle;
	DWORD id;
};

DWORD WINAPI producer(LPVOID v) {
	TransmitedParametrs* parametrs = (TransmitedParametrs*)v;

	for (int i = 0; i < parametrs->number; i++) {


		WaitForSingleObject(empty1, INFINITE);

		EnterCriticalSection(&criticalSection);

		double c = rand() % 250;
		parametrs->stack->push(c);
		cout << "Поток: " << parametrs->threadNumder << " Произвёл число: \"" << c << "\"\n";
		parametrs->stack->print();

		LeaveCriticalSection(&criticalSection);
		Sleep(30);

		while (!ReleaseSemaphore(full, 1, NULL));
	}

	return 0;
}

DWORD WINAPI consumer(LPVOID v) {
	TransmitedParametrs* parametrs = (TransmitedParametrs*)v;

	for (int i = 0; i < parametrs->number; i++) {

		WaitForSingleObject(full, INFINITE);

		EnterCriticalSection(&criticalSection);

		double character = parametrs->stack->pop();
		cout << "Поток: " << parametrs->threadNumder << " Съел число: \"" << character << "\"\n";
		parametrs->stack->print();

		LeaveCriticalSection(&criticalSection);

		Sleep(7);
		while (!ReleaseSemaphore(empty1, 1, NULL));
	}



	return 0;
}

int main() {
	setlocale(LC_ALL, "Russian");
	srand(time(NULL));

	cout << "Введите размер стека:\n";
	int size;
	cin >> size;

	empty1 = CreateSemaphore(NULL, size, size, L"Empty");
	full = CreateSemaphore(NULL, 0, size, L"Full");
	InitializeCriticalSection(&criticalSection);

	MonitorStack* stack = new MonitorStack(size);

	cout << "Введите кол-во потоков producer:\n";
	int producerThreadCount;
	cin >> producerThreadCount;

	int* producerProductCountList = new int[producerThreadCount];
	for (int i = 0; i < producerThreadCount; i++) {
		cout << "Введите кол-во продуктов для " << i + 1 << "-ого потока producer:\n";
		double producerProductCount;
		cin >> producerProductCount;

		producerProductCountList[i] = producerProductCount;
	}

	cout << "Введите кол-во потоков consumer:\n";
	int consumerThreadCount;
	cin >> consumerThreadCount;

	double* consumerProductCountList = new double[consumerThreadCount];
	for (int i = 0; i < consumerThreadCount; i++) {
		cout << "Введите кол-во продуктов для " << i + 1 << "-ого потока consumer:\n";
		double consumerProductCount;
		cin >> consumerProductCount;

		consumerProductCountList[i] = consumerProductCount;
	}

	double threadsCount = producerThreadCount + consumerThreadCount;
	HANDLE arr[20];
	int inc = 0;

	ThreadData** producerThreadData = new ThreadData * [producerThreadCount];

	for (int i = 0; i < producerThreadCount; i++) {
		producerThreadData[i] = new ThreadData();
		TransmitedParametrs* tp = new TransmitedParametrs(stack, producerProductCountList[i], i + 1);
		producerThreadData[i]->handle = CreateThread(NULL, 0, producer, (void*&)(tp), 0, &producerThreadData[i]->id);

		arr[inc] = producerThreadData[i]->handle;
		inc++;
	}

	ThreadData** consumerThreadData = new ThreadData * [consumerThreadCount];

	for (int i = 0; i < consumerThreadCount; i++) {
		consumerThreadData[i] = new ThreadData();
		TransmitedParametrs* tp = new TransmitedParametrs(stack, consumerProductCountList[i], i + 1);
		consumerThreadData[i]->handle = CreateThread(NULL, 0, consumer, (void*)(tp), 0, &consumerThreadData[i]->id);

		arr[inc] = consumerThreadData[i]->handle;
		inc++;
	}

	WaitForMultipleObjects(producerThreadCount, arr, TRUE, INFINITE);

	system("pause");
	return 0;
}