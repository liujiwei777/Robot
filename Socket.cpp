#include <assert.h>
#include <map>
#include <iostream>
#include <fstream>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <dirent.h>
#include <sys/stat.h>
#include "Buff_Manager.h"
#include "Config.h"
#include "Socket.h"
#include "Log.h"



using namespace std;

string strConnectCount = Config::GetInstance()->GetValue("ConnectCount");
int MAX_CONNECT_COUNT = std::atoi(strConnectCount.c_str());

//#define MAX_CONNECT_COUNT 256

static int zip_count = 0;
static int send_count = 0;
static int g_zip_read_ok = 0;

void *WaitPacket(void *vargp);
void *threadrecv(void *vargp);
void *threadzipreader(void *vargp);

void Revere_String(char* pszString, int nCount);
Buff_Manager* Create_Buff_Manager();
Buff_Manager* g_buff_manaer;
void List_Dir(char *path);

typedef map<int, Socket*> SOCKET_MAP;
SOCKET_MAP socket_map;


#pragma pack(push, 1)
struct ZipLocalFileHeader
{
	unsigned int m_u32Sig; // local file header signature     4 bytes(0x04034b50)
	unsigned short m_u16Ver; // version needed to extract       2 bytes
	unsigned short m_u16Flags; // general purpose bit flag        2 bytes
	unsigned short m_u16Method; // compression method              2 bytes
	unsigned short m_u16Time; // last mod file time              2 bytes
	unsigned short m_u16Date; // last mod file date              2 bytes
	unsigned int m_u32Crc; // crc - 32                          4 bytes
	unsigned int m_u32ComprSize; // compressed size                 4 bytes
	unsigned int m_u32UncomprSize; // uncompressed size               4 bytes
	unsigned short m_u16FileNameLen; // file name length                2 bytes
	unsigned short m_u16ExtraFieldLen; // extra field length              2 bytes
};

struct IP_PORT
{
	char IP[16];
	int port;
};

Socket::Socket()
{
	m_id = -1;
	m_pSocket = 0;
	m_buff = NULL;
	m_buff_manager = NULL;
	m_send_buff_index = -1;
}


Socket::~Socket()
{
}

void Socket::Create(int id)
{
	m_id = id;

	m_pSocket = (int *)malloc(sizeof(int));

	*m_pSocket = socket(AF_INET, SOCK_STREAM, 0);
}

