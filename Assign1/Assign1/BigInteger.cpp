#include "BigInteger.h"
#include <assert.h>
#include <cmath>//abs()

//��Ŀ�������ɶ���Ϊ��Ա����(����ʹ��thisָ�룬���������Ϊ*this)
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
		sscanf(str.substr(start, end - start).c_str(), "%d", &x);//c_str()��stringתchar *
		this->s.push_back(x);
	}
	return *this;
}

BigInteger BigInteger::operator + (const BigInteger &b) const{
	BigInteger c;
	c.s.clear();
	for (int i = 0, g = 0;; i++) {
		if (g == 0 && i >= (*this).s.size() && i >= b.s.size()) break;//gΪ��λ����λΪ0ͬʱ(*this)��b����λ������ʱ�˳�ѭ��
		int x = g;//x+=g
		if (i < (*this).s.size()) x += (*this).s[i];//x+=s[i]
		if (i < b.s.size()) x += b.s[i];//x+=b.s[i]
		c.s.push_back(x%BASE);
		g = x / BASE;
	}
	return c;
}

/*
	ps:һ��Ҫ������С��
*/
BigInteger BigInteger::operator - (const BigInteger &b)const {
	assert((*this)>=b);//���ԣ�������С��
	BigInteger c;
	c.s.clear();
	for (int i = 0, borrow = 0;; i++) {
		if (i >= (*this).s.size() && i >= b.s.size()) break;//borrowΪ��λ��־����λΪ0ͬʱ(*this)��b����λ������ʱ�˳�ѭ��
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
	��������λ�����
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
	Karatsuba�����˷�O(nlog3(n))��
	x,y��λ������Ҫ��ͬ�����֧��n=64λ�������
*/
BigInteger BigInteger::operator * (const BigInteger &y)const {
	//assert(this->s.size() == y.s.size());//���ԣ�x,yλ��Ҫ��ͬ
	//���1: single-digt�˻�
	if ((this->s.size()) == 1 && y.s.size() == 1) {
		//����ֱ��return (x.s[0])*(y.s[0]); ��ʽת��ΪBigInteger����"=".
		BigInteger product = (this->s[0])*(y.s[0]);
		return product; //single-digtֱ��return�˻�
	}
	//���2: ���Ȳ�ͬ�Ĵ����˻��ֽ�
	else if ((this->s.size()) != y.s.size()) {
		BigInteger a, b, d;//����aΪ�����еĸ�λ,bΪ�����еĵ�λ,dΪ�����б���
		a.s.clear(); b.s.clear(); d.s.clear();
		size_t len_cut, len_else;//(len_cut,len_else):�����зֽ��ĳ���
		//��x���ȴ���y��ֻ��x��Ҫ�ֽ�
		if (this->s.size() > y.s.size()) {
			//push x��y����
			len_cut = this->s.size() - y.s.size();
			len_else = y.s.size();
			for (int i = 0; i < len_cut; i++)
				b.s.push_back(this->s[i]);
			for (int i = len_cut; i < (this->s.size()); i++)
				a.s.push_back(this->s[i]);
			d = y;
		}else {
			//push y��x����
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
	//���3: ������ͬ�Ĵ����˻��ֽ�
	else if ((this->s.size()) == y.s.size()) {
		BigInteger a, b, c, d;
		a.s.clear(); b.s.clear(); c.s.clear(); d.s.clear();//�����һ��Ԫ��
		size_t len = this->s.size();
		//x��y������ͬʱ����Ҫ�ֽ�,��size() = 9, i = 0~3.
		for (int i = 0; i < len / 2; i++) {
			b.s.push_back(this->s[i]);//bΪx�ĵ�λ
			d.s.push_back(y.s[i]);//dΪy�ĵ�λ
		}
		//��size() = 9, i = 4~8.
		for (int i = len / 2; i < len; i++) {
			a.s.push_back(this->s[i]);//aΪx�ĸ�λ
			c.s.push_back(y.s[i]);//cΪy�ĸ�λ
		} 
		return ((a*c)<<len) + ((a*d)<<len/2)+((b*c)<<len/2)+b*d;
	}
}

//˫Ŀ��������ض���������(�ǳ�Ա������
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


