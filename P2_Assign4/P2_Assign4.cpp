/*
The goal of this problem is to implement a variant of the 2-SUM algorithm 
covered in this week's lectures.

The file contains 1 million integers, both positive and negative (there might
be some repetitions!).This is your array of integers, with the i^th row of 
the file specifying the i^th entry of the array.

Your task is to compute the number of target values t in the interval 
[-10000,10000] (inclusive) such that there are distinct numbers x,y in the
input file that satisfy x+y=t. (NOTE: ensuring distinctness requires a one
-line addition to the algorithm from lecture.)

Write your numeric answer (an integer between 0 and 20001) in the space 
provided.

OPTIONAL CHALLENGE: If this problem is too easy for you, try implementing 
your own hash table for it. For example, you could compare performance under
the chaining and open addressing approaches to resolving collisions.
*/

//there are distinct numbers x,y in the input file that satisfy x + y = t
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>  // refer: https://stackoverflow.com/questions/3578083/what-is-the-best-way-to-use-a-hashmap-in-c	
#include <sstream>
using namespace std;

int main()
{
	ifstream fin("algo1-programming_prob-2sum.txt", fstream::in | fstream::out);
	
	unordered_map<int, int> num_dict;
	
	// insert all numbers into hashmap
	int num;
	string str;
	
	while (getline(fin, str)) {
		stringstream ss;
		ss.str(str);
		ss >> num;
		num_dict[num] = 1;
	}
	
	// search 2-sum t between [-10000,10000]
	for (int t = - 10000; t <= 10000; t++) {
		bool trial = false;
		for (auto iter = num_dict.begin(); iter != num_dict.end(); iter++) {
			int key = iter->first;
			if (num_dict.find(t - key) != num_dict.end()) {
				trial = true; break;
			}
		}
		if (trial == true) {
			cout << "2-sum: " << t << endl; break;
		}
	}
	fin.close();
    
}