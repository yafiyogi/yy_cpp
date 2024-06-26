// g++ -Wall -std=c++17 -I .. subject_observer.cpp && ./a.out
/*

  MIT License

  Copyright (c) 2019-2022 Yafiyogi

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include <iostream>
#include "subject_observer.h"

using namespace std;
using namespace yafiyogi;

class Param
{
  public:
    Param() :
      type("Param default"),
      version(0)
    {
    }

    Param(const Param & p) :
      type("Param copy"),
      version(p.version + 1)
    {
    }

    Param(const Param && p) noexcept :
      type("Param move"),
      version(p.version)
    {
    }

    const std::string type;
    const int version;
};

class obs
{
  public:
    void handleInt_v(const int * i)
    {
      if(nullptr != i)
      {
        cout << "handleInt_v " << *i << endl;
      }
    }

    void handleInt_vs(const int * i, const Param & p)
    {
      if(nullptr != i)
      {
        cout << "handleInt_vs " << *i << " " << p.type
             << " version=" << p.version << endl;
      }
    }

    void handleDouble_v(const double * d)
    {
      if(nullptr != d)
      {
        cout << "handleDouble_v " << *d << endl;
      }
    }

    int handleInt_i(const int * i)
    {
      if(nullptr != i)
      {
        cout << "handleInt_i " << *i << endl;
      }
      return 3;
    }

    int handleDouble_i(const double * d)
    {
      if(nullptr != d)
      {
        cout << "handleDouble_i " << *d << endl;
      }
      return 7;
    }
};

yy_data::subject<int, void> m_sub_v;
yy_data::subject<int, int> m_sub_i;
yy_data::subject<int, void, const Param &> m_sub_vp;

shared_ptr<obs> o{std::make_shared<obs>()};

void func_do(const double * d, const Param & /* p */)
{
  cout << "func_do( const double *d, const Param & p) " << *d << endl;
}

int main()
{
  m_sub_v.add(1, o, &obs::handleInt_v);
  m_sub_v.add(2, o, &obs::handleDouble_v);
  m_sub_v.add(3, [=](const double * d) {
    cout << "void [=]( const double *d) ";
    o->handleDouble_v(d);
  });
  m_sub_v.add(4, [](const double * d) {
    cout << "void []( const double *d) " << *d << endl;
  });

  m_sub_vp.add(5, [](const double * d, const Param & /* p */) {
     cout << "void []( const double *d, const Param & p) " << *d << endl;
  });

  m_sub_vp.add(6, func_do);


  m_sub_vp.add(7, o, &obs::handleInt_vs);

  int i = 3;
  double d = 3.7;
  Param p;
  const Param p2;

  cout << "Test1\n";
  m_sub_v.event(1, &i);
  cout << "\nTest2\n";
  m_sub_v.event(2, &d);
  cout << "\nTest3\n";
  m_sub_v.event(3, &d);
  cout << "\nTest4\n";
  m_sub_v.event(4, &d);

  cout << "\nTest5a\n";
  m_sub_vp.event(5, &d, p);

  cout << "\nTest5b\n";
  m_sub_vp.event(5, &d, p2);

  cout << "\nTest5c\n";
  m_sub_vp.event(5, &d, Param{});

  cout << "\nTest6a\n";
  m_sub_vp.event(6, &i, p);

  cout << "\nTest6b\n";
  m_sub_vp.event(6, &i, p2);

  cout << "\nTest6c\n";
  m_sub_vp.event(6, &i, Param{});

  m_sub_i.add(1, o, &obs::handleInt_i);
  m_sub_i.add(2, o, &obs::handleDouble_i);
  m_sub_i.add(3, [=](const double * dbl) -> int {
    cout << "int [=]( const double *d) ";
    return o->handleDouble_i(dbl);
  });

  cout << "\nTest9";
  cout << "\nrv=" << std::get<1>(m_sub_i.event(1, &i));
  cout << "\n\nTest10";
  cout << "\nrv=" << std::get<1>(m_sub_i.event(2, &d));
  cout << "\n\nTest11";
  cout << "\nrv=" << std::get<1>(m_sub_i.event(3, &d));
  cout << "\n\nTest12";
  cout << "\nrv=" << std::get<1>(m_sub_i.event(4, &d));
  cout << endl;
  return 0;
}
