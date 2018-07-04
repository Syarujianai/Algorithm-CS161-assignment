// Assign2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
size_t MergeCountSplitInv(vector<int> &arr, int beg, int mid, int end);

/*
	arr: ���ظ�����
	beg: 0, end: n
*/
size_t MergeCount(vector<int> &arr, int beg, int end) {
	if (end == beg + 1) return 0;
	else {
		int mid = (beg + end) / 2;
		size_t leftCount, rightCount, splitCount;
		leftCount = MergeCount(arr, beg, mid);//left: beg~mid-1, divide
		rightCount = MergeCount(arr, mid, end);//right: mid~end-1, divide
		splitCount = MergeCountSplitInv(arr, beg, mid, end);//split: beg~end-1, conquer
		return leftCount + rightCount + splitCount;
	}
}

size_t MergeCountSplitInv(vector<int> &arr, int beg, int mid, int end) {
	int *c = new int[end - beg];//c: ����O(n)�����ռ�
	size_t inv_count_ad = 0, inv_count = 0;
	for (int k = 0, i = beg, j = mid; k < end - beg; k++) {
		if (j == end || (i != mid && arr[i] < arr[j])) {//��������sort��� �� arr[i] < arr[j], ��������δsort���
			c[k] = arr[i];
			if (i != mid) i++;//��i==mid����������з�Χ
			inv_count += inv_count_ad;
		}
		else {//arr[j] < arr[i], ��j != end - 1
			c[k] = arr[j];
			if (j != end ) j++;//j==end�����Ұ����з�Χ;
			inv_count_ad++;
		}
	}
	for (int k = 0; k < end - beg; k++)
		arr[k + beg] = c[k];
	delete[] c;
	return inv_count;
}

/*
	InputFile: "IntegerArray.txt"
	OutputFile: "SortedArray.txt"
*/
int main()
{
	ifstream fin("IntegerArray.txt");
	ofstream fout;

	vector<int> intArr;
	int temp;

	while (fin >> temp) {
		intArr.push_back(temp);
	}
	size_t  dataSize = intArr.size();
	cout << "Data Size:" << dataSize << endl;

	size_t count = MergeCount(intArr,0, dataSize);
	cout << count << endl;
	fout.open("SortedArray.txt", ios::out);
	for (auto num : intArr) {
		fout << num;
		fout << '\n';
	}
	fout.close();
	return 0;
}

