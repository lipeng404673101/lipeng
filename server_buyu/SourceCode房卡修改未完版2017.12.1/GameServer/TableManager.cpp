#include "StdAfx.h"
#include "Role.h"
#include "TableManager.h"
#include "FishLogic\FishScene\FishResManager.h"
#include "FishServer.h"
#include<atlconv.h>
#include <mmsystem.h>

#include "..\CommonFile\FishServerConfig.h"
#include<algorithm>

#pragma comment(lib, "winmm.lib")
#include "..\CommonFile\DBLogManager.h"

extern void SendLogDB(NetCmd* pCmd);

static volatile long	g_UpdateCount = 0;
static volatile bool	g_bRun = true;
struct TableUpdateParam
{
	GameTable* pTable;
	bool m_bUpdateTime;
	bool m_bUpdateRp;
	bool m_bUpdateRpEffect;
};
SafeList<TableUpdateParam*> g_SafeList[THREAD_NUM];
UINT WINAPI TableThreadUpdate(void *p)
{
	int idx = (int)p;
	SafeList<TableUpdateParam*> &safeList = g_SafeList[idx];
	while (g_bRun)
	{
		TableUpdateParam* pDesk = safeList.GetItem();
		while (pDesk)
		{
			//根据参数更新桌子
			if (!pDesk)
				continue;
			if (!pDesk->pTable)
			{
				free(pDesk);
				continue;
			}
			pDesk->pTable->Update(pDesk->m_bUpdateTime, pDesk->m_bUpdateRp, pDesk->m_bUpdateRpEffect);
			::InterlockedIncrement(&g_UpdateCount);
			free(pDesk);

			pDesk = safeList.GetItem();
		}
		Sleep(1);
	}
	return 0;
}



//全局桌子管理器
TableManager::TableManager() //:m_GameConfig(this)
{
	m_pGameConfig = new CConfig(this);
	m_MaxTableID = 0;

	m_TableTypeSum.clear();
	m_TableGlobeMap.clear();

	RNumrateF = 0;
	RNumrateS = 0;
	RNumrateT = 0;
	RNumrateR = 0;
	
	//KucunNum[]
//	memset(KucunNum, 0, sizeof(KucunNum));
	ZeroMemory(KucunNum, sizeof(KucunNum));

	//初始化私有桌子ID池
	for (unsigned short i = 0; i < 65535; ++i)
		m_privateTableIDPool.push_back(i);



}
TableManager::~TableManager()
{
	Destroy();
}
void TableManager::OnInit()
{
	//读取全局的配置文件
	FishCallback::SetFishSetting(m_pGameConfig);
	if (!FishResManager::Inst() || !FishResManager::Inst()->Init(L"fishdata"))
	{
		//AfxMessageBox(TEXT("读取fishdata失败，请检查"));
		return;
	}

	{
		//USES_CONVERSION;
		//if (!m_GameConfig.LoadConfig(W2A(L"fishdata")))
		//{
		//	//AfxMessageBox(TEXT("读取game config失败"));
		//	return;
		//}
		//m_TimerRanomCatch.StartTimer(m_pGameConfig->RandomCatchCycle()*MINUTE2SECOND, REPEATTIMER);//?
		m_TimerRanomCatch.StartTimer(m_pGameConfig->RandomCatchCycle(), REPEATTIMER);//?
		m_TimerRp.StartTimer(m_pGameConfig->RpCycle(), REPEATTIMER);
		m_TimerGameTime.StartTimer(GAME_TIME_SPACE, REPEATTIMER);
	}
	
	for (int i = 0; i < THREAD_NUM; ++i)//开启线程
	{
		::_beginthreadex(0, 0, (TableThreadUpdate), (void*)i, 0, 0);
	}
	m_LastUpdate = timeGetTime();
}
//删除桌子   房卡需要
void TableManager::Destroy()
{
	//当销毁的时候 
	//1.将所有桌子对象全部销毁掉 
	g_bRun = false;
	if (!m_TableVec.empty())
	{
		std::vector<GameTable*>::iterator Iter = m_TableVec.begin();
		for (; Iter != m_TableVec.end(); ++Iter)
		{
			if (!(*Iter))
				continue;
			//CTraceService::TraceString(TEXT("一个桌子对象 已经被销毁"), TraceLevel_Normal);
			delete *Iter;
		}
		m_TableVec.clear();
	}
	//2.清理全部的配置文件
	if (FishResManager::Inst())
		FishResManager::Inst()->Shutdown();
}
void TableManager::OnStopService()
{
	//当服务器停止的时候
	Destroy();//桌子管理器停止服务器的时候 不是关闭进程
}
void TableManager::Update(DWORD dwTimeStep)
{
	if (m_TableVec.empty())
		return;
	float TimeStep = (dwTimeStep - m_LastUpdate) * 0.001f;
	m_LastUpdate = dwTimeStep;

	if (m_TimerRanomCatch.Update(TimeStep))
	{

		//m_pGameConfig->RandomdFishByTime();
	}

	bool m_bUpdateTime = m_TimerGameTime.Update(TimeStep);
	bool m_bUpdateRp = m_TimerRp.Update(TimeStep);
	bool m_bUpdateRpEffect = m_TimerRpAdd.Update(TimeStep);// zhi hou
	if (m_bUpdateRp)
	{
		m_TimerRpAdd.StartTimer(m_pGameConfig->RpEffectCycle(), 1);
	}
	//对桌子数组进行更新
	g_UpdateCount = 0;
	int UpdateSum = 0;
	for (UINT i = 0; i < m_TableVec.size(); ++i)
	{
		if (!m_TableVec[i])
			continue;
		if (m_TableVec[i]->GetTablePlayerSum() == 0)
			continue;
		int idx = i % THREAD_NUM;
		TableUpdateParam* pParam = (TableUpdateParam*)malloc(sizeof(TableUpdateParam));
		if (!pParam)
		{
			ASSERT(false);
			return;
		}
		pParam->m_bUpdateRp = m_bUpdateRp;
		pParam->m_bUpdateTime = m_bUpdateTime;
		pParam->m_bUpdateRpEffect = m_bUpdateRpEffect;
		pParam->pTable = m_TableVec[i];
		g_SafeList[idx].AddItem(pParam);
		++UpdateSum;
	}
	while (g_UpdateCount != UpdateSum)
		Sleep(1);
}
GameTable* TableManager::GetTable(WORD TableID)
{
	{
	if (TableID == 0xffff || TableID >= m_TableVec.size())
		//ASSERT(false);
		return NULL;
	}
	GameTable* pTable = m_TableVec[TableID];
	return pTable;
}
bool TableManager::OnPlayerJoinTable(WORD TableID, CRoleEx* pRoleEx, bool IsSendToClient)
{
	//玩家进入一张桌子 
	//判断玩家是否可以加入房间
	//首先发送玩家是否可以进入房间的命令 进入成功后 在发送其他玩家的数据
	if (!pRoleEx)
	{
		ASSERT(false);
		return false;
	}
	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return false;
	}
	BYTE TableTypeID = pTable->GetTableTypeID();
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(pRoleEx->GetUserID());
	if (Iter != m_RoleTableMap.end())
	{
		//玩家已经有桌子了无法继续进入桌子
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return false;
	}
	HashMap<BYTE, TableInfo>::iterator IterConfig = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(TableTypeID);
	if (IterConfig == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return false;
	}

	if (pRoleEx->IsRobot() && !pTable->IsCanAddRobot())
	{
		return false;
	}
	if (pTable->OnRoleJoinTable(pRoleEx, pTable->GetTableMonthID(), IsSendToClient))
	{
		ServerClientData* pData = g_FishServer.GetUserClientDataByIndex(pRoleEx->GetGameSocketID());
		if (pData)
		{
			pData->IsInScene = true;
		}

		if (pTable->GetTableMonthID() == 0)
			OnChangeTableTypePlayerSum(TableTypeID, true);

		

		DBR_Cmd_TableChange msgDB;//记录玩家进入
		SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
		msgDB.dwUserID = pRoleEx->GetUserID();
		msgDB.CurrceySum = pRoleEx->GetRoleInfo().dwCurrencyNum;
		msgDB.GlobelSum = pRoleEx->GetRoleInfo().dwGlobeNum;
		msgDB.MedalSum = pRoleEx->GetRoleInfo().dwMedalNum;
		msgDB.JoinOrLeave = true;
		msgDB.LogTime = time(null);
		msgDB.TableTypeID = TableTypeID;
		msgDB.TableMonthID = pTable->GetTableMonthID();
		g_FishServer.SendNetCmdToSaveDB(&msgDB);
		g_DBLogManager.LogRoleJoinOrLeaveTableInfo(msgDB.dwUserID, msgDB.GlobelSum, msgDB.CurrceySum, msgDB.MedalSum, msgDB.TableTypeID, msgDB.TableMonthID, true, SendLogDB);

		//用户成功加入桌子 我们想要发送命令到客户端去 告诉客户端玩家进入游戏成功
		m_RoleTableMap.insert(HashMap<DWORD, WORD>::value_type(pRoleEx->GetUserID(), pTable->GetTableID()));
		LogInfoToFile("CollideBuYu.txt", "OnPlayerJoinTable加入桌子条件 33322 *******-----%d******************** %d-----", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());


		if (pTable->GetTableMonthID() == 0 && !pRoleEx->IsRobot())
			g_FishServer.GetRobotManager().OnRoleJoinNormalRoom(pTable);
		return true;
	}
	else
	{
		//非人满的情况 不可以进入桌子 我们无须继续判断
		//发送命令到客户端 告诉玩家进入桌子失败了
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		return false;
	}
}


