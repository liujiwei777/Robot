#include <iostream>
#include "Buff.h"

using namespace std;

#define MAX_BUFF_COUNT  350000 

class Buff_Manager
{
public:
	Buff_Manager();
	~Buff_Manager();
	Buff* Get_Buff(int nIndex);
	void Clean(int nIndex);
	int  Get_UnUse_Buff();
	int  Write_Empty_Buff(char *pszBuff, int nLen);
	int  Copy_Send_Buff(Buff* buff);
	int Get_Send_Index();
	void Set_Send_Index(int nIndex);
	MyLock & Get_My_Lock();
private:
	Buff m_buff[MAX_BUFF_COUNT];
	int  m_zip_index;
	int  m_send_index;
	MyLock m_thread_lock;
};

