#include "utl_debug.h"


void utl::debugLn(int l)
{
    for(int i=0; i<l; i++)
        cout << endl;
}

void utl::debugLn(string s, int l)
{
    debug(s);
    debugLn(l);
}


void utl::debugLn(string s, bool b, int l)
{
    debug(s, b);
    debugLn(l);
}


void utl::debugLn(string s, char c, int l)
{
    debug(s, c);
    debugLn(l);
}
void utl::debugLn(string s, string s2, int l)
{
    debug(s, s2);
    debugLn(l);
}

void utl::debugLn(string s, unsigned int i, int l)
{
    debug(s, i);
    debugLn(l);
}
void utl::debugLn(string s, int i, int l)
{
    debug(s, i);
    debugLn(l);
}

void utl::debugLn(string s, float f, int l)
{
    debug(s, f);
    debugLn(l);
}


void utl::debug(string s)
{
    cout << s << endl;
}


void utl::debug(string s, bool b)
{
    if(b)
        cout << s << " true" << endl;
    else
        cout << s << " false" << endl;
}


void utl::debug(string s, char c)
{
    cout << s << "  "  << c << endl;
}

void utl::debug(string s, string s2)
{
    cout << s << "  "  << s2 << endl;
}

void utl::debug(string s, unsigned int i)
{
    cout << s << "  " << i << endl;
}

void utl::debug(string s, int i)
{
    cout << s << " " << i << endl;
}

void utl::debug(string s, float f)
{
    cout << s << " " << f << endl;
}

void utl::debug(string s, double d)
{
	cout << s << " " << d << endl;
}
