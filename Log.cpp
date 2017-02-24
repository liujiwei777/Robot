#include "Log.h"
#include "Config.h"
//Log
Log* Log::instance = NULL;

Log::Log()
{
	//char filename[] = "/home/liujiwei/robot/out.txt"; // 此处写入文件名 
	ofstream *fout = new ofstream(Config::GetInstance()->GetValue("LogPath").c_str(), ios::app);
	m_fout = fout;
}


Log::~Log()
{
	delete m_fout;
}

void Log::Insert(char* pszText)
{
	(*m_fout) << pszText << endl;
}

Log* Log::GetInstance()
{
	if (instance == NULL)
	{
		instance = new Log();
	}

	return instance;
}
