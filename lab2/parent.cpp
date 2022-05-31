#include <windows.h>
#include <iostream>
using namespace std;

void setSTP(STARTUPINFO* stp) {
	ZeroMemory(stp, sizeof(STARTUPINFO));
	stp->cb = sizeof(STARTUPINFO);
	stp->lpTitle = (LPWSTR)L"Hello world!";
	stp->dwFlags = STARTF_USEPOSITION | STARTF_USESIZE;
	stp->dwXSize = 800;
}

int main() {
	int n;
	cin >> n;
	string st("Child.exe");
	for (int i = 0; i < n; ++i) {
		char c;
		cin >> c;
		st.append(' ' + string(1, c));
	}
	wstring q(st.begin(), st.end());
	for (int i = 0;i < st.length();++i)
		q[i] = wchar_t(st[i]);

	STARTUPINFO stp;
	PROCESS_INFORMATION pi;
	setSTP(&stp);

	CreateProcess(NULL, &q[0], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &stp, &pi);

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);



	Sleep(1000);
	ZeroMemory(&stp, sizeof(STARTUPINFO));
	stp.cb = sizeof(STARTUPINFO);
	CreateProcess(L"Count.exe", NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &stp, &pi);

	Sleep(1000);
	TerminateProcess(pi.hProcess, 0);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);


	ExitProcess(0);
}