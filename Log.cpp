#include "Log.h"

Log* Log::instance = NULL;

Log::Log()
{
	char filename[] = "/home/tcpflow/out.txt"; // 此处写入文件名 
	ofstream *fout = new ofstream(filename, ios::app);
	m_fout = fout;
}


Log::~Log()
{
	delete m_fout;
}

void Log::Insert(string & strText)
{
	(*m_fout) << strText << endl;
}

Log* Log::GetInstance()
{
	if (instance == NULL)
	{
		instance = new Log();
	}

	return instance;
}
