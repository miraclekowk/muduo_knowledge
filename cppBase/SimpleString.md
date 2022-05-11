注意操作符的重载：
-如果是+=这种需要返回自身的，则是返回一个引用如返回String&。
-如果是+这种。返回的是一个新的对象。
-如果+这种是以友元函数实现(在本类型可以和其他类型相加的场景下)，返回一个新对象，且参数是二元参数。
-如果+是类内操作，则自然参数是一元参数，因为另一个参与操作的对象是this。
如：

```c++
String operator+(const String& other); //+返回一个新的值
//或
friend String operator+(const String& str1,const String& str2);
```



char*字符数组可以直接流输出到指定OS。
重载输入输出的时候返回类型要区别istream和ostream，如果用iostream会报‘Non-const lvalue reference to type 'std::iostream’的错误。

代码如下：

```c++
#include <iostream>
#include <cstring>
#include <bits/stdc++.h>

using namespace std;

class String{
private:
    char* data;
    size_t length;

public:
    String(const char* str = NULL); //String("abc")
    String(const String& other); //拷贝构造函数
    String(String&& other); //移动构造函数
    ~String();

    String& operator=(const String& other); //拷贝操作符
    String& operator+=(const String& other); //+=号，返回自身引用
    String operator+(const String& other); //+返回一个新的值
    //friend String operator+(const String& str1,const String& str2);


    bool operator==(const String& other);
    char& operator[](size_t n);


    size_t size() const;
    friend ostream& operator<<(ostream& os,const String& str);
    friend istream& operator>>(istream& os,String& str);

};

String::String(const char *str) {
    if(str == NULL){
        length = 0;
        data = new char [1];
        *data = '\0';
    }else{
        length = strlen(str);
        data = new char[length+1];
        strcpy(data,str); //C风格内存拷贝，str末尾的结束符也会被拷贝过来
    }
}

String::String(const String& other) {
    length = other.size();
    data = new char[length + 1];
    strcpy(data,other.data);
}

String::String(String&& other){
    data = nullptr;
    data = other.data;
    length = other.length;
    other.data = nullptr;
    other.length = 0;
}


String::~String() {
    delete[] data;
}

String& String::operator=(const String &other) {
    if(data != other.data){
        delete[] data;

        length = other.length;
        data = new char[length + 1];
        strcpy(data,other.data);
    }
    return *this;

}

String& String::operator+=(const String &other) {
    length = length + other.length;
    String tmp;
    tmp.data = new char[length + 1];

    strcpy(tmp.data,data);
    strcpy(tmp.data,other.data);
    delete[] data;  //删除旧的
    
    data = tmp.data;
    return *this;

}

String String::operator+(const String &other) {
    String tmp;
    tmp.data = new char[length + 1];
    tmp.length = length + other.length;

    strcpy(tmp.data,data);
    strcpy(tmp.data + length,other.data);
    return tmp;

}

bool String::operator==(const String& other){
    if(length != other.length){
        return false;
    }else{
        return strcmp(data,other.data)? false:true;
    }
}

char& String::operator[](size_t n){
    if(n > length){
        return data[length -1];  //如果越界
    }else{
        return data[n];
    }
}

size_t String::size() const{
    return length;
}


ostream& operator<<(ostream& os,const String& str){
    if(nullptr == str.data){
        os <<" ";
    }else{
        os<<str.data;
    }
    return os;
}

istream& operator>>(istream& os,String& str){
    os>>str.data;
    return os;
}


int main(){
    String str1("abccc");

    String str2 = str1;
    cout<<"str1 is "<<str1<<"str2 is "<<str2<<endl;
    
    String str3;
    str3 = str1 + str2;
    cout<<str3;
    
    return 0;

}
```


