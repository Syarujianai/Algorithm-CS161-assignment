/*

The goal of this problem is to implement the "Median Maintenance" algorithm
(covered in the Week 3 lecture on heap applications). The text file contains
a list of the integers from 1 to 10000 in unsorted order; you should treat
this as a stream of numbers, arriving one by one. Letting x_i denote the i th
number of the file, the k th median m_k is defined as the median of the
numbers x_1,…,x_k. (So, if k is odd, then m_kmk is ((k+1)/2)((k+1)/2)th
smallest number among x_1,…,x_k

In the box below you should type the sum of these 10000 medians, modulo 10000
(i.e., only the last 4 digits). That is, you should compute
(m1+m2+m3+⋯+m10000)mod10000.

OPTIONAL EXERCISE: Compare the performance achieved by heap-based and search-tree-based implementations of the algorithm.

*/

#include <iostream>
#include <fstream>
#include <string>
#include <queue>

using namespace std;

// refer: https://www.geeksforgeeks.org/implement-min-heap-using-stl/
int main()
{
	ifstream fin("Median.txt", fstream::in | fstream::out);

	priority_queue <int> low_heap;  // max heap
	priority_queue <int, vector<int>, greater<int>> high_heap; // min heap

	int num, median = 0;
	int count_num = 0;
	long long sum = 0;
	while (fin >> num) {
		count_num++;
		// first entry
		if (low_heap.empty() && high_heap.empty())
			low_heap.push(num);
		else {
			// else entries
			if (num <= low_heap.top())
				low_heap.push(num);
			else high_heap.push(num);
		}

		// balance high and low heap
		if (int(high_heap.size() - low_heap.size()) >= 2) {
			low_heap.push(high_heap.top());
			high_heap.pop();
		}
		else if (int(low_heap.size() - high_heap.size()) >= 2) {
			high_heap.push(low_heap.top());
			low_heap.pop();
		}

		// compute sum of median
		if (high_heap.size() - low_heap.size() == 1)
			median = high_heap.top();
		else if (low_heap.size() - high_heap.size() == 1)
			median = low_heap.top();
		else if (low_heap.size() == high_heap.size())
			median = (high_heap.top() + low_heap.top()) / 2;
		sum += median;
	}
	fin.close();

	cout << "sum modulo of median: " << sum % 10000 << endl;

	return 0;
}

