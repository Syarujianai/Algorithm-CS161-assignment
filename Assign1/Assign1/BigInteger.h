#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdio>
//#include <cstring>
#include <string>

using namespace std;

/*
	����p124��������(�Ǹ�)
*/
class BigInteger
{
public:
	static const int BASE = 100000000;//BigInteger::BASE
	static const int WIDTH = 8;//����������λ��
	vector<long long> s;//vector�洢������

	//~BigInteger();
	BigInteger(long long num = 0) {
		*this = num;
	}
	BigInteger(const string &str) {//����string->BigInteger��ʽת���Ĺ��캯��
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
		for (int i = s.size() - 1; i >= 0; i--) //�Ӹ�λ����
			if (s[i] != b.s[i]) return s[i] < b.s[i];
		return false;//���
	}
	bool operator > (const BigInteger &b) const { return b < *this; }
	bool operator <= (const BigInteger &b) const { return !(b < *this); }
	bool operator >= (const BigInteger &b) const { return !(*this < b); }
	bool operator != (const BigInteger &b) const { return b < *this || *this < b; }
	bool operator == (const BigInteger &b) const { return !(b < *this) && !(*this < b); }
};

//˫Ŀ��������ض���������(�ǳ�Ա������
extern ostream& operator << (ostream &out, const BigInteger &x);
extern istream& operator >> (istream &in, BigInteger &x);


