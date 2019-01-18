#include<iostream>
using namespace std;
class Date
{
public:
  //Date(int year,int month, int day)
  //{
  //  _year = year;
  //  _month = month;
  //  _day = day;
  //}
  Date(int year,int month,int day)
    :_year(year)
    ,_month(month)
    ,_day(day)
  {}
  void Print()
  {
    cout<<_year<<"-"<<_month<<"-"<<_day<<endl;
  }
  private:
  int _year;
  int _month;
  int _day;
};

int main()
{
  Date d1(2018,8,29);
  d1.Print();
  return 0;
}
