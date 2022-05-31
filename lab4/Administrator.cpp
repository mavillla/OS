#include <iostream>
#include <windows.h>
#include <vector>
#include <string>

int get_amount(std::string&& msg) 
{
	int N;
	std::string buff;
	std::cout << msg << std::endl;
	std::getline(std::cin, buff);
	return atoi(buff.c_str());
}

void CloseHandles(HANDLE mutex, HANDLE semaphore, HANDLE* events_writer,
	HANDLE* events_reader, HANDLE termination_event, int size = 3) 
{
	CloseHandle(mutex);
	CloseHandle(semaphore);
	CloseHandle(termination_event);
	for (size_t i = 0; i < size; ++i)
	{
		CloseHandle(events_reader[i]);
		CloseHandle(events_writer[i]);
	}
}

void AddProcessDataToVector(std::vector<std::pair<STARTUPINFO, PROCESS_INFORMATION>>& vec, wchar_t* title) 
{																										   
	vec.push_back(std::make_pair<STARTUPINFO, PROCESS_INFORMATION>(STARTUPINFO(), PROCESS_INFORMATION()));

	ZeroMemory(&(vec.back().first), sizeof(vec.back().first));
	ZeroMemory(&(vec.back().first), sizeof(vec.back().second));

	vec.back().first.cb = sizeof(vec.back().first);
	vec.back().first.lpTitle = title;
}

void CloseProcesses(std::vector<std::pair<STARTUPINFO, PROCESS_INFORMATION>>& vec) 
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
			CloseHandle(vec[i].second.hThread);
			CloseHandle(vec[i].second.hProcess);
	}
}
int main()
{
	std::vector<std::pair<STARTUPINFO, PROCESS_INFORMATION>> readers, writers;

	HANDLE mutex, 
		semaphore, 
		events_for_reader[3], 
		events_for_admin[4]; 
	const char* msgs[] = { "A", "B", "C", "C (session termination)"}; 
																	 

	int processes_n, messages_n; 
	processes_n = get_amount("Enter amount of readers and writers: ");
	if (processes_n <= 0 || processes_n > 10)
	{
		std::cerr << "Wrong data\n";
		return -1;
	}

	messages_n = get_amount("Enter amount of messages: ");
	if (messages_n <= 0)
	{
		std::cerr << "Wrong data\n";
		return -2;
	}

	readers.reserve(processes_n);
	writers.reserve(processes_n);

	mutex = CreateMutex(NULL, FALSE, L"writer_mutex");
	semaphore = CreateSemaphore(NULL, 1, 1, L"reader_semaphore");

	events_for_admin[0] = CreateEvent(NULL, FALSE, FALSE, L"A_writer");
	events_for_admin[1] = CreateEvent(NULL, FALSE, FALSE, L"B_writer");
	events_for_admin[2] = CreateEvent(NULL, FALSE, FALSE, L"C_writer");
	events_for_admin[3] = CreateEvent(NULL, FALSE, FALSE, L"termination"); 

	events_for_reader[0] = CreateEvent(NULL, FALSE, FALSE, L"A_reader");
	events_for_reader[1] = CreateEvent(NULL, FALSE, FALSE, L"B_reader");
	events_for_reader[2] = CreateEvent(NULL, FALSE, FALSE, L"C_reader");

	std::wstring reader_cmd_str = L"Reader.exe " + std::to_wstring(messages_n), 
		writer_cmd_str = L"Writer.exe " + std::to_wstring(messages_n); 
																	   

	for (size_t i = 0; i < processes_n; ++i)
	{
		std::wstring reader_title = L"Reader " + std::to_wstring(i + 1);
		std::wstring writer_title = L"Writer " + std::to_wstring(i + 1);

		AddProcessDataToVector(readers, const_cast<wchar_t*>(reader_title.c_str()));
		AddProcessDataToVector(writers, const_cast<wchar_t*>(writer_title.c_str()));


		if (!CreateProcess(NULL, const_cast<wchar_t*>(reader_cmd_str.c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &readers.back().first, &readers.back().second) ||
			!CreateProcess(NULL, const_cast<wchar_t*>(writer_cmd_str.c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &writers.back().first, &writers.back().second))
		{
			std::cerr << "Fail to create a process\n";

			system("pause"); 
			CloseHandles(mutex, semaphore, events_for_admin, events_for_reader, events_for_admin[3]);
			CloseProcesses(readers);
			CloseProcesses(writers);
			return GetLastError();
		}
	}
	processes_n *= 2; 
	std::cout << "Messages from Writers( and Readers):\n";
	while (processes_n)
	{
		int index = WaitForMultipleObjects(4, events_for_admin, FALSE, INFINITE) - WAIT_OBJECT_0; 
		std::cout << msgs[index] << std::endl;
		if (index == 3) 
		{
			--processes_n;
			continue;
		}
		SetEvent(events_for_reader[index]); 
	}

	std::cout << "Program is over\n";
	CloseHandles(mutex, semaphore, events_for_admin, events_for_reader, events_for_admin[3]);
	CloseProcesses(readers);
	CloseProcesses(writers);
	return 0;
}
