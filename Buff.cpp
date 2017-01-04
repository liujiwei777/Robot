#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Buff.h"

Buff::Buff()
{
	Clean();
}

void Buff::Clean()
{
	m_nCurrent = 0;
	m_nLen = 0;
	m_bSending = false;
}

Buff::~Buff()
{

}

void Buff::Set_Sending(bool bSending)
{
	m_bSending = bSending;
}

void Buff::Get_Sending(bool & bSending)
{
	bSending = m_bSending;
}

void Buff::Set_Len(int nLen)
{
	m_nLen = nLen;
}

int Buff::Get_Len()
{
	return m_nLen;
}

void Buff::Set_Current(int nCurrent)
{
	m_nCurrent = nCurrent;
}

void Buff::Get_Current(int & nCurrent)
{
	nCurrent = m_nCurrent;
}

void Buff::Get_Remain(int & nRemain)
{
	nRemain = m_nLen - m_nCurrent;
}

void Buff::Get_Current_Buff(char*& pszBuff)
{
	pszBuff = m_pszBuff + m_nCurrent;
}

char* Buff::Get_Buff()
{
	return m_pszBuff;
}

void Buff::Init_Char(char* pszChar, int nLen)
{
	for (int i = 0; i < nLen; i++)
	{
		m_pszBuff[i] = pszChar[i];
	}
	m_nLen = nLen;
}

MyLock & Buff::Get_My_Lock()
{
	return m_thread_lock;
}

