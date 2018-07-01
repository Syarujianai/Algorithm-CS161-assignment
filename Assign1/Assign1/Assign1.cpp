// Assign1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "BigInteger.h"

using namespace std;

int main()
{
	BigInteger num0 = "3141592653589793238462643383279502884197169399375105820974944592";
	BigInteger num1 = "2718281828459045235360287471352662497757247093699959574966967627";
	cout << (num0*num1);
    return 0;
}

