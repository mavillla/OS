#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <iostream>
#include <process.h>

struct MyStruct {
	int count;
	short* mas;
};


UINT WINAPI thread(void* m) {
	Sleep(1000);
	MyStruct* strt = (MyStruct*)m;
	for (int i = 0; i < strt->count; ++i) {
		if (strt->mas[i] % 9 == 0 && strt->mas[i] < 0)
			std::cout << strt->mas[i] << '\n';
		Sleep(30);
	}
	return 0;
}


void fibonacci() {
	unsigned long long fst = 0, scd = 1;
	std::cout << fst << '\n';
	//int n = 1;
	while (true) {
		Sleep(50);
		std::cout << scd << '\n';
		unsigned long long next = scd + fst;
		fst = scd;
		scd = next;
		//n++;
	}
}


int main() {
	int n;
	std::cin >> n;

	short* mas = new short[n];

	for (int i = 0; i < n; ++i)
		std::cin >> mas[i];

	std::cout << "worker:\n";
	HANDLE worker, Count;
	UINT IDworker;
	DWORD IDCount;
	worker = (HANDLE)_beginthreadex(NULL, 0, thread, new MyStruct{ n, mas }, 0, &IDworker);
	if (worker == NULL)
		return GetLastError();
	Count = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fibonacci, NULL, CREATE_SUSPENDED, &IDCount);
	if (Count == NULL)
		return GetLastError();

	SuspendThread(worker);
	std::cout << "worker is stoped\n";
	//Sleep(1000);
	std::cout << "worker is started\n";
	ResumeThread(worker);


//	WaitForSingleObject(worker, INFINITE);
	
	Sleep(1000);
	ResumeThread(Count);
	Sleep(5000);
	SuspendThread(Count);
	WaitForSingleObject(Count, INFINITE);
	//CloseHand;
	CloseHandle(worker);
	return 0;

}