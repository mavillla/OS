#include <iostream>
#include <windows.h>

void SetTerminationEvent() 
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

int main(int args, char * argv[])
{
    HANDLE mutex, events[3];
    
    mutex = OpenMutex(SYNCHRONIZE, FALSE, L"writer_mutex");
    if (mutex == NULL) 
    {
        std::cerr << "Can't open mutex\n";
        system("pause");
        SetTerminationEvent();
        return GetLastError();
    }

    if (args != 2)
    {
        std::cerr << "Wrong parameters amount\n";
        system("pause");
        SetTerminationEvent();
        CloseHandle(mutex);
        return GetLastError();
    }
    int n = atoi(argv[1]);

    if (n <= 0)
    {
        std::cerr << "Wrong messages amount\n";
        SetTerminationEvent();
        CloseHandle(mutex);
        system("pause");
        return GetLastError();
    }

    events[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"A_writer");
    events[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"B_writer");
    events[2] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"C_writer");

    for (size_t i = 0; i < 3; ++i) 
        if (events[i] == NULL)
        {
            for (size_t j = 0; j < 3; ++j)
                if (events[j] != NULL)
                    CloseHandle(events[i]);
            std::cerr << "Can't open event\n";
            system("pause");
            SetTerminationEvent();
            CloseHandle(mutex);
            return GetLastError();
        }

    for (size_t i = 0; i < n; ++i)
    {
        WaitForSingleObject(mutex, INFINITE); 

        char res;
        std::cout << "Choose the message type: \n"
            "A\n"
            "B\n"
            "C\n"
            "input: ";

        std::cin >> res;
        switch (res)  
        {
        case 'A':
            SetEvent(events[0]);
            break;
        case 'B':
            SetEvent(events[1]);
            break;
        case 'C':
            SetEvent(events[2]);
            break;
        default:
            std::cerr << "Wrong data\n";
            --i;
        }
        ReleaseMutex(mutex); 
    }

    std::cout << "I'm done!\n";
    system("pause");
    SetTerminationEvent(); 
    for (size_t i = 0; i < 3; ++i)
        CloseHandle(events[i]);
    CloseHandle(mutex);
    return 0;
}

