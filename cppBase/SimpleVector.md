target:熟悉基本的模板编程和内存空间控制。

#### 仿造STL的vector写的简化版：

```c++
#include <iostream>

#include <iostream>
#include <memory>

#include <bits/stdc++.h>


using namespace std;


template<typename T>
class my_vector{
public:
    typedef T vector_type;
    typedef vector_type* iterator;
    typedef size_t size_type;
    typedef vector_type& refer;

public:
    my_vector():start(nullptr),finish(nullptr),end_of_storage(nullptr){};
    my_vector(size_type n){
        start = alloc.allocate(n);
        end_of_storage = finish = start + n;
        //填充对象 调用模板类的构造函数
        uninitialized_fill_n(start,n,vector_type());
    }
    my_vector(size_type n,vector_type val){
        start = alloc.allocate(n);
        end_of_storage = finish = start + n;
        uninitialized_fill_n(start,n,val);
    }

    ~my_vector(){
        //顺序析构内部对象
        for(iterator i = start;i != finish;i++){
            alloc.destroy(i);
        }
        if(start != NULL){
            //释放全部申请过的空间
            alloc.deallocate(start,end_of_storage - start); //finish会变，end_of_storage是真实容积
        }
    }
    
    iterator begin(){
        return start;
    }
    iterator end(){
        return finish;
    }
    size_type size(){
        return end() - begin();
    }
    size_type capacity(){
        return end_of_storage - begin();
    }
    bool empty(){
        return begin() == end();
    }
    refer front(){
        return *(begin());
    }
    refer back(){
        return *(end() -1);
    }
    refer operator[](const size_type n){
        return *(begin()+n);
    }
    
    void push_back(vector_type val){
        if(finish == end_of_storage){
            reallocate();
        }
        alloc.construct(finish++,val); //construct()用法参见prime P428
    }
    
    void pop_back(vector_type& val){
        --finish;
        alloc.destroy(finish); //析构对象但不释放空间
    }
    //copy(): Copies the elements in the range [first,last) into the range beginning at result.
    iterator erase(iterator position){
        if(position+1 != finish){
            copy(position+1,finish,position); //利用内存拷贝来覆盖
        }
        alloc.destroy(--finish);
        return position;
    }
    
    iterator erase(iterator first_pos,iterator last_pos){
        if(first_pos < start || last_pos > finish){
            throw ("Invalid iterator");
        }
        copy(last_pos,finish,first_pos);
        int len = last_pos - first_pos;
        while(len--){
            --finish;
            alloc.destroy(finish);
        }
        return first_pos;
    }
    
    void clear(){
        erase(start,finish);
    }
    
    void insert(vector_type val,iterator position){
        if(finish == end_of_storage){
            reallocate();
        }
    
        size_type before_size = position - start,after_size = finish - position;
        alloc.construct(finish,*(finish -1)); //把最后一个元素构造出来
        //Copies the elements in the range [first,last) starting from the end into the range terminating at result.
        copy_backward(position,finish-1,finish); //从后向前复制
        *position = val;
    }
    
    void show(){
        for(auto it = begin();it != end();it++){
            cout<<" "<<*it;
        }
        cout<<endl;
    }

private:
    iterator start;
    iterator finish; //当前容量
    iterator end_of_storage; //最大容积
    ///为什么不能是静态的？
    std::allocator<vector_type> alloc; //动态空间分配器

    void reallocate();

};

template <typename T>
void my_vector<T>::reallocate() {
    size_type oldsize = size();
    size_type newsize = oldsize * 2 + 1;

    iterator newstart = alloc.allocate(newsize);
    uninitialized_copy(start,finish,newstart);
    
    for(iterator it = start;it != finish;it++){
        alloc.destroy(it);
    }
    if(start != NULL){
        //释放全部申请过的空间
        alloc.deallocate(start,end_of_storage - start); //finish会变，end_of_storage是真实容积
    }
    
    start = newstart;
    finish = newstart + oldsize;
    end_of_storage = newstart + newsize;

}


int main(){

    my_vector<int> vec(5,10);
    vec.show();
    
    vec.push_back(555);
    vec.push_back(6666);
    vec.show();
    
    auto it = vec.begin();
    it += 5;
    vec.erase(it);
    vec.show();
    
    auto itt = vec.begin();
    itt += 2;
    vec.insert(777,itt);
    vec.show();
    
    vec.clear();
    vec.show();
    
    return 0;

}



```

#### 使用到的API：

申请、释放动态内存：

```c++
#include<memory>
allocator<T> a; //创建一个allocator对象
a.allocate(n); //分配n个类型为T的内存，但不构造

a.construct(p,args); //p是一个T*的指针即iterator,args可以是T的构造函数或T对象

a.deallocate(p,n); //从p开始释放空间，一共释放n个T那么多
a.destory(p); //调用p指向对象的析构


操作动态内存上的对象：
uninitialized_copy_n(b,n,b2);  //从迭代器b开始，赋值n个元素到b2开始的内存中
uninitialized_fill_n(b,n,t); //从b指向对象构建n个对象，对象值均为t

//Copies the elements in the range [first,last) into the range beginning at result.
copy(first,last,target);


```

#### uninitialized_copy和copy区别：

copy需要作用在已经构造过的空间上，是纯拷贝。uninitialized_copy相当于是构造+拷贝。



#### 补充--阅读书籍C++标准库：

**-压缩vector容积用swap原理。两个vector交换内容后，两者的容积也会交换。**

```c++
void shrink(vector<int> &v){
    std::vector<int> tmp(v); //创建临时tmp,拷贝v的内容，拥有较小容积
    v.swap(tmp); //v和tmp整体交换，内容没变，容积变小了
} //出函数，tmp脱离作用域自然死亡
```

**-想要删除vector中全部值为val的元素 -- 用remove。**用法：

```c++
vec.erase(remove(vec.begin(),vec.end(),val) , vec.end() );
```

remove会把vec中val的值全部用后值整体移动覆盖，并返回一个新的终点。这样新终点到end()之间的值都是废弃的空间了。

-**vector<bool> 被STL专门单独实现过**，用Bit压缩了空间，但iterator不是真正的随机访问迭代器了，因此可能template不适用于vector<bool>。



遇到问题：
为啥子构造器不能是静态的？报错：undefined reference to `my_vector<int>::alloc'
static std::allocator<vector_type> alloc;