//创建房间
void TableManager::OnPlayerSitDown(BYTE TableTypeID, CRoleEx* pRoleEx, NetCmd* pData, BYTE MonthID, bool IsSendToClient)
{
	if (!pRoleEx)
	{
		//ASSERT(false);
		return;
	}

	//玩家进入桌子
	CL_JoinTable* pMsg = (CL_JoinTable*)pData;
	if (!pMsg)
	{
		return;
	}
	LogInfoToFile("CollideBuYu.txt", "创建房间  创建房间  创建房间  在这里添加创建房间消息*****%d********** %d-----", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());
	////////////////////////////////////////////////////////////////////////////	
	//std::shared_ptr<CTableFrame> pTableFrame;

	//如果有自己创建的私有房间，则回到自己的房间。
	auto iter = m_UserID_2_PriTableID.find(pRoleEx->GetUserID());
	if (iter != m_UserID_2_PriTableID.end())
	{
		int sPrivateTableID = iter->second;
		//找到桌子
		GameTable* pTable = GetTable(m_PriTableID_2_TableID[sPrivateTableID]);
		//pTableFrame = GetPriTableFrame(sPrivateTableID);
		//如果桌子存在
		if (pTable)
		{
			//1表示创建
			if (1 == pMsg->bCreateTable)
			{
				//普通玩家不能重复创建房间，vip用户可以重复创建房间。
				//if (0 == pIServerUserItem->GetMemberOrder())		
				//{	//非vip创建房间
				//	SendRequestFailure(pIServerUserItem, TEXT("你已创建了房间，请先解散当前房间！"), 0);
				//	return true;
				//}
				LogInfoToFile("CollideBuYu.txt", TEXT("创建房间自己创建的私有房间1"));
				return ;
			}
			else
			{
				//////////////////////////////////////////////////////////////////////////修改原逻辑  多个房间
				//非VIP房间默认返回0号座位，VIP房间需要随机分配座位
				//if (pTable->bVipTable())
				//	wRequestChairID = pTableFrame->GetNullChairID();
				//else
				//	//wRequestChairID = 0;//房主默认坐0号位置GetNullChairIDFree
				//	wRequestChairID = pTableFrame->GetNullChairIDFree();
				//	//wRequestChairID = pTableFrame->GetNullChairID();
				//pTable->PerformSitDownAction(wRequestChairID, pIServerUserItem, pUserSitDown->szPassword, 1);//加入房间
				//////////////////////////////////////////////////////////////////////////

				LogInfoToFile("CollideBuYu.txt", TEXT("创建房间自己创建的私有房间2"));
				DWORD NowTime = timeGetTime();//创建房间的时间

				//桌子插入后 我们开始进入
				if (pTable->OnRoleJoinTable(pRoleEx, MonthID, IsSendToClient))
				{
					//玩家进入桌子成功了
					ServerClientData* pData = g_FishServer.GetUserClientDataByIndex(pRoleEx->GetGameSocketID());
					if (pData)
					{
						pData->IsInScene = true;
					}

					if (MonthID == 0)
						OnChangeTableTypePlayerSum(TableTypeID, true);

					DBR_Cmd_TableChange msgDB;//记录玩家进入
					SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
					msgDB.dwUserID = pRoleEx->GetUserID();
					msgDB.CurrceySum = pRoleEx->GetRoleInfo().dwCurrencyNum;
					msgDB.GlobelSum = pRoleEx->GetRoleInfo().dwGlobeNum;
					msgDB.MedalSum = pRoleEx->GetRoleInfo().dwMedalNum;
					msgDB.JoinOrLeave = true;
					msgDB.LogTime = time(null);
					msgDB.TableTypeID = TableTypeID;
					msgDB.TableMonthID = MonthID;
					g_FishServer.SendNetCmdToSaveDB(&msgDB);
					g_DBLogManager.LogRoleJoinOrLeaveTableInfo(msgDB.dwUserID, msgDB.GlobelSum, msgDB.CurrceySum, msgDB.MedalSum, msgDB.TableTypeID, msgDB.TableMonthID, true, SendLogDB);
					m_RoleTableMap.insert(HashMap<DWORD, WORD>::value_type(pRoleEx->GetUserID(), m_TableVec.size() - 1));
					LogInfoToFile("CollideBuYu.txt", "OnPlayerJoinTable创建房间 创建33322 ******%d********** %d-----", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());
					
					if (MonthID == 0 && !pRoleEx->IsRobot())
						g_FishServer.GetRobotManager().OnRoleCreateNormalRoom(pTable);

					return;
				}
				else
				{
					LC_JoinTableResult msg;
					SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
					msg.bTableTypeID = TableTypeID;
					msg.Result = false;
					pRoleEx->SendDataToClient(&msg);
					ASSERT(false);
					return;
				}
				return ;
				////////////////////////////////////////////////////////////////////////
			}

		}
		else {

			//桌子不存在，然后创建
			m_UserID_2_PriTableID.erase(iter);
			LogInfoToFile("CollideBuYu.txt", TEXT("创建房间自己创建的私有房间3"));
			////通知删除私有房间号
			//CMD_GR_USER_PRIVATE_TABLE_ID req;
			//lstrcpy(req.szPrivateTableID, L"");
			//req.dwUserID = 0;
			//SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_PRIVATE_TABLE_ID, &req, sizeof(req));

			//if (2 == pUserSitDown->cbCreateTable)
			//{
			//	SendRequestFailure(pIServerUserItem, TEXT("对不起，你还没有创建房间！"), 0);
			//	return true;
			//}
			return;
		}

	}


	if (0 != pMsg->bCreateTable)	//进入私有桌子
	{
		LogInfoToFile("CollideBuYu.txt", TEXT("创建房间自己创建的私有房间4"));
		//////////////////////////////////////////////////////////////////////////
		//tagPrivateRoomCost cost;
		//ZeroMemory(&cost, sizeof(cost));
		if (1 == pMsg->bCreateTable) {

			//tagGameConfig* pConfig = (tagGameConfig*)pUserSitDown->TableConfig;
			//auto pTableFrame = GetTableFrameInstance();
			////这里创建桌子 
			//if (!pTableFrame)
			//{
			//	//SendRequestFailure(pIServerUserItem, TEXT("没有这个房间，请确认后再试！11"), 0);
			//	return;
			//}
			LogInfoToFile("CollideBuYu.txt", "进入私有桌子****%d********** %d-----", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());
			GameTable* pTable = new GameTable();
			if (!pTable)
			{
				LC_JoinTableResult msg;
				SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
				msg.bTableTypeID = TableTypeID;
				msg.Result = false;
				pRoleEx->SendDataToClient(&msg);
				//ASSERT(false);
				return;
			}
			auto tableId = m_privateTableIDPool.front();
			m_privateTableIDPool.pop_front();

			pTable->OnInit(tableId, TableTypeID, MonthID);//将桌子初始化   生成新桌子
			//m_MaxTableID++;
			if (pTable)
			{
				auto tableId = pTable->GetTableID();
				//	m_TableFrameArray[tableId] = table;
				pTable->GetPrivateTableID() = pMsg->szPrivateRoomID;//这里应该从数据库返回房间号
				pTable->GetCreateTime() = time(0);
			}
			//将创建好的桌子放入容器
			m_UserID_2_PriTableID[pRoleEx->GetUserID()] = pTable->GetPrivateTableID();
			m_PriTableID_2_TableID[pTable->GetPrivateTableID()] = pTable->GetTableID();
			m_TableVec.push_back(pTable);
			//设置房主
			pTable->SetPrivateTableOwnerID(pRoleEx->GetUserID());

			DWORD NowTime = timeGetTime();//创建房间的时间
			//pTableFrame->GetConsumeCurrency(cost, pConfig, 1);
		}
		else if (2 == pMsg->bCreateTable)
		{
			//如果是加入房间
			///	pTableFrame = GetPriTableFrame(pUserSitDown->szPrivateTableID);
			LogInfoToFile("CollideBuYu.txt", TEXT("创建房间自己创建的私有房间5"));			
			GameTable* pTable = GetTable(m_PriTableID_2_TableID[pMsg->szPrivateRoomID]);
			if (!pTable)
			{
				//SendRequestFailure(pIServerUserItem, TEXT("没有这个房间，请确认后再试！"), 0);
				return;
			}
			DWORD NowTime = timeGetTime();//加入房间的时间
			// 			pTableFrame->GetConsumeCurrency(cost);
			// 			tagGameConfig* pConfig = (tagGameConfig*)pUserSitDown->TableConfig;	// 
			// 			if (pConfig->wPayCosType == 1)//房主支付
			// 			{
			// 				cost.dwFeeCount = 0;
			// 			}

			LogInfoToFile("CollideBuYu.txt", "如果是加入房间****%d********** %d-----", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());
		}
	}

	//找到查询条件后 我们将命令转为DBR数据 发送到数据库去
	DBR_Cmd_QueryPrivateRoom msg;
	SetMsgInfo(msg, DBR_QueryPrivateRoom, sizeof(DBR_Cmd_QueryPrivateRoom));
	msg.cbCreateRoom = (1 == pMsg->bCreateTable) ? 1 : 0;
	//if (pConfig->wPayCosType==1&& pIServerUserItem->GetUserID()!=)//房主支付
	//{
	//	req.dwCostFeeCount = cost.dwFeeCount;
	//}

	msg.dwCostFeeCount = 1;//开房间费用    dang当然这个只是数据加减  需要同步客户端才行   捕鱼的缓存
	msg.wFeeType = 1;// cost.wFeeType;   //0 金币  1钻石
	msg.dwUserID = pRoleEx->GetUserID();
	msg.wDataSize = sizeof(DBR_Cmd_QueryPrivateRoom);
	//添加参数
	//DWORD NowTime = timeGetTime();//创建房间的时间

	msg.wDifen = 1;//pConfig->wDifen;
	msg.wPlayCountRule = 10;//pConfig->wPlayCountRule;//原来是局数现在是时间   时间限制  10fen
	msg.bTableTypeID = pMsg->bTableTypeID;
	g_FishServer.SendNetCmdToDB(&msg);
	LogInfoToFile("CollideBuYu.txt", TEXT("创建房间自己创建的私有房间46"));

	return;
}
//删除房间  房卡 
bool TableManager::DeletePrivateTable(GameTable* pTableFrame, CRoleEx* pRoleEx)
{
	if (!pTableFrame)
		return false;
	if (!pRoleEx)
	{
		return false;
	}
	//std::vector<GameTable*>::iterator Iter = m_TableVec.begin();

	m_PriTableID_2_TableID.erase(pTableFrame->GetPrivateTableID());
	m_UserID_2_PriTableID.erase(pTableFrame->GetPrivateTableOwnerID());
	m_privateTableIDPool.push_back(pTableFrame->GetTableID());

	DBR_GR_DeletePrivateRoom req;	
	SetMsgInfo(req, DBR_DeletePrivateRoom, sizeof(DBR_GR_DeletePrivateRoom));
	req.dwUserID = pTableFrame->GetPrivateTableOwnerID();
//	lstrcpy(req.szPrivateTableID, pTableFrame->GetPrivateTableID().c_str());
	req.szPrivateTableID = pTableFrame->GetPrivateTableID();
	g_FishServer.SendNetCmdToDB(&req);
	//m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_PRIVATE_ROOM, 0, &req, sizeof(req));
	return true;
}

