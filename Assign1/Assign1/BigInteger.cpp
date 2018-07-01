#include "BigInteger.h"
#include <assert.h>
#include <cmath>//abs()

//单目操作符可定义为成员函数(可以使用this指针，左操作数即为*this)
BigInteger BigInteger::operator = (long long num) {
	this->s.clear();
	do {
		this->s.push_back(num%BASE);
		num /= BASE;
	} while (num > 0);
	return *this;
}

BigInteger BigInteger::operator = (const string &str) {
	this->s.clear();
	int x, len = (str.length() - 1) / WIDTH + 1;
	for (int i = 0; i < len; i++) {
		int end = str.length() - i*WIDTH;
		int start = max(0, end - WIDTH);
		sscanf(str.substr(start, end - start).c_str(), "%d", &x);//c_str()：string转char *
		this->s.push_back(x);
	}
	return *this;
}

BigInteger BigInteger::operator + (const BigInteger &b) const{
	BigInteger c;
	c.s.clear();
	for (int i = 0, g = 0;; i++) {
		if (g == 0 && i >= (*this).s.size() && i >= b.s.size()) break;//g为进位，进位为0同时(*this)和b所有位都加完时退出循环
		int x = g;//x+=g
		if (i < (*this).s.size()) x += (*this).s[i];//x+=s[i]
		if (i < b.s.size()) x += b.s[i];//x+=b.s[i]
		c.s.push_back(x%BASE);
		g = x / BASE;
	}
	return c;
}

/*
	ps:一定要大数减小数
*/
BigInteger BigInteger::operator - (const BigInteger &b)const {
	assert((*this)>=b);//测试：大数减小数
	BigInteger c;
	c.s.clear();
	for (int i = 0, borrow = 0;; i++) {
		if (i >= (*this).s.size() && i >= b.s.size()) break;//borrow为借位标志，进位为0同时(*this)和b所有位都加完时退出循环
		int x = borrow;
		if (borrow != 0) borrow = 0;
		if (i < (*this).s.size()) x += (*this).s[i];//x+=s[i]
		if (i < b.s.size()) x -= b.s[i];//x-=b.s[i]
		if ((*this).s[i] < b.s[i] && i < (*this).s.size() && i < b.s.size()) {
			x += BASE;//x+=BASE
			borrow = -1;
		}
		c.s.push_back(x);	
	}
	return c;
}

/*
	重载左移位运算符
*/
BigInteger BigInteger::operator << (size_t len)const {
	BigInteger shifted_num;
	shifted_num.s.clear();
	for (int i = 0; i < len; i++) {
		shifted_num.s.push_back(0);
	}
	for (int i = 0; i < (this->s.size()); i++) {
		shifted_num.s.push_back(this->s[i]);
	}
	return shifted_num;
}

/*
	Karatsuba大数乘法O(nlog3(n))：
	x,y的位数不需要相同，最高支持n=64位大数相乘
*/
BigInteger BigInteger::operator * (const BigInteger &y)const {
	//assert(this->s.size() == y.s.size());//测试：x,y位数要相同
	//情况1: single-digt乘积
	if ((this->s.size()) == 1 && y.s.size() == 1) {
		//或者直接return (x.s[0])*(y.s[0]); 隐式转换为BigInteger调用"=".
		BigInteger product = (this->s[0])*(y.s[0]);
		return product; //single-digt直接return乘积
	}
	//情况2: 长度不同的大数乘积分解
	else if ((this->s.size()) != y.s.size()) {
		BigInteger a, b, d;//定义a为长序列的高位,b为长序列的低位,d为短序列本身
		a.s.clear(); b.s.clear(); d.s.clear();
		size_t len_cut, len_else;//(len_cut,len_else):长序列分解后的长度
		//若x长度大于y，只有x需要分解
		if (this->s.size() > y.s.size()) {
			//push x，y不变
			len_cut = this->s.size() - y.s.size();
			len_else = y.s.size();
			for (int i = 0; i < len_cut; i++)
				b.s.push_back(this->s[i]);
			for (int i = len_cut; i < (this->s.size()); i++)
				a.s.push_back(this->s[i]);
			d = y;
		}else {
			//push y，x不变
			len_cut = y.s.size() - (this->s.size());
			len_else = this->s.size();
			for (int i = 0; i < len_cut; i++)
				b.s.push_back(y.s[i]);
			for (int i = len_cut; i < y.s.size(); i++)
				a.s.push_back(y.s[i]);
			d = (*this);
		}
		return ((a*d)<< len_else) + b*d;
	}
	//情况3: 长度相同的大数乘积分解
	else if ((this->s.size()) == y.s.size()) {
		BigInteger a, b, c, d;
		a.s.clear(); b.s.clear(); c.s.clear(); d.s.clear();//清除第一个元素
		size_t len = this->s.size();
		//x、y长度相同时都需要分解,若size() = 9, i = 0~3.
		for (int i = 0; i < len / 2; i++) {
			b.s.push_back(this->s[i]);//b为x的低位
			d.s.push_back(y.s[i]);//d为y的低位
		}
		//若size() = 9, i = 4~8.
		for (int i = len / 2; i < len; i++) {
			a.s.push_back(this->s[i]);//a为x的高位
			c.s.push_back(y.s[i]);//c为y的高位
		} 
		return ((a*c)<<len) + ((a*d)<<len/2)+((b*c)<<len/2)+b*d;
	}
}

//双目运算符重载定义在类外(非成员函数）
extern ostream& operator << (ostream &out, const BigInteger &x) {
	out << x.s.back();
	for (int i = x.s.size() - 2; i >= 0; i--) {
		char buf[20];
		sprintf(buf, "%08d", x.s[i]);//int->string
		for (int j = 0; j < strlen(buf); j++) out << buf[j];
	}
	return out;
}

istream& operator >> (istream &in, BigInteger &x) {
	string s;
	if (!(in >> s)) return in;
	x = s;
	return in;
}


