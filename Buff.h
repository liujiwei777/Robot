#include <iostream>

using namespace std;

#define MAX_BUFF_SIZE 2048 

class MyLock
{
	pthread_mutex_t         m_Mutex;
public:
	MyLock() { pthread_mutex_init(&m_Mutex, NULL); };
	~MyLock() { pthread_mutex_destroy(&m_Mutex); };
	void    Lock() { pthread_mutex_lock(&m_Mutex); };
	void    Unlock() { pthread_mutex_unlock(&m_Mutex); };
};

class Buff
{
public:
	Buff();
	~Buff();
	void Clean();
	void Set_Sending(bool bUsed);
	void Get_Sending(bool & bSend);
	void Set_Len(int nLen);
	int  Get_Len();
	void Set_Current(int nCurrent);
	void  Get_Current(int & nCurrent);
	void  Get_Remain(int & nRemain);
	void Get_Current_Buff(char *& pBuff);
	char* Get_Buff();
	void Init_Char(char* pszChar, int nLen);
	void Get_Char(int nIndex, char & var);
	MyLock & Get_My_Lock();
private:
	bool  m_bSending;
	char*  m_pszBuff;
	int   m_nCurrent;
	int   m_nLen;
	MyLock m_thread_lock;
};


