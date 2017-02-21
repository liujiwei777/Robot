#ifndef _GET_CONFIG_H_
#define _GET_CONFIG_H_

#include <string>
#include <map>
using namespace std;

#define COMMENT_CHAR '#'

class Config
{
public:
	Config();
	~Config();
	bool ReadConfig(const string & filename);
	void PrintConfig(const map<string, string> & m);
	bool AnalyseLine(const string & line, string & key, string & value);
	void Trim(string & str);
	bool IsCommentChar(char c);
	bool IsSpace(char c);
	string & GetValue(const string & key);
	static Config* GetInstance();
private:
	map<string, string> m_config;
	static Config* instance;
};

#endif


