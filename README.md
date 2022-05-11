# muduo_knowledge

  由于之前学习C++的时候，很多东西看过没做过或者做过没看过，要么是通过博客零散的学习的，知识点比较零散，因此需要一次对C++知识梳理和集中实践。这里采用了陈硕编写的muduo网络库的配套书籍中的学习路线。

## 关于学习方法：

对于理论知识的学习，要集中精力速战速决。而旁枝末节和非本质的知识内容，完全可以留给时间去零碎敲打。

**学习步骤：**

1.明确这部分技术要解决的问题
2.分析框架
3.形成整体脉络
4.对于重点单点突破



## C++基础：

-阅读C++Prime。

-自己编写四个类，以熟悉C++基本操作。分别是

1. 写一个复数类或[大整数类](cppBase/BigNumber_Class.md)，实现基本加减乘法运算，熟悉封装与数据抽象。

2. 写一个[字符串类](cppBase/SimpleString.md)，熟悉内存管理与拷贝控制。
3. 写一个[简化的vector<T>类](cppBase/SimpleVector.md)，熟悉基本的模板编程。
4. 写一个表达式计算器，实现节点继承体系，体会面向对象编程。

-熟悉一些C++常用特性。



## Unix网络编程：

#### -网络编程学习的三个步骤：

(详见Muduo P569)

1.读过教程和文档，做过小练习
2.熟悉TCP/IP协议栈特性与脾气 
3.自己写过一个简单的TCP/IP stack

#### -阅读Unix网络编程卷一卷二：



#### 练手TCP网络编程值得研究的三个例子：

-echo：熟悉服务端被动接收新连接，收发数据，被动处理连接断开。
-chat：连接之间的数据有交流，从A收到的数据要发给B。
-proxy：更复杂的的连接管理，既要考虑被动接收连接也要考虑主动发起连接。

#### 工具：

学习socket api的利器： IPython。
tcp调试：tcpdump。

