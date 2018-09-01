#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdio>
//#include <cstring>
#include <string>

using namespace std;

/*
	紫书p124大整数类(非负)
*/
class BigInteger
{
public:
	static const int BASE = 100000000;//BigInteger::BASE
	static const int WIDTH = 8;//大数操作的位数
	vector<long long> s;//vector存储大整数

	//~BigInteger();
	BigInteger(long long num = 0) {
		*this = num;
	}
	BigInteger(const string &str) {//用于string->BigInteger隐式转换的构造函数
		*this = str;
	}
	BigInteger operator = (long long);
	BigInteger operator = (const string&);
	BigInteger operator + (const BigInteger&)const;
	BigInteger operator - (const BigInteger&)const;
	BigInteger operator << (size_t)const;
	BigInteger operator * (const BigInteger&)const;
	bool operator < (const BigInteger &b)const {
		if (s.size() != b.s.size()) return s.size() < b.s.size();
		for (int i = s.size() - 1; i >= 0; i--) //从高位比起
			if (s[i] != b.s[i]) return s[i] < b.s[i];
		return false;//相等
	}
	bool operator > (const BigInteger &b) const { return b < *this; }
	bool operator <= (const BigInteger &b) const { return !(b < *this); }
	bool operator >= (const BigInteger &b) const { return !(*this < b); }
	bool operator != (const BigInteger &b) const { return b < *this || *this < b; }
	bool operator == (const BigInteger &b) const { return !(b < *this) && !(*this < b); }
};

//双目运算符重载定义在类外(非成员函数）
extern ostream& operator << (ostream &out, const BigInteger &x);
extern istream& operator >> (istream &in, BigInteger &x);