//房卡专用
void TableManager::OnPlayerCreateOrJoinTable(BYTE TableTypeID, CRoleEx* pRoleEx, NetCmd* pData, BYTE MonthID, bool IsSendToClient)
{
	LogInfoToFile("CollideBuYu.txt", TEXT("房卡专用--------"));
	if (!pRoleEx)
	{		
		return;
	}
	//玩家进入桌子
	CL_JoinTableOne* pMsg = (CL_JoinTableOne*)pData;

	if (!pMsg)
	{		
		return ;
	}

	LogInfoToFile("CollideBuYu.txt", TEXT("房卡专用"));
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(pRoleEx->GetUserID());
	if (Iter != m_RoleTableMap.end())
	{
		//玩家已经有桌子了无法继续进入桌子
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return;
	}
	LogInfoToFile("CollideBuYu.txt", TEXT("房卡专用1"));
	HashMap<BYTE, TableInfo>::iterator IterConfig = g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(TableTypeID);
	if (IterConfig == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return;
	}
	LogInfoToFile("CollideBuYu.txt", TEXT("房卡专用2"));
	// 添加
	if (pMsg->bCreateTable==2)
	{
		GameTable* pTable = GetTable(m_PriTableID_2_TableID[pMsg->szPrivateRoomID]);
		if (!pTable)
		{
			//SendRequestFailure(pIServerUserItem, TEXT("没有这个房间，请确认后再试！"), 0);
			return;
		}

		LogInfoToFile("CollideBuYu.txt", TEXT("房卡专用3"));
		//桌子插入后 我们开始进入
		if (pTable->OnRoleJoinTable(pRoleEx, MonthID, IsSendToClient))
		{
			//玩家进入桌子成功了
			ServerClientData* pData = g_FishServer.GetUserClientDataByIndex(pRoleEx->GetGameSocketID());
			if (pData)
			{
				pData->IsInScene = true;
			}

			if (MonthID == 0)
				OnChangeTableTypePlayerSum(TableTypeID, true);

			DBR_Cmd_TableChange msgDB;//记录玩家进入
			SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
			msgDB.dwUserID = pRoleEx->GetUserID();
			msgDB.CurrceySum = pRoleEx->GetRoleInfo().dwCurrencyNum;
			msgDB.GlobelSum = pRoleEx->GetRoleInfo().dwGlobeNum;
			msgDB.MedalSum = pRoleEx->GetRoleInfo().dwMedalNum;
			msgDB.JoinOrLeave = true;
			msgDB.LogTime = time(null);
			msgDB.TableTypeID = TableTypeID;
			msgDB.TableMonthID = MonthID;
			g_FishServer.SendNetCmdToSaveDB(&msgDB);
			g_DBLogManager.LogRoleJoinOrLeaveTableInfo(msgDB.dwUserID, msgDB.GlobelSum, msgDB.CurrceySum, msgDB.MedalSum, msgDB.TableTypeID, msgDB.TableMonthID, true, SendLogDB);

			m_RoleTableMap.insert(HashMap<DWORD, WORD>::value_type(pRoleEx->GetUserID(), m_TableVec.size() - 1));
			LogInfoToFile("CollideBuYu.txt", "创建 创建 记录玩家进入33322 ******%d********** %d-----", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());


			if (MonthID == 0 && !pRoleEx->IsRobot())
				g_FishServer.GetRobotManager().OnRoleCreateNormalRoom(pTable);

			return;
		}
		else
		{
			LC_JoinTableResult msg;
			SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
			msg.bTableTypeID = TableTypeID;
			msg.Result = false;
			pRoleEx->SendDataToClient(&msg);
			ASSERT(false);
			return;
		}

		//////////////////////////////////////////////////////////////////////////
// 		std::vector<GameTable*>::iterator IterVec = m_TableVec.begin();
// 		//自动检索已存在的桌子  如果这些桌子都满人了就创建一个新桌子  
// 		for (WORD i = 0; IterVec != m_TableVec.end(); ++IterVec, ++i)
// 		{
// 			if (!(*IterVec))
// 				continue;
// 
// 			if ((*IterVec)->GetTableTypeID() == TableTypeID && MonthID == (*IterVec)->GetTableMonthID() && !(*IterVec)->IsFull())
// 			{
// 				//判断机器人是否可以加入当前桌子
// 				if (pRoleEx->IsRobot() && !(*IterVec)->IsCanAddRobot())
// 				{
// 					continue;
// 				}
// 				if ((*IterVec)->OnRoleJoinTable(pRoleEx, MonthID, IsSendToClient))
// 				{
// 					ServerClientData* pData = g_FishServer.GetUserClientDataByIndex(pRoleEx->GetGameSocketID());
// 					if (pData)
// 					{
// 						pData->IsInScene = true;
// 					}
// 
// 					if (MonthID == 0)
// 						OnChangeTableTypePlayerSum(TableTypeID, true);
// 
// 					DBR_Cmd_TableChange msgDB;//记录玩家进入
// 					SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
// 					msgDB.dwUserID = pRoleEx->GetUserID();
// 					msgDB.CurrceySum = pRoleEx->GetRoleInfo().dwCurrencyNum;
// 					msgDB.GlobelSum = pRoleEx->GetRoleInfo().dwGlobeNum;
// 					msgDB.MedalSum = pRoleEx->GetRoleInfo().dwMedalNum;
// 					msgDB.JoinOrLeave = true;
// 					msgDB.LogTime = time(null);
// 					msgDB.TableTypeID = TableTypeID;
// 					msgDB.TableMonthID = MonthID;
// 					g_FishServer.SendNetCmdToSaveDB(&msgDB);
// 					g_DBLogManager.LogRoleJoinOrLeaveTableInfo(msgDB.dwUserID, msgDB.GlobelSum, msgDB.CurrceySum, msgDB.MedalSum, msgDB.TableTypeID, msgDB.TableMonthID, true, SendLogDB);
// 
// 					//用户成功加入桌子 我们想要发送命令到客户端去 告诉客户端玩家进入游戏成功
// 					m_RoleTableMap.insert(HashMap<DWORD, WORD>::value_type(pRoleEx->GetUserID(), i));
// 
// 					LogInfoToFile("CollideBuYu.txt", "OnPlayerJoinTable玩家进入一张桌子 33322 ******%d --------%d------", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());
// 
// 					if (MonthID == 0 && !pRoleEx->IsRobot())
// 						g_FishServer.GetRobotManager().OnRoleJoinNormalRoom(*IterVec);
// 
// 					return;
// 				}
// 				else
// 				{
// 					//非人满的情况 不可以进入桌子 我们无须继续判断
// 					//发送命令到客户端 告诉玩家进入桌子失败了
// 					LC_JoinTableResult msg;
// 					SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
// 					msg.bTableTypeID = TableTypeID;
// 					msg.Result = false;
// 					pRoleEx->SendDataToClient(&msg);
// 					return;
// 				}
// 			}
		//}

	}
else if (pMsg->bCreateTable == 1)//创建
{

	LogInfoToFile("CollideBuYu.txt", "创建 创建 ******%d********** %d-----", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());
	GameTable* pTable = new GameTable();
	if (!pTable)
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		//ASSERT(false);
		return;
	}
	auto tableId = m_privateTableIDPool.front();
	m_privateTableIDPool.pop_front();

	pTable->OnInit(tableId, TableTypeID, MonthID);//将桌子初始化   生成新桌子
	//m_MaxTableID++;
	if (pTable)
	{
		auto tableId = pTable->GetTableID();
	//	m_TableFrameArray[tableId] = table;

		pTable->GetPrivateTableID() = pMsg->szPrivateRoomID;//这里应该从数据库返回房间号
		pTable->GetCreateTime() = time(0);
	}

	LogInfoToFile("CollideBuYu.txt", TEXT("房卡专用55"));
	
	//将创建好的桌子放入容器
	m_UserID_2_PriTableID[pRoleEx->GetUserID()] = pTable->GetPrivateTableID();
	m_PriTableID_2_TableID[pTable->GetPrivateTableID()] = pTable->GetTableID();

	m_TableVec.push_back(pTable);

	//桌子插入后 我们开始进入
	if (pTable->OnRoleJoinTable(pRoleEx, MonthID, IsSendToClient))
	{
		//玩家进入桌子成功了
		ServerClientData* pData = g_FishServer.GetUserClientDataByIndex(pRoleEx->GetGameSocketID());
		if (pData)
		{
			pData->IsInScene = true;
		}

		if (MonthID == 0)
			OnChangeTableTypePlayerSum(TableTypeID, true);

		DBR_Cmd_TableChange msgDB;//记录玩家进入
		SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
		msgDB.dwUserID = pRoleEx->GetUserID();
		msgDB.CurrceySum = pRoleEx->GetRoleInfo().dwCurrencyNum;
		msgDB.GlobelSum = pRoleEx->GetRoleInfo().dwGlobeNum;
		msgDB.MedalSum = pRoleEx->GetRoleInfo().dwMedalNum;
		msgDB.JoinOrLeave = true;
		msgDB.LogTime = time(null);
		msgDB.TableTypeID = TableTypeID;
		msgDB.TableMonthID = MonthID;
		g_FishServer.SendNetCmdToSaveDB(&msgDB);
		g_DBLogManager.LogRoleJoinOrLeaveTableInfo(msgDB.dwUserID, msgDB.GlobelSum, msgDB.CurrceySum, msgDB.MedalSum, msgDB.TableTypeID, msgDB.TableMonthID, true, SendLogDB);

		m_RoleTableMap.insert(HashMap<DWORD, WORD>::value_type(pRoleEx->GetUserID(), m_TableVec.size() - 1));
		LogInfoToFile("CollideBuYu.txt", "OnPlayerJoinTable桌子插入后 33322 ******%d********** %d-----", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());


		if (MonthID == 0 && !pRoleEx->IsRobot())
			g_FishServer.GetRobotManager().OnRoleCreateNormalRoom(pTable);

		return;
	}
	else
	{
		LogInfoToFile("CollideBuYu.txt", TEXT("房卡专用222"));
		LC_JoinTableResult msg;
		SetMsgInfo(msg, GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return;
	}

}



return;


}
//房卡模式修改入口   桌子的生成
void TableManager::OnPlayerJoinTable(BYTE TableTypeID, CRoleEx* pRoleEx, BYTE MonthID, bool IsSendToClient)
{
	//玩家进入一张桌子 
	//判断玩家是否可以加入房间
	//首先发送玩家是否可以进入房间的命令 进入成功后 在发送其他玩家的数据
	if (!pRoleEx)
	{
		ASSERT(false);
		return;
	}
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(pRoleEx->GetUserID());
	if (Iter != m_RoleTableMap.end())
	{
		//玩家已经有桌子了无法继续进入桌子
		LC_JoinTableResult msg;
		SetMsgInfo(msg,GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return;
	}

	HashMap<BYTE, TableInfo>::iterator IterConfig =  g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.find(TableTypeID);
	if (IterConfig == g_FishServer.GetFishConfig().GetTableConfig().m_TableConfig.end())
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg,GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return;
	}
	std::vector<GameTable*>::iterator IterVec = m_TableVec.begin();
	//自动检索已存在的桌子  如果这些桌子都满人了就创建一个新桌子  
	for (WORD i = 0; IterVec != m_TableVec.end(); ++IterVec, ++i)
	{
		if (!(*IterVec))
			continue;

		if ((*IterVec)->GetTableTypeID() == TableTypeID && MonthID == (*IterVec)->GetTableMonthID() && !(*IterVec)->IsFull())
		{
			//判断机器人是否可以加入当前桌子
			if (pRoleEx->IsRobot() && !(*IterVec)->IsCanAddRobot())
			{
				continue;
			}
			if ((*IterVec)->OnRoleJoinTable(pRoleEx,MonthID,IsSendToClient))
			{
				ServerClientData* pData = g_FishServer.GetUserClientDataByIndex(pRoleEx->GetGameSocketID());
				if (pData)
				{
					pData->IsInScene = true;
				}

				if (MonthID == 0)
					OnChangeTableTypePlayerSum(TableTypeID, true);

				DBR_Cmd_TableChange msgDB;//记录玩家进入
				SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
				msgDB.dwUserID = pRoleEx->GetUserID();
				msgDB.CurrceySum = pRoleEx->GetRoleInfo().dwCurrencyNum;
				msgDB.GlobelSum = pRoleEx->GetRoleInfo().dwGlobeNum;
				msgDB.MedalSum = pRoleEx->GetRoleInfo().dwMedalNum;
				msgDB.JoinOrLeave = true;
				msgDB.LogTime = time(null);
				msgDB.TableTypeID = TableTypeID;
				msgDB.TableMonthID = MonthID;
				g_FishServer.SendNetCmdToSaveDB(&msgDB);
				g_DBLogManager.LogRoleJoinOrLeaveTableInfo(msgDB.dwUserID, msgDB.GlobelSum, msgDB.CurrceySum, msgDB.MedalSum, msgDB.TableTypeID, msgDB.TableMonthID, true, SendLogDB);

				//用户成功加入桌子 我们想要发送命令到客户端去 告诉客户端玩家进入游戏成功
				m_RoleTableMap.insert(HashMap<DWORD, WORD>::value_type(pRoleEx->GetUserID(), i));

				LogInfoToFile("CollideBuYu.txt", "OnPlayerJoinTable玩家进入一张桌子 33322 ******%d --------%d------", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());

				if (MonthID == 0 && !pRoleEx->IsRobot())
					g_FishServer.GetRobotManager().OnRoleJoinNormalRoom(*IterVec);

				return;
			}
			else
			{
				//非人满的情况 不可以进入桌子 我们无须继续判断
				//发送命令到客户端 告诉玩家进入桌子失败了
				LC_JoinTableResult msg;
				SetMsgInfo(msg,GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
				msg.bTableTypeID = TableTypeID;
				msg.Result = false;
				pRoleEx->SendDataToClient(&msg);
				return;
			}
		}
	}
	GameTable* pTable = new GameTable();
	if (!pTable)
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg,GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		//ASSERT(false);
		return;
	}
	pTable->OnInit(m_MaxTableID, TableTypeID,MonthID);//将桌子初始化   生成新桌子
	m_MaxTableID++;
	m_TableVec.push_back(pTable);
	//桌子插入后 我们开始进入
	if (pTable->OnRoleJoinTable(pRoleEx,MonthID,IsSendToClient))
	{
		//玩家进入桌子成功了
		ServerClientData* pData = g_FishServer.GetUserClientDataByIndex(pRoleEx->GetGameSocketID());
		if (pData)
		{
			pData->IsInScene = true;
		}

		if (MonthID == 0)
			OnChangeTableTypePlayerSum(TableTypeID, true);

		DBR_Cmd_TableChange msgDB;//记录玩家进入
		SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
		msgDB.dwUserID = pRoleEx->GetUserID();
		msgDB.CurrceySum = pRoleEx->GetRoleInfo().dwCurrencyNum;
		msgDB.GlobelSum = pRoleEx->GetRoleInfo().dwGlobeNum;
		msgDB.MedalSum = pRoleEx->GetRoleInfo().dwMedalNum;
		msgDB.JoinOrLeave = true;
		msgDB.LogTime = time(null);
		msgDB.TableTypeID = TableTypeID;
		msgDB.TableMonthID = MonthID;
		g_FishServer.SendNetCmdToSaveDB(&msgDB);
		g_DBLogManager.LogRoleJoinOrLeaveTableInfo(msgDB.dwUserID, msgDB.GlobelSum, msgDB.CurrceySum, msgDB.MedalSum, msgDB.TableTypeID, msgDB.TableMonthID, true, SendLogDB);

		m_RoleTableMap.insert(HashMap<DWORD, WORD>::value_type(pRoleEx->GetUserID(), m_TableVec.size() - 1));
		LogInfoToFile("CollideBuYu.txt", "OnPlayerJoinTable桌子插入后 33322 ******%d********** %d-----", pRoleEx->GetRoleInfo().dwGlobeNum, pRoleEx->GetUserID());


		if (MonthID == 0 && !pRoleEx->IsRobot())
			g_FishServer.GetRobotManager().OnRoleCreateNormalRoom(pTable);

		return;
	}
	else
	{
		LC_JoinTableResult msg;
		SetMsgInfo(msg,GetMsgType(Main_Table, LC_Sub_JoinTable), sizeof(LC_JoinTableResult));
		msg.bTableTypeID = TableTypeID;
		msg.Result = false;
		pRoleEx->SendDataToClient(&msg);
		ASSERT(false);
		return;
	}
}
void TableManager::ResetTableInfo(DWORD dwUserID)
{
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(dwUserID);
	if (Iter == m_RoleTableMap.end())
	{
		//ASSERT(false);
		return;
	}
	WORD Index = Iter->second;
	if (m_TableVec.size() <= Index)
	{
		ASSERT(false);
	}
	GameTable* pTable = m_TableVec[Index];
	if (!pTable)
	{
		ASSERT(false);
	}
	pTable->SendTableRoleInfoToClient(dwUserID);
}
void TableManager::OnPlayerLeaveTable(DWORD dwUserID)
{
	//根据玩家找到当前的桌子 
	//根据桌子找到玩家 然后让玩家离开桌子
	//CWHDataLocker lock(m_CriticalSection);
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(dwUserID);
	if (Iter == m_RoleTableMap.end())
	{
		//ASSERT(false);
		return;
	}
	WORD Index = Iter->second;
	if (m_TableVec.size() <= Index)
	{
		ASSERT(false);
	}
	GameTable* pTable = m_TableVec[Index];
	if (!pTable)
	{
		ASSERT(false);
	}

	if (pTable->GetTableMonthID() == 0)
		OnChangeTableTypePlayerSum(pTable->GetTableTypeID(), false);

	//当玩家离开桌子的时候 将金币刷新下到客户端去
	CRoleEx* pRole = g_FishServer.GetRoleManager()->QueryUser(dwUserID);
	if (pRole)
	{
		DBR_Cmd_TableChange msgDB;//记录玩家进入
		SetMsgInfo(msgDB, DBR_TableChange, sizeof(DBR_Cmd_TableChange));
		msgDB.dwUserID = pRole->GetUserID();
		msgDB.CurrceySum = pRole->GetRoleInfo().dwCurrencyNum;
		msgDB.GlobelSum = pRole->GetRoleInfo().dwGlobeNum;
		msgDB.MedalSum = pRole->GetRoleInfo().dwMedalNum;
		msgDB.JoinOrLeave = false;
		msgDB.LogTime = time(null);
		msgDB.TableTypeID = pTable->GetTableTypeID();
		msgDB.TableMonthID = pTable->GetTableMonthID();
		g_FishServer.SendNetCmdToSaveDB(&msgDB);
		g_DBLogManager.LogRoleJoinOrLeaveTableInfo(msgDB.dwUserID, msgDB.GlobelSum, msgDB.CurrceySum, msgDB.MedalSum, msgDB.TableTypeID, msgDB.TableMonthID, false, SendLogDB);

		if (pTable->GetTableMonthID() != 0)
		{
			pRole->GetRoleMonth().OnPlayerLeaveTable();//玩家离开比赛
		}

		ServerClientData* pData = g_FishServer.GetUserClientDataByIndex(pRole->GetGameSocketID());
		if (pData)
		{
			pData->IsInScene = false;
		}
		LogInfoToFile("CollideBuYu.txt", "OnPlayerLeaveTable将金币刷新下到客户端去 33322******* %d******* %d-----", pRole->GetRoleInfo().dwGlobeNum, pRole->GetUserID());


		LC_Cmd_ResetRoleGlobel msg;
		SetMsgInfo(msg, GetMsgType(Main_Role, LC_ResetRoleGlobel), sizeof(LC_Cmd_ResetRoleGlobel));
		msg.dwTotalGlobel = pRole->GetRoleInfo().dwGlobeNum;
		msg.dwLotteryScore = pRole->GetRoleInfo().LotteryScore;
		pRole->SendDataToClient(&msg);//
	}

	pTable->OnRoleLeaveTable(dwUserID);
	m_RoleTableMap.erase(Iter);//将玩家的数据删除掉
	if (pTable->GetTableMonthID() == 0 && !pRole->IsRobot())
	{
		g_FishServer.GetRobotManager().OnRoleLeaveNormalRoom(pTable);
	}
		
	//////////////////////////////////////////////////////////////////////////添加房卡模式的删除桌子  Index
	WORD	wTableID = pTable->GetTableID();

	if (pTable->GetTableTypeID() == 0)
	{
		//获取桌子玩家情况
		if (pTable->m_RoleManager.GetRoleSum() != 0)
		{
			return;
		}
		if (!m_TableVec.empty())
		{
			std::vector<GameTable*>::iterator Iter = m_TableVec.begin();

			for (; Iter != m_TableVec.end(); ++Iter)
			{
				if (!(*Iter))
					continue;

				if (m_TableVec[Index] != (*Iter))
				{
					continue;
				}
				//CTraceService::TraceString(TEXT("一个桌子对象 已经被销毁"), TraceLevel_Normal);
				LogInfoToFile("CollideBuYu.txt", "一个桌子对象 已经被销毁33322******* %d******* %d----------%d----------%d----", pRole->GetRoleInfo().dwGlobeNum, pRole->GetUserID(), Iter, Index);

				delete *Iter;

				m_TableVec.erase(Iter);
				m_privateTableIDPool.push_back(wTableID);
				//删除桌子  zheshi  这是测试  
				DeletePrivateTable(m_TableVec[Index], pRole);
			}			
			//m_TableVec.clear();
			
		}

	}
	//////////////////////////////////////////////////////////////////////////

}
bool TableManager::OnHandleTableMsg(DWORD dwUserID,NetCmd* pData)
{
	//将Game消息传递到内部去
	//CWHDataLocker lock(m_CriticalSection);
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(dwUserID);
	if (Iter == m_RoleTableMap.end())
	{
		//ASSERT(false);
		return false;
	}
	WORD Index = Iter->second;
	if (m_TableVec.size() <= Index)
	{
		ASSERT(false);
		return false;
	}
	GameTable* pTable = m_TableVec[Index];
	if (!pTable)
	{
		ASSERT(false); 
	}
	pTable->OnHandleTableMsg(dwUserID,pData);
	return true;
}
void TableManager::SendDataToTable(DWORD dwUserID, NetCmd* pData)
{
	//将命令发送到指定玩家的桌子上面去
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(dwUserID);
	if (Iter == m_RoleTableMap.end())
	{
		return;
	}
	WORD Index = Iter->second;
	if (m_TableVec.size() <= Index)
	{
		ASSERT(false);
		return;
	}
	GameTable* pTable = m_TableVec[Index];
	if (pTable)
	{
		pTable->SendDataToTable(dwUserID, pData);
	}
	else
	{
		ASSERT(false);
	}
}
CRole* TableManager::SearchUser(DWORD dwUserid)
{
	HashMap<DWORD, WORD>::iterator Iter = m_RoleTableMap.find(dwUserid);
	if (Iter == m_RoleTableMap.end())
	{
		//ASSERT(false);
		return NULL;
	}
	WORD Index = Iter->second;
	if (m_TableVec.size() <= Index)
	{
		ASSERT(false);
		return NULL; 
	}
	GameTable* pTable = m_TableVec[Index];
	if (!pTable)
	{
		ASSERT(false);
		return NULL;
	}
	return pTable->SearchUser(dwUserid);
}
CConfig* TableManager::GetGameConfig()
{
	return m_pGameConfig;
}
void TableManager::OnChangeTableGlobel(WORD TableID, int AddGlobel, USHORT uTableRate)
{
	
	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return;
	}
	int64 AllGlobelSum = 0;
	//////////////////////////////////////////////////////////////////////////修改
	if (AddGlobel>0)
	{
		//LogInfoToFile("Control.txt", "没打到鱼:::::::::::::::%d:::::%d::::", AddGlobel, KucunNum[pTable->GetTableTypeID()]);
		KucunNum[pTable->GetTableTypeID()] += AddGlobel*0.995;
	}
	else
	{
		//LogInfoToFile("Control.txt", "打到鱼:::::::::::::::%d::::%d:::::", AddGlobel, KucunNum[pTable->GetTableTypeID()]);
		KucunNum[pTable->GetTableTypeID()] += AddGlobel;
	}

	//////////////////////////////////////////////////////////////////////////
	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());

	if (Iter == m_TableGlobeMap.end())
	{
		AllGlobelSum = AddGlobel;
		m_TableGlobeMap.insert(HashMap<BYTE, GoldPool>::value_type(pTable->GetTableTypeID(), GoldPool(AllGlobelSum)));
	}
	else
	{
		Iter->second.gold += AddGlobel;
		AllGlobelSum = Iter->second.gold;
	}	

	Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());
	//{
	//	char szKey[10] = { 0 };
	//	char szValue[32] = { 0 };
	//	char szFile[256] = { 0 };
	//	sprintf_s(szKey, sizeof(szKey), "%d", pTable->GetTableTypeID());
	//	sprintf_s(szValue, sizeof(szValue), "%lld", AllGlobelSum);
	//	sprintf_s(szFile, sizeof(szFile), "%s\\%s", g_FishServerConfig.GetFTPServerConfig()->FTPFilePath, "fish.txt");
	//	WritePrivateProfileStringA("fish", szKey, szValue, szFile);
	//}	
	if (Iter->second.open)
	{
		if (AllGlobelSum <=0)
		{
			Iter->second.open = false;
			//Iter->second.gold = 0;			
		}
	}

	if(AllGlobelSum >= Con_LuckItem::s_threshold*uTableRate)//开启奖池的条件  1000*10
	{
		Iter->second.open = true;
		Iter->second.gold -= (int64)(Con_LuckItem::s_base*AllGlobelSum);//先扣  衰减
		byte nGiveIndex=Iter->second.byGive++;			

		DWORD UseGlobelSum = (DWORD)AllGlobelSum;// 		
		DWORD PlayerSum = 0;
		HashMap<BYTE, DWORD>::iterator Iter = m_TableTypeSum.find(pTable->GetTableTypeID());
		if (Iter == m_TableTypeSum.end())
		{
			PlayerSum = 0;
		}
		else
		{
			PlayerSum = Iter->second;
		}
		if (PlayerSum != 0 )
		{					
			DWORD RoleAddGlobel = (DWORD)min(UseGlobelSum / PlayerSum, UseGlobelSum *Con_LuckItem::s_part);
			if (nGiveIndex%2 == 0)//先到先得
			{
				RoleAddGlobel = (int64)(UseGlobelSum *Con_LuckItem::s_part);
			}
			DWORD AddLuckyValue = RoleAddGlobel ;			
			//将全部的幸运值放入玩家的身上
			std::vector<GameTable*>::iterator IterVec = m_TableVec.begin();
			for (; IterVec != m_TableVec.end(); ++IterVec)
			{
				if (!(*IterVec))
					continue;
				if ((*IterVec)->GetTableTypeID() == pTable->GetTableTypeID() && (*IterVec)->GetTableMonthID() == 0)
				{
					BYTE MaxSum = (*IterVec)->GetRoleManager().GetMaxPlayerSum();
					for (BYTE i = 0; i < MaxSum; ++i)
					{
						CRole* pRole = (*IterVec)->GetRoleManager().GetRoleBySeatID(i);
						if (pRole && pRole->IsActionUser())
						{
							pRole->SetRoleLuckyValue(AddLuckyValue);
						}
					}
				}
			}
		}
	}
}
//修改桌子的库存值
void TableManager::OnResetTableGlobel(WORD TableID, int64 nValue)
{
	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return;
	}
	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());
	if (Iter != m_TableGlobeMap.end())
	{
		Iter->second = nValue;
		LogInfoToFile("Control.txt", "OnResetTableGlobel修改桌子的库存值:::::::%d:::::::::::%d::::::::::::::::::::", nValue, TableID);
	}
}
int64 TableManager::GetTableGlobel(WORD TableID)
{
	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return 0;
	}
	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());
	if (Iter != m_TableGlobeMap.end())
		return Iter->second.gold;
	else
		return 0;
}
void TableManager::OnChangeTableTypePlayerSum(BYTE TableTypeID, bool IsAddOrDel)
{
	HashMap<BYTE, DWORD>::iterator Iter = m_TableTypeSum.find(TableTypeID);
	if (Iter == m_TableTypeSum.end())
	{
		if (IsAddOrDel)
		{
			m_TableTypeSum.insert(HashMap<BYTE, DWORD>::value_type(TableTypeID, 1));
			return;
		}
		else
		{
			ASSERT(false);
			return;
		}
	}
	else
	{
		if (IsAddOrDel)
		{
			Iter->second += 1;
			return;
		}
		else
		{
			if (Iter->second == 0)
			{
				ASSERT(false);
				return;
			}
			Iter->second -= 1;
			return;
		}
	}
}

