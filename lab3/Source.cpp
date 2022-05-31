#include <windows.h>
#include <iostream>

using namespace std;



CRITICAL_SECTION CSArr;
CRITICAL_SECTION CSArr2;
HANDLE SumEEvent;
struct MyStruct { int k; double* arr; };

DWORD WINAPI work(void* p) {
	EnterCriticalSection(&CSArr);

	UINT count;
	cout << "¬ведите count: ";
	cin >> count;
	MyStruct* str = (MyStruct*)p;

	for (int i = 1; i < str->k; ++i) {
		for (int j = 0; j < str->k - i; ++j)
			if (str->arr[j] > str->arr[j + 1])
				swap(str->arr[j], str->arr[j + 1]);
		Sleep(count);
	}
	LeaveCriticalSection(&CSArr);
	return 0;
}

DWORD WINAPI SumElement(void* p) {
	EnterCriticalSection(&CSArr2);
	WaitForSingleObject(SumEEvent, INFINITE);

	MyStruct* str = (MyStruct*)p;
	double res = 0.;
	for (int i = 0; i < str->k; ++i)
		res += str->arr[i];
	cout << "SumElement result : " << res << '\n';
	LeaveCriticalSection(&CSArr2);
	return 0;
}

int main() {
	setlocale(LC_ALL, "ru");
	InitializeCriticalSection(&CSArr);
	InitializeCriticalSection(&CSArr2);
	SumEEvent = CreateEvent(NULL, FALSE, FALSE, L"SumEEvent");
	HANDLE hWork, hSumE;
	DWORD iWork, iSumE;
	int n, k;
	cin >> n;
	double* arr = new double[n];
	for (int i = 0; i < n; ++i)
		cin >> arr[i];
	cin >> k;


	hWork = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)work, new MyStruct{ k,arr }, NULL, &iWork);
	hSumE = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SumElement, new MyStruct{ k,arr }, NULL, &iSumE);
	Sleep(50);

	EnterCriticalSection(&CSArr);
	cout << "Sorted k elements : ";
	for (int i = 0; i < k; ++i)
		cout << arr[i] << "  ";
	cout << '\n';
	SetEvent(SumEEvent);
	LeaveCriticalSection(&CSArr);

	//Sleep(50);

	EnterCriticalSection(&CSArr2);
	cout << "Print after k-th elem : ";
	for (int i = k; i < n; ++i)
		cout << arr[i] << "  ";
	LeaveCriticalSection(&CSArr2);


	DeleteCriticalSection(&CSArr);
	DeleteCriticalSection(&CSArr2);
	CloseHandle(hWork);
	CloseHandle(hSumE);
	CloseHandle(SumEEvent);
}