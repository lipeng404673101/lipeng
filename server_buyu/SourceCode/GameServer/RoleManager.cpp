#include "Stdafx.h"
#include "RoleManager.h"
#include "FishServer.h"
#include "RoleEx.h"
RoleManager::RoleManager()
{

}
RoleManager::~RoleManager()
{
	Destroy();
}
void RoleManager::Destroy()
{
	if (m_RoleMap.empty())
		return;
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		if (!Iter->second)
			continue;
		Iter->second->SaveAllRoleInfo(false);

		//Iter->second->SaveAllRoleInfo(true);
		Iter->second->SetIsExit(true);//�����������
		delete Iter->second;
	}
	m_RoleMap.clear();
	m_RoleSocketMap.clear();
	m_RoleGameMap.clear();
}
bool RoleManager::OnInit()
{
	return true;
}
CRoleEx* RoleManager::QueryUser(DWORD dwUserID)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter != m_RoleMap.end())
		return Iter->second;
	else
		return NULL;
}
CRoleEx* RoleManager::QueryUserByGameID(DWORD GameID)
{
	//��ѯ��� ����GameID
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleGameMap.find(GameID);
	if (Iter != m_RoleMap.end())
		return Iter->second;
	else
		return NULL;
}
CRoleEx* RoleManager::QuertUserBySocketID(DWORD dwSocketID)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleSocketMap.find(dwSocketID);
	if (Iter != m_RoleSocketMap.end())
		return Iter->second;
	else
		return NULL;
}
void  RoleManager::OnDelUser(DWORD dwUserID, bool IsWriteSaveInfo, bool IsLeaveCenter)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(dwUserID);
	if (Iter != m_RoleMap.end())
	{
		//��һ����ҵ��� ���� �˳�GameServer��ʱ�� ����Ӧ�û�����ҵ�RoleEx���� 
		//RoleEx���� ����ɾ�� ֱ������RoleEx ��������� ����SocketID Ϊ0  �޿ͻ��������� �ڱ����뿪���� ������ �뿪���ӵ� 
		//������Ҫ��Logon����ͨѶ ����ҽ��뵽GameServer��ʱ�� ������Ҫ��Logon�ҵ� ��� �ڶ��gameServer �ϵķֲ���
		if (IsWriteSaveInfo)
		{
			if (Iter->second->SaveAllRoleInfo(true))//��ұ仯
			{
				//��Ҫ���� ���� ��Ҫ�ȴ�
				return;
			}
			else
			{
				ASSERT(false);
			}
		}
		//ֱ��ɾ����� �Ƿ���Ҫ�������������?
		//������ �� �������ʧ�� ������Ҫ�����������
		//���ֱ������ 
		//Iter->second->ChangeRoleIsOnline(false);
		if (IsLeaveCenter)
			Iter->second->SendUserLeaveToCenter();
		g_FishServer.GetTableManager()->OnPlayerLeaveTable(Iter->second->GetUserID());

		m_RoleSocketMap.erase(Iter->second->GetGameSocketID()/*GetGateSocketID*/);
		m_RoleGameMap.erase(Iter->second->GetRoleInfo().GameID);
		delete Iter->second;
		m_RoleMap.erase(Iter);
	}
}
void RoleManager::OnDelUserResult(DBO_Cmd_SaveRoleAllInfo* pResult)
{
	if (!pResult)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(pResult->dwUserID);
	if (Iter != m_RoleMap.end())
	{
		if (Iter->second->IsExit())//������˳�״̬
		{
		//	LogInfoToFile("RoleManager.txt", "OnDelUserResult������˳�::::::: %d,", pResult->dwUserID);
			//g_FishServer.ShowInfoToWin("��ҽ������߱���ط�����ɹ� �����ʽ����");
			//Iter->second->SetIsExit(false);
			//Iter->second->ChangeRoleIsOnline(false);
			Iter->second->SendUserLeaveToCenter();
			g_FishServer.GetTableManager()->OnPlayerLeaveTable(Iter->second->GetUserID());

			m_RoleSocketMap.erase(Iter->second->GetGameSocketID());
			m_RoleGameMap.erase(Iter->second->GetRoleInfo().GameID);
			delete Iter->second;
			m_RoleMap.erase(Iter);
		}
		else
		{
			//��Ҳ�ѯ������ ��������
			//g_FishServer.ShowInfoToWin("��ҽ������߱���ط�����ɹ� �ط�����ʧЧ �������������");
			//LogInfoToFile("RoleManager.txt", "OnDelUserResult��Ҳ�ѯ������::::::::::: %d,", pResult->dwUserID);
			return;
		}
	}
	else
	{
		ASSERT(false);
		return;
	}
}
void RoleManager::OnKickAllUser()
{
	//LogInfoToFile("RoleManager.txt", "OnKickAllUser�������������555544,"/*, pResult->dwUserID*/);
	//ֱ���ߵ����е���� 
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		Iter->second->SendUserLeaveToCenter();
		g_FishServer.GetTableManager()->OnPlayerLeaveTable(Iter->second->GetUserID());
		Iter->second->SaveAllRoleInfo(false);//��������ҵ�����

	//	Iter->second->SaveAllRoleInfo(true);//��������ҵ�����
		Iter->second->SetIsExit(true);//����������� ��������µĴ���
		delete Iter->second;
	}
	m_RoleMap.clear();
	m_RoleSocketMap.clear();
	m_RoleGameMap.clear();
}
//��ұ仯  ��½ �״ε�½��ȡ�����Ϣ��� 
bool RoleManager::CreateRole(tagRoleInfo* pUserInfo,tagRoleServerInfo* pUserServerInfo, DWORD dwSocketID, time_t pTime, bool LogobByGameServer, bool IsRobot)
{
	if (!pUserInfo || !pUserServerInfo)
	{
		ASSERT(false);
		return false;
	}
//	LogInfoToFile("CollideBuYu.txt", "CreateRole��Ҷ����ʼ����Ϻ�{{{{,::::::%d,:::::::���:%d", pUserInfo->dwUserID, pUserInfo->dwGlobeNum);


	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.find(pUserInfo->dwUserID);
	if (Iter != m_RoleMap.end())
	{
		return true;
	}
	else
	{
		CRoleEx * pUser = new CRoleEx();
		if (!pUser)
		{
			ASSERT(false);
			return false;
		}
		//��Ҷ����ʼ����Ϻ� ���Ǵ洢����  ��һ�ε�½���뵽��������  �����Ϣ�͸�����  ����Ǵ����ݿ���µ�
		m_RoleMap.insert(HashMap<DWORD, CRoleEx*>::value_type(pUserInfo->dwUserID, pUser));
		m_RoleGameMap.insert(HashMap<DWORD, CRoleEx*>::value_type(pUserInfo->GameID, pUser));
		if (!IsRobot)//�������������ӵ�SocketMap����
			m_RoleSocketMap.insert(HashMap<DWORD, CRoleEx*>::value_type(dwSocketID/*dwGateSocketID*/, pUser));
		if (!pUser->OnInit(pUserInfo, pUserServerInfo, this, dwSocketID, pTime, LogobByGameServer, IsRobot))
		{
			OnDelUser(pUserInfo->dwUserID,false,false);//����Ҫɾ�����������  ����¼�ʧ�ܾ�ɾ����ҵĻ���
			delete pUser;
			return false;
		}
		return true;
	}
}
void RoleManager::SendSystemMailToUser(DWORD dwUserID, LPTSTR pContext, WORD RewardID)
{
	//����һ��ϵͳ�ʼ������������ȥ
	CC_Cmd_SendSystemMail msg;
	SetMsgInfo(msg,GetMsgType(Main_Mail, CC_SendSystemMail), sizeof(CC_Cmd_SendSystemMail));
	msg.dwDestUserID = dwUserID;
	msg.MailInfo.RewardID = RewardID;
	TCHARCopy(msg.MailInfo.Context, CountArray(msg.MailInfo.Context), pContext, _tcslen(pContext));
	msg.MailInfo.bIsRead = false;
	msg.MailInfo.MailID = 0;
	msg.MailInfo.SendTimeLog = time(NULL);
	msg.MailInfo.SrcFaceID = 0;
	msg.MailInfo.SrcUserID = 0;
	msg.MailInfo.bIsExistsReward = (msg.MailInfo.RewardID != 0 && msg.MailInfo.RewardSum != 0);
	TCHARCopy(msg.MailInfo.SrcNickName, CountArray(msg.MailInfo.SrcNickName), TEXT(""), 0);
	g_FishServer.SendNetCmdToCenter(&msg);
}
void RoleManager::OnUpdate(DWORD dwTimer)
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		Iter->second->GetRoleMonth().OnUpdate(dwTimer);
	}
}
void RoleManager::OnUpdateRoleTask()
{
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		Iter->second->GetRoleTaskManager().OnDayChange();
	}
}
void RoleManager::OnUpdateByMin(bool IsHourChange, bool IsDayChange, bool IsMonthChange, bool IsYearChange)
{
	if (m_RoleMap.empty())
		return;
	SYSTEMTIME pTime;
	GetLocalTime(&pTime);
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		Iter->second->UpdateByMin(IsHourChange, IsDayChange, IsMonthChange, IsYearChange);
		if (pTime.wMinute == 0)//������и��� 
		{
			Iter->second->GetRoleActionManager().UpdateByHour();
		}
	}
}
void RoleManager::ChangeRoleSocket(CRoleEx* pRole, DWORD SocketID)
{
	if (!pRole)
	{
		ASSERT(false);
		return;
	}
	m_RoleSocketMap.erase(pRole->GetGameSocketID());
	if (SocketID != 0)
		m_RoleSocketMap.insert(HashMap<DWORD, CRoleEx*>::value_type(SocketID, pRole));
}
//void RoleManager::SetRoleLeave(CRoleEx* pRole)
//{
//	m_RoleSocketMap.erase(pRole->GetGameSocketID());
//}
//void RoleManager::SetRoleOnLine(CRoleEx* pRole, DWORD SocketID)
//{
//	m_RoleSocketMap.insert(HashMap<DWORD, CRoleEx*>::value_type(SocketID, pRole));
//}
void RoleManager::OnSaveInfoToDB()
{
	if (m_RoleMap.empty())
		return;
	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
	for (; Iter != m_RoleMap.end(); ++Iter)
	{
		Iter->second->OnSaveInfoToDB();
	}
}
//void RoleManager::OnGetRoleAchievementIndex()
//{
//	if (m_RoleMap.empty())
//		return;
//	HashMap<DWORD, CRoleEx*>::iterator Iter = m_RoleMap.begin();
//	for (; Iter != m_RoleMap.end(); ++Iter)
//	{
//		Iter->second->OnGetRoleAchievenmentIndexByDB();
//	}
//}