bool TableManager::QueryPool(WORD TableID,int64 & nPoolGold)
{
	GameTable* pTable = GetTable(TableID);
	if (!pTable)
	{
		ASSERT(false);
		return false;
	}
	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(pTable->GetTableTypeID());
	if (Iter == m_TableGlobeMap.end())
	{
		return false;
	}
	nPoolGold = Iter->second.gold;
	return 	Iter->second.open;
}

void TableManager::QueryPool(BYTE TableTypeID, bool & bopen, int64 & nPoolGold)
{
	bopen = false;
	nPoolGold = 0;

	HashMap<BYTE, GoldPool>::iterator Iter = m_TableGlobeMap.find(TableTypeID);
	if (Iter == m_TableGlobeMap.end())
	{
		return;
	}
	bopen = Iter->second.open;
	nPoolGold = Iter->second.gold;
}

std::list<DWORD> TableManager::GetBlackList()
{
	return m_blacklist;
}

bool TableManager::SetBlackList(DWORD *pid, BYTE byCount)
{
	if (pid == NULL || byCount == 0)
	{
		return false;
	}
	m_blacklist.clear();
	for (int i = 0; i < byCount; i++)
	{
		m_blacklist.push_back(pid[i]);
	}	
	return true;
}

bool TableManager::Isabhor(DWORD dwUserid)
{
	return std::find(m_blacklist.begin(), m_blacklist.end(), dwUserid) != m_blacklist.end();
}

//修改概率
float TableManager::OnChangeGailv(float NumrateF, float NumrateS, float NumrateT, float NumrateR,bool IsContrl)
{

	if (!IsContrl)
	{
		RNumrateF = 0;
		RNumrateS = 0;
		RNumrateT = 0;
		RNumrateR = 0;
		return 0;
	}

	RNumrateF = NumrateF;
	RNumrateS = NumrateS;
	RNumrateT = NumrateT;
	RNumrateR = NumrateR;

	LogInfoToFile("Control.txt", "OnChangeGailv修改捕鱼概率控制入口:::::::%f:::::::::::%f::::::::::::::::::::", RNumrateF, RNumrateS);

	return NumrateF;
}

