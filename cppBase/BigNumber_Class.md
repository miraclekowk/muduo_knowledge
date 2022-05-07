### 大整数类

#### target:

熟悉封装与数据抽象



一个奇怪的Bug，string如果push_back进去int后再赋值会有奇怪的Bug。

调试发现，number为 "\006\b\006\a"。第二位的8和前面的6结合成了ASCII码的b(68)。

```c++
string number = "";
number.push_back(1);
number.push_back(2);
number.push_back(3);
vector<int> vec = {6,8,6,7,0};

number.resize(vec.size()); //扩充bignumber a
for(int s,i = 0,t = 0;i < vec.size();i++){
    s = t+vec[i];
    vec[i] = s % 10;
    t = s / 10;
    number[i] = vec[i];
}
cout<<number;
```

简易大整数类：

```c++
#include <iostream>
#include "sstream"
#include "vector"
#include "algorithm"
#include "string"

#include <bits/stdc++.h>

using namespace std;

class BigNum{
private:
    string number;
public:

    BigNum();
    BigNum(string & str);
    BigNum(long long & longnum);
    BigNum(const char* s);

    friend bool Null(const BigNum &);

    friend ostream& operator<<(ostream& os,BigNum bigNum);

    BigNum& operator=(const BigNum& a);

    friend BigNum& operator*=(BigNum& a,const BigNum& b);//复合赋值运算符
    friend BigNum operator*(BigNum& a,BigNum &b);//关系算术符

};

BigNum::BigNum(){
    number = "";
}

BigNum::BigNum(string &str) {
    number = "";
    for(int i = str.length()-1;i >= 0;i--){
        if(!isdigit(str[i])){
            throw("input item is not a digits");
        }
        number.push_back(str[i]);
    }

}

BigNum::BigNum(long long & longnum){
    do{
    number.push_back(longnum % 10);
    longnum /= 10;
    } while (longnum);
}

BigNum::BigNum(const char *s){
    number = "";
    for (int i = strlen(s)  - 1; i >= 0;i--)
    {
        if(!isdigit(s[i]))
            throw("ERROR");
        number.push_back(s[i]);
    }
}


bool Null(const BigNum & a){
    return a.number.empty();
}


ostream& operator<<(ostream& os,BigNum bigNum){
    os<<bigNum.number<<endl;
}

BigNum& BigNum::operator=(const BigNum &a) {
    number = a.number;
    return *this;
};

BigNum& operator*=(BigNum& a,const BigNum& b){
    if(Null(a) || Null(b)){
        BigNum temp;
        a = temp;
    }
    int n = a.number.length(),m = b.number.length();
    vector<int> vec(m+n);
    for(int i = 0;i < n;i++){
        for(int j = 0;j < m;j++){
            vec[i+j] += (a.number[i] - '0') * (b.number[j] - '0');
        }
    }
    n = vec.size();
    a.number.resize(n); //扩充bignumber a
    for(int s,i = 0,t = 0;i < n;i++){
        s = t+vec[i];
        vec[i] = s % 10;
        t = s / 10;
        a.number[i] = vec[i] + '0';
    }

    //去除在数字前的0  如01476->1476
    for(int i = n-1;i >= 1 && !vec[i];i--){
        a.number.pop_back();
    }
    reverse(a.number.begin(),a.number.end());

    return a;
};

//返回了一个新对象，所以不需要引用号啦
BigNum operator*(BigNum& a,BigNum &b){
    BigNum temp;
    temp = a;
    temp *= b;
    return temp;
}

int main(){
    BigNum a("123");
    BigNum b("12");
    BigNum c;

    c = a * b;
    cout<<c;
    
    return 0;
}
```

#### 需要注意的事：

1.prime中提到，**操作符都要写成友元形式**。因为如<<，我们平时调用cout<<相当于cout.operator<<(xxx)，是cout的调用。如果是类内定义，则左侧操作对象必须是类对象。

2.操作符的返回对象需注意。如+=，*=这种称为复合赋值运算符，返回的是参与运算的某个对象的引用。(a+=1，返回的是a的本身即a的引用)。 + ，*这种称为关系运算符，返回的是一个新的对象。(c = a+b，a+b返回的肯定是一个新的对象呀。**关系运算千万别加上引用号**！)
总结：返回的是一个参与运算的对象本身，则加引用号。返回的是一个新对象或者一个值(如关系运算符和后置自增符)，返回一个对象，不加引用号。

