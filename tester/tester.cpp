// tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	string oko = "k128r34";
	char c;
	int a, b;
	stringstream strim(oko);
	strim >> c;
	strim >> a;
	strim >> c;
	strim >> b;

	cout << oko << endl;
	cout << c << endl;
	cout << a << endl << b;

	cin >> c;



	return 0;
}

