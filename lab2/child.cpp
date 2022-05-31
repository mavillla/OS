#include <windows.h>
#include <iostream>

using namespace std;


int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i)
		for (int j = 1; j < argc - i; ++j)
			if (*argv[j] > *argv[j + 1]) {
				char* r = argv[j];
				argv[j] = argv[j + 1];
				argv[j + 1] = r;
			}
	for (int i = 1; i < argc; ++i)
		cout << argv[i] << " ";
	cin.get();
}