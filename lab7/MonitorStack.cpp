#include <windows.h>
#include <iostream>

using namespace std;

class MonitorStack {
private:
	double* arr;
	int size;
	int pos;

public:
	MonitorStack(int size) {
		this->size = size;
		arr = new double[size];
		pos = 0;
	}

	~MonitorStack() {
		delete[]arr;
	}

	void push(double element) {
		if (pos < size) {
			arr[pos++] = element;
		}
	}

	char pop() {
		if (pos > 0) {
			return arr[--pos];
		}
	}

	void print() {
		for (int i = 0; i < pos; i++) {
			cout << "\"" << arr[i] << "\" ";
		}
		cout << endl;
	}
};