3.prime中提到，**先实现+=,*=这种复合赋值运算符，再用其去实现+ *这种关系运算符**。 

4.string中push进int，会有拼接现象出现。如：

```c++
string.push_back(6);
string.push_back(7);
//这时候后面一位会变成a -->即ascii码的67.
```

用(short)关键字截断即可解决。



参考：标准代码：

https://www.geeksforgeeks.org/bigint-big-integers-in-c-with-example/



```c++
// C++ program to implement
// the above approach
#include <bits/stdc++.h>

using namespace std;

class BigInt{
	string digits;
public:

	//Constructors:
	BigInt(unsigned long long n = 0);
	BigInt(string &);
	BigInt(const char *);
	BigInt(BigInt &);

	//Helper Functions:
	friend void divide_by_2(BigInt &a);
	friend bool Null(const BigInt &);
	friend int Length(const BigInt &);
	int operator[](const int)const;

			/* * * * Operator Overloading * * * */

	//Direct assignment
	BigInt &operator=(const BigInt &);

	//Post/Pre - Incrementation
	BigInt &operator++();
	BigInt operator++(int temp);
	BigInt &operator--();
	BigInt operator--(int temp);

	//Addition and Subtraction
	friend BigInt &operator+=(BigInt &, const BigInt &);
	friend BigInt operator+(const BigInt &, const BigInt &);
	friend BigInt operator-(const BigInt &, const BigInt &);
	friend BigInt &operator-=(BigInt &, const BigInt &);

	//Comparison operators
	friend bool operator==(const BigInt &, const BigInt &);
	friend bool operator!=(const BigInt &, const BigInt &);

	friend bool operator>(const BigInt &, const BigInt &);
	friend bool operator>=(const BigInt &, const BigInt &);
	friend bool operator<(const BigInt &, const BigInt &);
	friend bool operator<=(const BigInt &, const BigInt &);

	//Multiplication and Division
	friend BigInt &operator*=(BigInt &, const BigInt &);
	friend BigInt operator*(const BigInt &, const BigInt &);
	friend BigInt &operator/=(BigInt &, const BigInt &);
	friend BigInt operator/(const BigInt &, const BigInt &);

	//Modulo
	friend BigInt operator%(const BigInt &, const BigInt &);
	friend BigInt &operator%=(BigInt &, const BigInt &);

	//Power Function
	friend BigInt &operator^=(BigInt &,const BigInt &);
	friend BigInt operator^(BigInt &, const BigInt &);
	
	//Square Root Function
	friend BigInt sqrt(BigInt &a);

	//Read and Write
	friend ostream &operator<<(ostream &,const BigInt &);
	friend istream &operator>>(istream &, BigInt &);

	//Others
	friend BigInt NthCatalan(int n);
	friend BigInt NthFibonacci(int n);
	friend BigInt Factorial(int n);
};

BigInt::BigInt(string & s){
	digits = "";
	int n = s.size();
	for (int i = n - 1; i >= 0;i--){
		if(!isdigit(s[i]))
			throw("ERROR");
		digits.push_back(s[i] - '0');
	}
}
BigInt::BigInt(unsigned long long nr){
	do{
		digits.push_back(nr % 10);
		nr /= 10;
	} while (nr);
}
BigInt::BigInt(const char *s){
	digits = "";
	for (int i = strlen(s) - 1; i >= 0;i--)
	{
		if(!isdigit(s[i]))
			throw("ERROR");
		digits.push_back(s[i] - '0');
	}
}
BigInt::BigInt(BigInt & a){
	digits = a.digits;
}

bool Null(const BigInt& a){
	if(a.digits.size() == 1 && a.digits[0] == 0)
		return true;
	return false;
}
int Length(const BigInt & a){
	return a.digits.size();
}
int BigInt::operator[](const int index)const{
	if(digits.size() <= index || index < 0)
		throw("ERROR");
	return digits[index];
}
bool operator==(const BigInt &a, const BigInt &b){
	return a.digits == b.digits;
}
bool operator!=(const BigInt & a,const BigInt &b){
	return !(a == b);
}
bool operator<(const BigInt&a,const BigInt&b){
	int n = Length(a), m = Length(b);
	if(n != m)
		return n < m;
	while(n--)
		if(a.digits[n] != b.digits[n])
			return a.digits[n] < b.digits[n];
	return false;
}
bool operator>(const BigInt&a,const BigInt&b){
	return b < a;
}
bool operator>=(const BigInt&a,const BigInt&b){
	return !(a < b);
}
bool operator<=(const BigInt&a,const BigInt&b){
	return !(a > b);
}

BigInt &BigInt::operator=(const BigInt &a){
	digits = a.digits;
	return *this;
}

BigInt &BigInt::operator++(){
	int i, n = digits.size();
	for (i = 0; i < n && digits[i] == 9;i++)
		digits[i] = 0;
	if(i == n)
		digits.push_back(1);
	else
		digits[i]++;
	return *this;
}
BigInt BigInt::operator++(int temp){
	BigInt aux;
	aux = *this;
	++(*this);
	return aux;
}

BigInt &BigInt::operator--(){
	if(digits[0] == 0 && digits.size() == 1)
		throw("UNDERFLOW");
	int i, n = digits.size();
	for (i = 0; digits[i] == 0 && i < n;i++)
		digits[i] = 9;
	digits[i]--;
	if(n > 1 && digits[n - 1] == 0)
		digits.pop_back();
	return *this;
}
BigInt BigInt::operator--(int temp){
	BigInt aux;
	aux = *this;
	--(*this);
	return aux;
}

BigInt &operator+=(BigInt &a,const BigInt& b){
	int t = 0, s, i;
	int n = Length(a), m = Length(b);
	if(m > n)
		a.digits.append(m - n, 0);
	n = Length(a);
	for (i = 0; i < n;i++){
		if(i < m)
			s = (a.digits[i] + b.digits[i]) + t;
		else
			s = a.digits[i] + t;
		t = s / 10;
		a.digits[i] = (s % 10);
	}
	if(t)
		a.digits.push_back(t);
	return a;
}
BigInt operator+(const BigInt &a, const BigInt &b){
	BigInt temp;
	temp = a;
	temp += b;
	return temp;
}

BigInt &operator-=(BigInt&a,const BigInt &b){
	if(a < b)
		throw("UNDERFLOW");
	int n = Length(a), m = Length(b);
	int i, t = 0, s;
	for (i = 0; i < n;i++){
		if(i < m)
			s = a.digits[i] - b.digits[i]+ t;
		else
			s = a.digits[i]+ t;
		if(s < 0)
			s += 10,
			t = -1;
		else
			t = 0;
		a.digits[i] = s;
	}
	while(n > 1 && a.digits[n - 1] == 0)
		a.digits.pop_back(),
		n--;
	return a;
}
BigInt operator-(const BigInt& a,const BigInt&b){
	BigInt temp;
	temp = a;
	temp -= b;
	return temp;
}

BigInt &operator*=(BigInt &a, const BigInt &b)
{
	if(Null(a) || Null(b)){
		a = BigInt();
		return a;
	}
	int n = a.digits.size(), m = b.digits.size();
	vector<int> v(n + m, 0);
	for (int i = 0; i < n;i++)
		for (int j = 0; j < m;j++){
			v[i + j] += (a.digits[i] ) * (b.digits[j]);
		}
	n += m;
	a.digits.resize(v.size());
	for (int s, i = 0, t = 0; i < n; i++)
	{
		s = t + v[i];
		v[i] = s % 10;
		t = s / 10;
		a.digits[i] = v[i] ;
	}
	for (int i = n - 1; i >= 1 && !v[i];i--)
			a.digits.pop_back();
	return a;
}
BigInt operator*(const BigInt&a,const BigInt&b){
	BigInt temp;
	temp = a;
	temp *= b;
	return temp;
}

BigInt &operator/=(BigInt& a,const BigInt &b){
	if(Null(b))
		throw("Arithmetic Error: Division By 0");
	if(a < b){
		a = BigInt();
		return a;
	}
	if(a == b){
		a = BigInt(1);
		return a;
	}
	int i, lgcat = 0, cc;
	int n = Length(a), m = Length(b);
	vector<int> cat(n, 0);
	BigInt t;
	for (i = n - 1; t * 10 + a.digits[i] < b;i--){
		t *= 10;
		t += a.digits[i] ;
	}
	for (; i >= 0; i--){
		t = t * 10 + a.digits[i];
		for (cc = 9; cc * b > t;cc--);
		t -= cc * b;
		cat[lgcat++] = cc;
	}
	a.digits.resize(cat.size());
	for (i = 0; i < lgcat;i++)
		a.digits[i] = cat[lgcat - i - 1];
	a.digits.resize(lgcat);
	return a;
}
BigInt operator/(const BigInt &a,const BigInt &b){
	BigInt temp;
	temp = a;
	temp /= b;
	return temp;
}

BigInt &operator%=(BigInt& a,const BigInt &b){
	if(Null(b))
		throw("Arithmetic Error: Division By 0");
	if(a < b){
		a = BigInt();
		return a;
	}
	if(a == b){
		a = BigInt(1);
		return a;
	}
	int i, lgcat = 0, cc;
	int n = Length(a), m = Length(b);
	vector<int> cat(n, 0);
	BigInt t;
	for (i = n - 1; t * 10 + a.digits[i] < b;i--){
		t *= 10;
		t += a.digits[i];
	}
	for (; i >= 0; i--){
		t = t * 10 + a.digits[i];
		for (cc = 9; cc * b > t;cc--);
		t -= cc * b;
		cat[lgcat++] = cc;
	}
	a = t;
	return a;
}
BigInt operator%(const BigInt &a,BigInt &b){
	BigInt temp;
	temp = a;
	temp %= b;
	return temp;
}

BigInt &operator^=(BigInt & a,const BigInt & b){
	BigInt Exponent, Base(a);
	Exponent = b;
	a = 1;
	while(!Null(Exponent)){
		if(Exponent[0] & 1)
			a *= Base;
		Base *= Base;
		divide_by_2(Exponent);
	}
	return a;
}
BigInt operator^(BigInt & a,BigInt & b){
	BigInt temp(a);
	temp ^= b;
	return temp;
}

void divide_by_2(BigInt & a){
	int add = 0;
	for (int i = a.digits.size() - 1; i >= 0;i--){
		int digit = (a.digits[i] >> 1) + add;
		add = ((a.digits[i] & 1) * 5);
		a.digits[i] = digit;
	}
	while(a.digits.size() > 1 && !a.digits.back())
		a.digits.pop_back();
}

BigInt sqrt(BigInt & a){
	BigInt left(1), right(a), v(1), mid, prod;
	divide_by_2(right);
	while(left <= right){
		mid += left;
		mid += right;
		divide_by_2(mid);
		prod = (mid * mid);
		if(prod <= a){
			v = mid;
			++mid;
			left = mid;
		}
		else{
			--mid;
			right = mid;
		}
		mid = BigInt();
	}
	return v;
}

BigInt NthCatalan(int n){
	BigInt a(1),b;
	for (int i = 2; i <= n;i++)
		a *= i;
	b = a;
	for (int i = n + 1; i <= 2 * n;i++)
		b *= i;
	a *= a;
	a *= (n + 1);
	b /= a;
	return b;
}

BigInt NthFibonacci(int n){
	BigInt a(1), b(1), c;
	if(!n)
		return c;
	n--;
	while(n--){
		c = a + b;
		b = a;
		a = c;
	}
	return b;
}

BigInt Factorial(int n){
	BigInt f(1);
	for (int i = 2; i <= n;i++)
		f *= i;
	return f;
}

istream &operator>>(istream &in,BigInt&a){
	string s;
	in >> s;
	int n = s.size();
	for (int i = n - 1; i >= 0;i--){
		if(!isdigit(s[i]))
			throw("INVALID NUMBER");
		a.digits[n - i - 1] = s[i];
	}
	return in;
}

ostream &operator<<(ostream &out,const BigInt &a){
	for (int i = a.digits.size() - 1; i >= 0;i--)
		cout << (short)a.digits[i];
	return cout;
}
```

