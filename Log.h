#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Log
{
public:
	Log();
	~Log();
public:
	void Insert(char* pszText);
	static Log* GetInstance();
private:
	ofstream *m_fout;
	static Log* instance;
};


