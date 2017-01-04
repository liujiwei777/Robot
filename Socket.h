#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>


class Buff_Manager;


class Socket
{
public:
	Socket();
	~Socket();
	void Create(int id);
	bool IsSending();
	void Connect(char* pszIP, int nPort);
	int Send();
	int Recv();
	int* Get_Socket();
	void Set_Buff_Manager(Buff_Manager* buff_manager);
	bool Select();
	void WaitPacket();
	bool  ProcessExcept();
	bool  ProcessInput();
	bool  ProcessOutput();
private:
	int  m_id;
	int* m_pSocket;
	char* m_buff;
	Buff_Manager* m_buff_manager;
	int  m_send_buff_index;
	int  m_recv_buff_index;
	//可读句柄
	fd_set					m_ReadFD;
	//可写句柄
	fd_set					m_WriteFD;
	//异常句柄
	fd_set					m_ExceptFD;
};

