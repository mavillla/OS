#include <iostream>
#include <windows.h>


void SetTerminationEvent() // отправка сообщения о завершении процесса
{
	HANDLE event_termination = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"termination");
	if (event_termination != NULL)
	{
		SetEvent(event_termination);
		CloseHandle(event_termination);
	}
	else
		std::cerr << "Can't open termination event\n";
}

int main(int args, char* argv[])
{
	HANDLE semaphore, events[3];
	char msgs[] = { 'A', 'B', 'C' };

	if (args != 2)
	{
		std::cerr << "Wrong parameters amount\n";
		system("pause");
		SetTerminationEvent();
		return GetLastError();
	}
	int n = atoi(argv[1]);

	if (n <= 0)
	{
		std::cerr << "Wrong messages amount\n";
		SetTerminationEvent();
		system("pause");
		return GetLastError();
	}

	semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"reader_semaphore");
	
	if (semaphore == NULL) // завершение процесса при неудаче в открытии семафора
	{
		std::cerr << "Can't open semaphore\n";
		system("pause");
		return GetLastError();
	}
	
	events[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"A_reader");
	events[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"B_reader");
	events[2] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"C_reader");

	for(size_t i = 0; i < 3; ++i) // завершение процесса при неудачи в открытии одного из событий
		if (events[i] == NULL)
		{
			for (size_t j = 0; j < 3; ++j)
				if (events[j] != NULL)
					CloseHandle(events[i]);
			std::cerr << "Can't open event\n";
			system("pause");
			SetTerminationEvent();
			CloseHandle(semaphore);
			return GetLastError();
		}

	std::cout << "Messages from Administrator:\n";
	for (size_t i = 0; i < n; ++i)
	{
		WaitForSingleObject(semaphore, INFINITE); // ожидание доступа 
		//std::cout << "active\n";
		int index = WaitForMultipleObjects(3, events, FALSE, INFINITE) - WAIT_OBJECT_0; // ожидание соощения от Administrator
		std::cout << msgs[index] << std::endl;
		ReleaseSemaphore(semaphore, 1, NULL); 
	}

	std::cout << "I'm done!\n";
	system("pause");
	SetTerminationEvent();  //сообщение о завершении работы
	for (size_t i = 0; i < 3; ++i)
		CloseHandle(events[i]);
	CloseHandle(semaphore);
	return 0;
}