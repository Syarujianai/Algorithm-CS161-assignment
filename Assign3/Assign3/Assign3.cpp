// Assign3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <map>

using namespace std;

enum PivotStrategy
{
	FIRST = 1,
	LAST,
	MEDIAN_OF_THREE
};

inline void swap(int &a, int &b) {
	int temp = a;
	a = b;
	b = temp;
}

int median(int x,int y,int z) {//you may use value arg instead of reference(&) arg, or sort algorithm will go wrong
	//bubble, z is maximum
	if (x > y) swap(x, y);
	if (y > z) swap(y, z);
	//return median value
	return x > y ? x : y;
}

/*
A: l~(r-1)
return: final position of chosen pivot
*/
int Partition(vector<int> &array, int l, int r, PivotStrategy strategy) {
	int pivot;
	switch (strategy)
	{
	case FIRST:
		pivot = array[l];//选择1st element为分割中心
		break;
	case LAST:
		pivot = array[r - 1];
		swap(array[r - 1], array[l]);
		break;
	case MEDIAN_OF_THREE:
		{
			map<int, int> indices;//store index of {first, middle, end}.
			int mid = (r -1 + l)/2;//;//middle element: (m - 1)/2+ l <=> l + (r - l - 1) / 2
			indices[array[l]] = l;
			indices[array[r - 1]] = r - 1;
			indices[array[mid]] = mid;
			if (r != l + 2) {
				pivot = median(array[l], array[mid], array[r - 1]);
				if (indices[pivot] != l) swap(array[l], array[indices[pivot]]);
			}
			else pivot = array[l];// if input size == 2,select first element as pivot as CASE FIRST.
			break;
		}
	default:
		break;
	}
	int i = l + 1;//i is the boundary of >pivot and <pivot
	for (int j = l + 1; j < r; j++) {//j is the boundary of no partition and partitioned.
		if (array[j] < pivot) {//Else if(A[j] > pivot), do nothing.
			if (j != i) swap(array[i], array[j]);
			i++;//在没有遇第一个大于pivot的数前，遇到小于pivot的数只i++不执行swap
		}
	}
	swap(array[i - 1], array[l]);
	return i - 1;
}

size_t QuickSortCount(vector<int> &array, int l, int r, PivotStrategy strategy) {
	if (r <= l + 1) return 0;//stop condition, r == l + 1 or r == l
	else {
		size_t compared_times = (r - l) - 1, left_count = 0, right_count = 0;//attention: r>l, so (r-l)-1
		int pivot_pos = Partition(array, l, r, strategy);
		left_count = QuickSortCount(array, l, pivot_pos, strategy);
		right_count = QuickSortCount(array, pivot_pos + 1, r, strategy);
		return compared_times + left_count + right_count;
	}
}

int main()
{
	ifstream fin("QuickSort.txt");
	ofstream fout;

	vector<int> arr1, arr2, arr3;
	int temp;

	while (fin >> temp) {
		arr1.push_back(temp);
		arr2.push_back(temp);
		arr3.push_back(temp);
	}
	fin.close();

	size_t  dataSize = arr1.size();
	cout << "Data Size:" << dataSize << endl;

	size_t compared_count1, compared_count2, compared_count3;
	compared_count1 = QuickSortCount(arr1, 0, dataSize, FIRST);
	compared_count2 = QuickSortCount(arr2, 0, dataSize, LAST);
	compared_count3 = QuickSortCount(arr3, 0, dataSize, MEDIAN_OF_THREE);
	cout << compared_count1 <<'\n'
		<< compared_count2 << '\n'
		<< compared_count3 << '\n' << endl;
	fout.open("SortedArray.txt", ios::out);
	for (auto num : arr3) {
		fout << num;
		fout << '\n';
	}
	fout.close();
	return 0;
}

