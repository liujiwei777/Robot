#include "Buff_Manager.h"
#include <stdio.h>

Buff_Manager::Buff_Manager()
{
	for (int i = 0; i < MAX_BUFF_COUNT; i++)
	{
		m_buff[i].Clean();
	}
	m_zip_index = 0;
	m_send_index = 0;
}

Buff_Manager::~Buff_Manager()
{

}

void Buff_Manager::Clean(int nIndex)
{
	if (nIndex < 0 || nIndex >= MAX_BUFF_COUNT)
	{
		return;
	}
	m_buff[nIndex].Clean();
}
Buff* Buff_Manager::Get_Buff(int nIndex)
{
	if (nIndex < 0 || nIndex >= MAX_BUFF_COUNT)
	{
		return NULL;
	}

	return &m_buff[nIndex];
}

int Buff_Manager::Get_UnUse_Buff()
{
	for (int i = m_zip_index; i < MAX_BUFF_COUNT; i++)
	{
		bool bSending = false;
		m_buff[i].Get_Sending(bSending);
		if (false == bSending)
		{
			return i;
		}
	}

	return MAX_BUFF_COUNT;
}

int Buff_Manager::Write_Empty_Buff(char *pszBuff, int nWriteLen)
{
	for (int i = m_zip_index; i < MAX_BUFF_COUNT; i++)
	{
		bool bSending = false;
		m_buff[i].Get_My_Lock().Lock();
		int nBuffLen = m_buff[i].Get_Len();
		m_buff[i].Get_Sending(bSending);
		if (0 == nBuffLen && false == bSending)
		{
			m_buff[i].Init_Char(pszBuff, nWriteLen);
			m_buff[i].Get_My_Lock().Unlock();
			return i;
		}
		m_buff[i].Get_My_Lock().Unlock();
	}

	return MAX_BUFF_COUNT;
}

int Buff_Manager::Copy_Send_Buff(Buff* buff)
{
	for (int i = m_send_index; i < MAX_BUFF_COUNT; i++)
	{
		bool bSending = true;
		m_buff[i].Get_My_Lock().Lock();
		m_buff[i].Get_Sending(bSending);
		int nLen = m_buff[i].Get_Len();
		if (nLen > 0 && false == bSending)
		{
			buff->Init_Char(m_buff[i].Get_Buff(), nLen);
			m_buff[i].Clean();
			Set_Send_Index(i);
			m_buff[i].Get_My_Lock().Unlock();
			return i;
		}
		m_buff[i].Get_My_Lock().Unlock();
	}

	buff = NULL;

	return -1;
}

int Buff_Manager::Get_Send_Index()
{
	return m_send_index;
}

void Buff_Manager::Set_Send_Index(int nIndex)
{
	m_send_index = nIndex + 1;
	if (m_send_index > MAX_BUFF_COUNT - 2)
	{
		m_send_index = 0;
	}
}

MyLock & Buff_Manager::Get_My_Lock()
{
	return m_thread_lock;
}

