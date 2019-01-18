#include<iostream>
using namespace std;
class A
{
  public:
    A(int a)
      :_a(a)
    {}
  private:
    int _a;
};
class B
{
  public:
    B(int a, int ref)
      :_aobj(a)
       ,_ref(ref)
       ,_n(10)
  {}
  private:
    A _aobj;//没有默认构造函数
    int& _ref;//引用
    const int _n;//const
};
