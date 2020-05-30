// g++ -Wall -std=c++11 -I ../.. demo.cpp && ./a.out

#include "SubjectObserver.h"
#include <iostream>

using namespace std;
using namespace yafiyogi;

class Param
{
public:
    Param():
        type( "default"),
        version(0)
    {
    }

    Param( const Param & p):
        type( "copy"),
        version( p.version + 1)
    {
    }

    Param( const Param && p):
        type( "move"),
        version( p.version )
    {
    }

    const std::string type;
    const int version;
};

class obs
{
public:
    void handleInt_v( const int * i)
    {
        if( nullptr != i)
        {
            cout << "handleInt_v " << *i << endl;
        }
    }

    void handleInt_vs( const int * i, const Param & p)
    {
        if( nullptr != i)
        {
            cout << "handleInt_vs " << *i << " " << p.type << " version=" << p.version << endl;
        }
    }

    void handleDouble_v( const double * d)
    {
        if( nullptr != d)
        {
            cout << "handleDouble_v " << *d << endl;
        }
    }

    int handleInt_i( const int * i)
    {
        if( nullptr != i)
        {
            cout << "handleInt_i " << *i << endl;
        }
        return 3;
    }

    int handleDouble_i( const double * d)
    {
        if( nullptr != d)
        {
            cout << "handleDouble_i " << *d << endl;
        }
        return 7;
    }
};

Subject<int, void> m_sub_v;
Subject<int, int> m_sub_i;
Subject<int, void, Param> m_sub_vs;

shared_ptr<obs> o( new obs);

int main()
{
    m_sub_v.add( 1, o, &obs::handleInt_v);
    m_sub_v.add( 2, o, &obs::handleDouble_v);
    m_sub_v.add( 3, [=]( const double *d) { cout << "void [=]( const double *d) "; o->handleDouble_v(d); });
    m_sub_v.add( 4, []( const double *d) { cout << "void []( const double *d) " << *d << endl; });

    m_sub_vs.add( 5, o, &obs::handleInt_vs);

    int i = 3;
    double d = 3.7;
    m_sub_v.event(1, &i);
    m_sub_v.event(2, &d);
    m_sub_v.event(3, &d);
    m_sub_v.event(4, &d);

    Param p;
    m_sub_vs.event(5, &i, p);

    m_sub_i.add( 1, o, &obs::handleInt_i);
    m_sub_i.add( 2, o, &obs::handleDouble_i);
    m_sub_i.add( 3, [=]( const double *d) -> int { cout << "int [=]( const double *d) "; return o->handleDouble_i(d); });

    cout << "\nrv=" << std::get<1>( m_sub_i.event(1, &i));
    cout << "\nrv=" << std::get<1>( m_sub_i.event(2, &d));
    cout << "\nrv=" << std::get<1>( m_sub_i.event(3, &d));
    cout << "\nrv=" << std::get<1>( m_sub_i.event(4, &d));

    return 0;
}