void Socket::Connect(char* pszIP, int nPort)
{
	struct sockaddr_in serveraddr;
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(nPort);
	serveraddr.sin_addr.s_addr = inet_addr(pszIP);
	int nReturnValue = 0;
	if (nReturnValue = connect(*m_pSocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		printf("connect to %s faild, error id is %d\n", pszIP, nReturnValue);
		exit(1);
	}
	//      printf("connect success ip=%s port=%d, m_pSocket=%d\n", pszIP, nPort, *m_pSocket);
}

bool Socket::IsSending()
{
	return m_send_buff_index > -1 ? true : false;
}


int Socket::Send()
{
	// Copy Buff
	Buff* buff = new Buff;
	int nIndex = m_buff_manager->Copy_Send_Buff(buff);
	if (-1 == nIndex)
	{
		if (1 == g_zip_read_ok)
		{
			time_t timep;
			time(&timep);
			printf("end time is %s\n", ctime(&timep));
			g_zip_read_ok = 0;
		}

		delete buff;

		return 0;
	}


	// Send Buff
	if (NULL != buff)
	{
		int nLen = buff->Get_Len();
		char* pszBuff = buff->Get_Buff();
		int nSendCount = 0;
		while (nSendCount < nLen)
		{
			int nSend = send(*m_pSocket, pszBuff + nSendCount, nLen - nSendCount, 0);
			if (-1 == nSend)
			{
				printf("Send strerror:%s\n", strerror(errno));
				m_send_buff_index = -1;
				buff->Clean();
				delete buff;
				return 0;
			}
			nSendCount += nSend;
		}
		delete buff;
		cout << "Send Count = " << nSendCount << endl;
		return nSendCount;
	}

	return 0;
}

int Socket::Recv()
{
	char buff[2048 * 10] = "\0";
	string strBuff;
	int nRecv = recv(*m_pSocket, const_cast<char*>(strBuff.c_str()), 2048 * 10, 0);
	if (nRecv <= 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
	{
		printf("Recv strerror:%s\n", strerror(errno));
		return 0;
	}
	if (nRecv > 0)
	   printf("%s\n", buff);
	Log::GetInstance()->Insert(strBuff);
	return 0;
}

int* Socket::Get_Socket()
{
	return m_pSocket;
}

void Socket::Set_Buff_Manager(Buff_Manager* buff_manager)
{
	m_buff_manager = buff_manager;
}

bool Socket::Select()
{
	FD_ZERO(&m_ReadFD);
	FD_ZERO(&m_WriteFD);
	FD_ZERO(&m_ExceptFD);

	FD_SET(*m_pSocket, &m_ReadFD);
	FD_SET(*m_pSocket, &m_WriteFD);
	FD_SET(*m_pSocket, &m_ExceptFD);

	timeval Timeout;
	Timeout.tv_sec = 0;
	Timeout.tv_usec = 0;

	if (-1 == select(*m_pSocket + 1, &m_ReadFD, &m_WriteFD, &m_ExceptFD, &Timeout))
	{
		printf("select -1 !!!!!!\n");
		return false;
	}
	return true;
}

void Socket::WaitPacket()
{
	if (!Select() || !ProcessExcept() || !ProcessInput() || !ProcessOutput())
	{
		printf(" Socket::WaitPacketerror !!!! \n");
	}
}

bool Socket::ProcessExcept()
{
	if (FD_ISSET(*m_pSocket, &m_ExceptFD))
	{
		printf("socket exrcept !!!!!!!\n");
		return false;
	}
	return true;
}

bool Socket::ProcessInput()
{
	if (FD_ISSET(*m_pSocket, &m_ReadFD))
	{
		Recv();
	}
	return true;
}

bool Socket::ProcessOutput()
{
	if (FD_ISSET(*m_pSocket, &m_WriteFD))
	{
		Send();
	}
	return true;
}

int main()
{
	Socket sock[MAX_CONNECT_COUNT];

	// Create Buff
	g_buff_manaer = Create_Buff_Manager();
	// Create and Init Configure
	ifstream ifs("ip.txt");
	string str;
	int i = 0;
	IP_PORT ip_port[MAX_CONNECT_COUNT];
	memset(&ip_port, 0, sizeof(IP_PORT)*MAX_CONNECT_COUNT);
	while (getline(ifs, str))
	{
		char* pszTemp = const_cast<char*>(str.c_str());
		strcpy(ip_port[i].IP, strtok(pszTemp, ","));
		pszTemp = strtok(NULL, ",");
		ip_port[i].port = atoi(pszTemp);
		//printf("IP=%s Port=%d\n", ip_port[i].IP, ip_port[i].port);
		++i;
	}

	printf("ip connect count is %d \n", i);
	// start zip reader
	pthread_t zipreader;
	pthread_create(&zipreader, NULL, threadzipreader, NULL);
	pthread_detach(zipreader);
	sleep(2);

	// Create and Init socket
	for (int i = 0; i < MAX_CONNECT_COUNT; i++)
	{
		sock[i].Create(i);
		sock[i].Connect(ip_port[i].IP, ip_port[i].port);
		sock[i].Set_Buff_Manager(g_buff_manaer);
		socket_map.insert(SOCKET_MAP::value_type(*sock[i].Get_Socket(), &sock[i]));
	}

	// record start time
	time_t timep;
	time(&timep);
	printf("start time is %s\n", ctime(&timep));

	// start thread
	pthread_t tid1[MAX_CONNECT_COUNT];
	for (int i = 0; i < MAX_CONNECT_COUNT; i++)
	{
		int err = 0;
		int* pSocket = sock[i].Get_Socket();
		err = pthread_create(&tid1[i], NULL, WaitPacket, pSocket);
		if (err != 0)
		{
			printf("number %d thread create failed err is %d\n", i, err);
		}
		err = pthread_detach(tid1[i]);
		if (err != 0)
		{
			printf("number %d thread detach failed err is %d\n", i, err);
		}
	}

	sleep(360000);

	printf(" Every thing is OK!!!\n");

	return 0;
}

void *WaitPacket(void * vargp)
{
	int connfd = *((int *)vargp);
	SOCKET_MAP::iterator itFind = socket_map.find(connfd);
	if (itFind != socket_map.end())
	{
		while (1)
		{
			itFind->second->WaitPacket();
		}
	}
	else
	{
		printf("have no this thread, thread is %d\n", connfd);
	}
	return NULL;
}

void *threadrecv(void *vargp)
{
	int connfd = *((int *)vargp);
	SOCKET_MAP::iterator itFind = socket_map.find(connfd);
	if (itFind != socket_map.end())
	{
		itFind->second->Recv();
	}
	return NULL;
}

void *threadzipreader(void *vargp)
{
	// init file
	//FILE* fp;
	//string strPath = Config::GetInstance()->GetValue("FilePath");
	//fp = fopen(strPath.c_str(), "r");
	//if (NULL == fp)
	//{
	//      printf("Zip File Not Exist!!!\n");
	//      exit(1);
	//}

	//// read file
	//ZipLocalFileHeader hdr;

	//for (;;)
	//{
	//      // read file
	//      fread(&hdr, sizeof(hdr), 1, fp);
	//      if (hdr.m_u32Sig != 0x04034b50)
	//      {// 0x04034b50 is the end
	//              cout << "Zip Have Total UnZip!!!" << endl;
	//              g_zip_read_ok = 1;
	//              break;
	//      }
	//      int buff_len = hdr.m_u32ComprSize + hdr.m_u16FileNameLen + hdr.m_u16ExtraFieldLen;
	//      char *send_buff = new char[buff_len + 1];
	//      fread(send_buff, hdr.m_u32ComprSize + hdr.m_u16FileNameLen + hdr.m_u16ExtraFieldLen, 1, fp);
	//      char *extraField = send_buff + hdr.m_u16FileNameLen + 4;
	//      char *httpBody = send_buff + hdr.m_u16FileNameLen + hdr.m_u16ExtraFieldLen;
	//      Revere_String(httpBody, hdr.m_u32ComprSize);
	//
	//      // write to buff
	//      int nIndex = g_buff_manaer->Write_Empty_Buff(extraField, hdr.m_u16ExtraFieldLen + hdr.m_u32ComprSize - 4);
	//      if (nIndex == MAX_BUFF_COUNT)
	//      {// have buff to send message
	//              delete[] send_buff;
	//              sleep(1);
	//              printf(" BUFF IS FULL !!!!!!!!\n");
	//      }
	//      delete[] send_buff;
	//}

	//fclose(fp);

	string strPath = Config::GetInstance()->GetValue("FilePath");
	char* pszPaht = const_cast<char*>(strPath.c_str());
	while (true)
	{
		List_Dir(pszPaht);
	}
}

void Revere_String(char* pszString, int nCount)
{
	for (size_t i = 0; i < nCount; i++)
	{
		pszString[i] = ~pszString[i];
	}
}

Buff_Manager* Create_Buff_Manager()
{
	Buff_Manager *buff = new Buff_Manager;
	return buff;
}

void List_Dir(char *path)
{
	DIR              *pDir;
	struct dirent    *ent;
	int               i = 0;
	char              childpath[512];

	pDir = opendir(path);
	memset(childpath, 0, sizeof(childpath));

	while ((ent = readdir(pDir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue;

		if (ent->d_type & DT_DIR)
		{
			sprintf(childpath, "%s/%s", path, ent->d_name);

			List_Dir(childpath);
		}
		else
		{
			//cout << ent->d_name << endl;
			std::string strPath(path);
			strPath += "/";
			strPath.append(ent->d_name);
			std::ifstream t(strPath.c_str());
			std::string str1((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
			if (0 == str1.size())
			{
				cout << "This is an Empty File !!!" << endl;
				exit(0);
			}
			char* pszTemp = const_cast<char*>(str1.c_str());
			g_buff_manaer->Write_Empty_Buff(pszTemp, str1.size());
		}
	}

}

#pragma pack(pop)