#include "StdAfx.h"
#include "servercontrolitemsink.h"
#include"FishServer.h"

CServerControlOx::CServerControlOx(void)
{
	m_cbExcuteTimes = 0;								
	m_cbControlStyle = 0;	
	ZeroMemory(m_bWinArea, sizeof(m_bWinArea));	
}

CServerControlOx::~CServerControlOx( void )
{

}

//服务器控制  牛牛
bool  CServerControlOx::Control(const void * pDataBuffer)
{

	const CL_CMD_OxAdminReq* AdminReq = static_cast<const CL_CMD_OxAdminReq*>(pDataBuffer);

	if (!AdminReq)
	{
		return false;
	}
	//LogInfoToFile("Control.txt", "CL_OxAdmin服务器控制牛牛:::::");
	//统一一个结构
	LC_CMD_OxAdminReq cResult;
	cResult.SetCmdSize(sizeof(cResult));
	cResult.SetCmdType((Main_NiuNiu << 8) | LC_OxAdminContrlBack);
	cResult.ClientID = AdminReq->ClientID;

	cResult.dwUserID = AdminReq->dwUserID;//新添参数   查询玩家id

	cResult.dwUserIDContrl = AdminReq->dwUserIDContrl;//修改添加控制玩家的id

	LogInfoToFile("Control.txt", "CL_OxAdmin控制消息牛牛控制消息服务器控制::::%d::::::::::%d:::::::::::::%d:::::::::::%d:::::::%d::::", AdminReq->dwUserID, AdminReq->dwUserIDContrl, AdminReq->cbReqType, AdminReq->cbExtendData.m_cbControlStyle);
	switch (AdminReq->cbReqType)
	{
	case RQ_QUERY_NAMES:
	{
						   g_FishServer.GetNiuNiuManager().QueryNames(AdminReq->ClientID, AdminReq->dwUserIDContrl);
						   break;						   						   
	}
	case RQ_QUERY_PLAYERJETTON:
	{
								  g_FishServer.GetNiuNiuManager().QueryPlayerJetton(AdminReq->dwUserID, AdminReq->ClientID, AdminReq->dwUserIDContrl);
								  break;
	}
	case RQ_QUERY_ALLJETTON:
	{
							   g_FishServer.GetNiuNiuManager().QueryAllJetton(AdminReq->ClientID, AdminReq->dwUserIDContrl);
							   break;
	}
	case RQ_RESET_CONTROL:
	{
							 m_cbControlStyle = 0;
							 m_cbExcuteTimes = 0;
							 ZeroMemory(m_bWinArea, sizeof(m_bWinArea));

							 cResult.cbResult = CR_ACCEPT;
							 cResult.cbAckType = ACK_RESET_CONTROL;
							 //////////////////////////////////////////////////////////////////////////
							// CRoleEx* pRoleContrl =  GetRoleManager()->QuertUserBySocketID(pClient->OutsideExtraData);
							 //if (!pRoleContrl)
							 //{
								// ASSERT(false);
								// return false;
							 //}
							 //
							 //pRoleContrl->SendDataToClient(&msg);
						

							// g_FishServer.SendNetCmdToControl()

							// g_FishServer.		 HandleNiuNiuMsg
							 //////////////////////////////////////////////////////////////////////////

							// g_FishServer.SendNetCmdToControl(&cResult);

							 g_FishServer.HandleNiuNiuContrlBack(&cResult);//发送消息到游戏服务


							 break;
	}
	case RQ_SET_WIN_AREA:
	{

							//const tagOxAdminReq*pAdminReq = reinterpret_cast<const tagOxAdminReq*>(AdminReq->cbExtendData);

							//AdminReq->cbExtendData.m_cbControlStyle
							//switch (pAdminReq->m_cbControlStyle)
							switch (AdminReq->cbExtendData.m_cbControlStyle)
							{
							case CS_BET_AREA:	//区域设置
							{
													m_cbControlStyle = AdminReq->cbExtendData.m_cbControlStyle;//pAdminReq->m_cbControlStyle;
													m_cbExcuteTimes = AdminReq->cbExtendData.m_cbExcuteTimes;// pAdminReq->m_cbExcuteTimes;
													BYTE cbIndex = 0;
													for (cbIndex = 0; cbIndex < MAX_NIUNIU_ClientSum; cbIndex++)
													{
														m_bWinArea[cbIndex] = AdminReq->cbExtendData.m_bWinArea[cbIndex];//pAdminReq->m_bWinArea[cbIndex];
													}
													cResult.cbResult = CR_ACCEPT;
													cResult.cbAckType = ACK_SET_WIN_AREA;

													LogInfoToFile("Control.txt", "CL_OxAdmin牛牛区域控制消息::::%d::::::::::%d:::::::::::::%d:::::::::::%d:::::::%d::::", AdminReq->dwUserID, AdminReq->dwUserIDContrl, AdminReq->cbReqType, AdminReq->cbExtendData.m_cbControlStyle, AdminReq->cbExtendData.m_cbExcuteTimes);
													//g_FishServer.SendNetCmdToControl(&cResult);
													g_FishServer.HandleNiuNiuContrlBack(&cResult);//发送消息到游戏服务
													break;
							}
							case CS_BANKER_LOSE:	//庄家输牌
							{
														m_cbControlStyle = AdminReq->cbExtendData.m_cbControlStyle;//pAdminReq->m_cbControlStyle;
														m_cbExcuteTimes = AdminReq->cbExtendData.m_cbExcuteTimes; //pAdminReq->m_cbExcuteTimes;
														cResult.cbResult = CR_ACCEPT;
														cResult.cbAckType = ACK_SET_WIN_AREA;
														//g_FishServer.SendNetCmdToControl(&cResult);

														g_FishServer.HandleNiuNiuContrlBack(&cResult);//发送消息到游戏服务
														break;
							}
							case CS_BANKER_WIN:		//庄家赢牌
							{
														m_cbControlStyle = AdminReq->cbExtendData.m_cbControlStyle;// pAdminReq->m_cbControlStyle;
														m_cbExcuteTimes = AdminReq->cbExtendData.m_cbExcuteTimes;// pAdminReq->m_cbExcuteTimes;
														cResult.cbResult = CR_ACCEPT;
														cResult.cbAckType = ACK_SET_WIN_AREA;
														//g_FishServer.SendNetCmdToControl(&cResult);

														g_FishServer.HandleNiuNiuContrlBack(&cResult);//发送消息到游戏服务
														break;
							}
							default:	//拒绝请求
							{
											cResult.cbResult = CR_REFUSAL;
											cResult.cbAckType = ACK_SET_WIN_AREA;
											//g_FishServer.SendNetCmdToControl(&cResult);

											g_FishServer.HandleNiuNiuContrlBack(&cResult);//发送消息到游戏服务
											break;
							}
							}

							break;
	}
	case RQ_PRINT_SYN:
	{
						 cResult.cbResult = CR_ACCEPT;
						 cResult.cbAckType = ACK_PRINT_SYN;
						//tagOxAdminReq*pAdminReq = reinterpret_cast<tagOxAdminReq*>(cResult.cbExtendData);
						 cResult.cbExtendData.m_cbControlStyle = m_cbControlStyle;
						 cResult.cbExtendData.m_cbExcuteTimes = m_cbExcuteTimes;
						// pAdminReq->m_cbControlStyle = m_cbControlStyle;
						 //pAdminReq->m_cbExcuteTimes = m_cbExcuteTimes;cbExtendData
						// memcpy(pAdminReq->m_bWinArea, m_bWinArea, sizeof(m_bWinArea));

						 memcpy(cResult.cbExtendData.m_bWinArea, m_bWinArea, sizeof(m_bWinArea));


						 //g_FishServer.SendNetCmdToControl(&cResult);

						 g_FishServer.HandleNiuNiuContrlBack(&cResult);//发送消息到游戏服务

						 break;
	}
	default:
	{
			   break;
	}
	}
	return true;
}

//需要控制
bool  CServerControlOx::NeedControl()
{
	return m_cbControlStyle > 0 && m_cbExcuteTimes > 0;
}


//完成控制
bool  CServerControlOx::CompleteControl()
{
	if (m_cbExcuteTimes > 0)
		m_cbExcuteTimes--;	
	return true;
}

//控制牌型
void  CServerControlOx::GetSuitResult(BYTE cbTableCardArray[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_BrandSum], WORD wTableBrandResult[MAX_NIUNIU_ClientSum + 1],UINT64 nAllJettonScore[MAX_NIUNIU_ClientSum])
{
	int nCompareCard[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_ClientSum + 1] = {};//倍率对照表
	BYTE UserCard[MAX_NIUNIU_ClientSum + 1][MAX_NIUNIU_BrandSum] = {};
	BYTE cbSuitStack[MAX_NIUNIU_ClientSum + 1] = {};//排序扑克
	
	for (BYTE cbIndex = 0; cbIndex<MAX_NIUNIU_ClientSum + 1; cbIndex++)//比较牌之间关系
	{
		for (BYTE j = cbIndex; j<MAX_NIUNIU_ClientSum + 1; j++)
		{
			if (wTableBrandResult[j]>wTableBrandResult[cbIndex])
			{
				nCompareCard[j][cbIndex] = g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetRateByBrandValue(wTableBrandResult[j]);
			}
			else
			{
				nCompareCard[j][cbIndex] = -g_FishServer.GetFishConfig().GetFishMiNiGameConfig().niuniuConfig.GetRateByBrandValue(wTableBrandResult[cbIndex]);
			}
			//g_FishServer.GetNiuNiuManager.HandleBrandValue()
			//nCompareCard[j][cbIndex] = m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex], MAX_CARD, m_cbTableCardArray[j], MAX_CARD, cbMultiple);
			//nCompareCard[j][cbIndex] *= cbMultiple;
			nCompareCard[cbIndex][j] = -nCompareCard[j][cbIndex];
		}
	}
	memcpy(UserCard, cbTableCardArray, sizeof(UserCard));
	GetSuitCardCombine(cbSuitStack, nCompareCard, nAllJettonScore);//自动组合
	for (BYTE cbIndex = 0; cbIndex < MAX_NIUNIU_ClientSum + 1; cbIndex++)
	{
		memcpy(cbTableCardArray[cbIndex], UserCard[cbSuitStack[cbIndex]], sizeof(BYTE)*MAX_NIUNIU_BrandSum);
	}
}

//是否是有效组合
bool CServerControlOx::GetSuitCardCombine(BYTE cbStack[], int nTableMutiple[][MAX_NIUNIU_ClientSum + 1], UINT64 nAllJettonScore[])
{
	switch(m_cbControlStyle)
	{
	case CS_BET_AREA:
		{
						AreaWinCard(cbStack, nTableMutiple);
			break;
		}
	case CS_BANKER_WIN:
		{
						  BankerWinCard(true, cbStack, nTableMutiple, nAllJettonScore);
			break;
		}
	case CS_BANKER_LOSE:
		{
						   BankerWinCard(false, cbStack, nTableMutiple, nAllJettonScore);
			break;
		}
	}

	return true;
}

bool CServerControlOx::AreaWinCard(BYTE cbStack[], int nTableMutiple[][MAX_NIUNIU_ClientSum + 1])
{
	bool bIsUser[MAX_NIUNIU_ClientSum+1] = { 0 };	//是否已经使用
	BYTE cbStackCount=0;	//栈中元素数量
	BYTE cbIndex=0;
	
	
	//查找派牌组合
	for(cbIndex=0;cbIndex<=MAX_NIUNIU_ClientSum+1;)
	{
		if(cbIndex<MAX_NIUNIU_ClientSum+1)
		{
			if(bIsUser[cbIndex]) cbIndex++;
			else
			{
				cbStack[cbStackCount]=cbIndex;
				bIsUser[cbIndex]=true;
				cbStackCount++;
				if(cbStackCount==MAX_NIUNIU_ClientSum+1)	//如果已经找到一种组合
				{

					if (m_bWinArea[0] == (nTableMutiple[cbStack[1]][cbStack[0]]>0 ? true : false)		\
						&&m_bWinArea[1] == (nTableMutiple[cbStack[2]][cbStack[0]]>0 ? true : false)	\
						&&m_bWinArea[2] == (nTableMutiple[cbStack[3]][cbStack[0]]>0 ? true : false)	\
						&&m_bWinArea[3] == (nTableMutiple[cbStack[4]][cbStack[0]]>0 ? true : false))
						break;
					cbIndex=MAX_NIUNIU_ClientSum+1;
				}
				else
				{
					cbIndex=0;
					continue;
				}
			}
		}
		else
		{
			if(cbStackCount>0)
			{
				cbIndex=cbStack[--cbStackCount];
				bIsUser[cbIndex]=false;
				cbIndex++;
				continue;
			}
			else break;
		}
	}
	return true;
}

LONGLONG CServerControlOx::GetBankerWinScore(int nWinMultiple[], UINT64 nAllJettonScore[])
{
	//庄家总量
	LONGLONG lBankerWinScore = 0;

	for (BYTE cbIndex = 0; cbIndex<MAX_NIUNIU_ClientSum; cbIndex++)
	{
		lBankerWinScore += nAllJettonScore[cbIndex] * nWinMultiple[cbIndex];
	}
	return lBankerWinScore;
}

void CServerControlOx::BankerWinCard(bool bIsWin, BYTE cbStack[], int nTableMutiple[][MAX_NIUNIU_ClientSum + 1], UINT64 nAllJettonScore[])
{
	bool bIsUser[MAX_NIUNIU_ClientSum+1]={0};	//是否已经使用
	BYTE cbStackCount=0;	//栈中元素数量
	//查找派牌组合
	for (BYTE cbIndex = 0; cbIndex <= MAX_NIUNIU_ClientSum + 1;)
	{
		if(cbIndex<MAX_NIUNIU_ClientSum+1)
		{
			if(bIsUser[cbIndex]) cbIndex++;
			else
			{
				cbStack[cbStackCount]=cbIndex;
				bIsUser[cbIndex]=true;
				cbStackCount++;
				if(cbStackCount==MAX_NIUNIU_ClientSum+1)	//如果已经找到一种组合
				{
					int nWinMultiple[4];
					for(int x=0;x<4;x++)
					{
						nWinMultiple[x] = nTableMutiple[cbStack[x + 1]][cbStack[0]];
					}

					if (bIsWin&&GetBankerWinScore(nWinMultiple, nAllJettonScore)<0)
						break;
					if (!bIsWin&&GetBankerWinScore(nWinMultiple, nAllJettonScore) >= 0)
						break;
					cbIndex=MAX_NIUNIU_ClientSum+1;
				}
				else
				{
					cbIndex=0;
					continue;
				}
			}
		}
		else
		{
			if(cbStackCount>0)
			{
				cbIndex=cbStack[--cbStackCount];
				bIsUser[cbIndex]=false;
				cbIndex++;
				continue;
			}
			else break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
////////////舞会控制
//////////////////////////////////////////////////////////////////////
CServerControlAnimal::CServerControlAnimal()
{
	m_cbExcuteTimes = 0;
	m_cbControlStyle = 0;
	m_cbarea = 0;
}
//舞会
bool  CServerControlAnimal::Control(const void * pDataBuffer)
{
	const CL_CMD_AnimalAdminReq* AdminReq = static_cast<const CL_CMD_AnimalAdminReq*>(pDataBuffer);


	//统一一个结构
	LC_CMD_AnimalAdminReq cResult;
	cResult.SetCmdSize(sizeof(cResult));
	//cResult.SetCmdType((Main_Control << 8) | LC_AnimalAdmin);
	cResult.SetCmdType((Main_Dial << 8) | LC_AnimalAdminContrlBack);
	cResult.ClientID = AdminReq->ClientID;

	cResult.dwUserID = AdminReq->dwUserID;//新添参数   查询玩家id

	cResult.dwUserIDContrl = AdminReq->dwUserIDContrl;//修改添加控制玩家的id


	LogInfoToFile("Control.txt", "CServerControlAnimal控制消息舞会控制消息服务器控制::::%d::::::::::%d:::::::::::::%d:::::::::::%d:::::::%d:::::::::%d::::", AdminReq->dwUserID, AdminReq->dwUserIDContrl, AdminReq->cbReqType, AdminReq->cbExtendData.m_cbControlStyle, AdminReq->cbExtendData.m_cbarea);


	switch (AdminReq->cbReqType)
	{
	case RQ_QUERY_NAMES:
	{
						   g_FishServer.GetDialManager().QueryNames(AdminReq->ClientID, AdminReq->dwUserIDContrl);
						   break;
	}
	case RQ_QUERY_PLAYERJETTON:
	{
								  g_FishServer.GetDialManager().QueryPlayerJetton(AdminReq->dwUserID, AdminReq->ClientID, AdminReq->dwUserIDContrl);
								  break;
	}
	case RQ_QUERY_ALLJETTON:
	{
							   g_FishServer.GetDialManager().QueryAllJetton(AdminReq->ClientID, AdminReq->dwUserIDContrl);
							   break;
	}
	case RQ_RESET_CONTROL:
	{
							 m_cbControlStyle = 0;
							 m_cbExcuteTimes = 0;							
							 cResult.cbResult = CR_ACCEPT;
							 cResult.cbAckType = ACK_RESET_CONTROL;
							// g_FishServer.SendNetCmdToControl(&cResult);

							 g_FishServer.HandleDailContrlBack(&cResult);//发送消息到游戏服务
							 break;
	}
	case RQ_SET_WIN_AREA:
	{

							//const tagAnimalAdminReq*pAdminReq = reinterpret_cast<const tagAnimalAdminReq*>(AdminReq->cbExtendData);
							switch (AdminReq->cbExtendData.m_cbControlStyle)
							{
							case CS_BET_AREA:	//区域设置
							{
													m_cbControlStyle = AdminReq->cbExtendData.m_cbControlStyle;//pAdminReq->m_cbControlStyle;
													m_cbExcuteTimes = AdminReq->cbExtendData.m_cbExcuteTimes; //pAdminReq->m_cbExcuteTimes;
													m_cbarea = AdminReq->cbExtendData.m_cbarea;//pAdminReq->m_cbarea;
													cResult.cbResult = CR_ACCEPT;
													cResult.cbAckType = ACK_SET_WIN_AREA;
													//g_FishServer.SendNetCmdToControl(&cResult);

													g_FishServer.HandleDailContrlBack(&cResult);//发送消息到游戏服务
													break;
							}
							case CS_BANKER_LOSE:	//庄家输牌
							{
														m_cbControlStyle = AdminReq->cbExtendData.m_cbControlStyle;//pAdminReq->m_cbControlStyle;
														m_cbExcuteTimes = AdminReq->cbExtendData.m_cbExcuteTimes;//pAdminReq->m_cbExcuteTimes;
														cResult.cbResult = CR_ACCEPT;
														cResult.cbAckType = ACK_SET_WIN_AREA;
														//g_FishServer.SendNetCmdToControl(&cResult);
														g_FishServer.HandleDailContrlBack(&cResult);//发送消息到游戏服务
														break;
							}
							case CS_BANKER_WIN:		//庄家赢牌
							{
														m_cbControlStyle = AdminReq->cbExtendData.m_cbControlStyle;//pAdminReq->m_cbControlStyle;
														m_cbExcuteTimes = AdminReq->cbExtendData.m_cbExcuteTimes;//pAdminReq->m_cbExcuteTimes;
														cResult.cbResult = CR_ACCEPT;
														cResult.cbAckType = ACK_SET_WIN_AREA;
														//g_FishServer.SendNetCmdToControl(&cResult);
														g_FishServer.HandleDailContrlBack(&cResult);//发送消息到游戏服务
														break;
							}
							default:	//拒绝请求
							{
											cResult.cbResult = CR_REFUSAL;
											cResult.cbAckType = ACK_SET_WIN_AREA;
											//g_FishServer.SendNetCmdToControl(&cResult);

											g_FishServer.HandleDailContrlBack(&cResult);//发送消息到游戏服务
											break;
							}
							}

							break;
	}
	case RQ_PRINT_SYN:
	{
						 cResult.cbResult = CR_ACCEPT;
						 cResult.cbAckType = ACK_PRINT_SYN;

						 cResult.cbExtendData.m_cbControlStyle = m_cbControlStyle;
						 cResult.cbExtendData.m_cbExcuteTimes = m_cbExcuteTimes;
						 cResult.cbExtendData.m_cbarea = m_cbarea;


						 g_FishServer.HandleDailContrlBack(&cResult);//发送消息到游戏服务
						 break;
	}
	default:
	{
			   break;
	}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
//获取玩家控制类型
BYTE CServerControlAnimal::GetContrlStyle()
{
	
	return m_cbControlStyle;
}
//////////////////////////////////////////////////////////////////////////
bool  CServerControlAnimal::NeedControl()//完成控制
{
	return m_cbControlStyle > 0 && m_cbExcuteTimes > 0;
}

bool  CServerControlAnimal::CompleteControl()
{
	if (m_cbExcuteTimes > 0)
		m_cbExcuteTimes--;
	return true;
}

BYTE CServerControlAnimal::GetSuitResult()
{
	return m_cbarea;
}

//////////////////////////////////////////////////////////////////////
////////////车控制
//////////////////////////////////////////////////////////////////////

CServerControlCar::CServerControlCar()
{
	m_cbExcuteTimes = 0;
	m_cbControlStyle = 0;
	m_cbarea = 0;
}
bool  CServerControlCar::Control(const void * pDataBuffer)
{
	const CL_CMD_CarAdminReq* AdminReq = static_cast<const CL_CMD_CarAdminReq*>(pDataBuffer);

	//统一一个结构
	LC_CMD_CarAdminReq cResult;
	cResult.SetCmdSize(sizeof(cResult));
	cResult.SetCmdType((Main_Control << 8) | LC_CarAdmin);
	cResult.ClientID = AdminReq->ClientID;

	switch (AdminReq->cbReqType)
	{
	case RQ_QUERY_NAMES:
	{
						   g_FishServer.GetCarManager().QueryNames(AdminReq->ClientID);
						   break;
	}
	case RQ_QUERY_PLAYERJETTON:
	{
								  g_FishServer.GetCarManager().QueryPlayerJetton((TCHAR*)AdminReq->cbExtendData, AdminReq->ClientID);
								  break;
	}
	case RQ_QUERY_ALLJETTON:
	{
							   g_FishServer.GetCarManager().QueryAllJetton(AdminReq->ClientID);
							   break;
	}
	case RQ_RESET_CONTROL:
	{
							 m_cbControlStyle = 0;
							 m_cbExcuteTimes = 0;
							 cResult.cbResult = CR_ACCEPT;
							 cResult.cbAckType = ACK_RESET_CONTROL;
							 g_FishServer.SendNetCmdToControl(&cResult);
							 break;
	}
	case RQ_SET_WIN_AREA:
	{

							const tagCarAdminReq*pAdminReq = reinterpret_cast<const tagCarAdminReq*>(AdminReq->cbExtendData);
							switch (pAdminReq->m_cbControlStyle)
							{
							case CS_BET_AREA:	//区域设置
							{
													m_cbControlStyle = pAdminReq->m_cbControlStyle;
													m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
													m_cbarea = pAdminReq->m_cbarea;
													cResult.cbResult = CR_ACCEPT;
													cResult.cbAckType = ACK_SET_WIN_AREA;
													g_FishServer.SendNetCmdToControl(&cResult);
													break;
							}
							case CS_BANKER_LOSE:	//庄家输牌
							{
														m_cbControlStyle = pAdminReq->m_cbControlStyle;
														m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
														cResult.cbResult = CR_ACCEPT;
														cResult.cbAckType = ACK_SET_WIN_AREA;
														g_FishServer.SendNetCmdToControl(&cResult);
														break;
							}
							case CS_BANKER_WIN:		//庄家赢牌
							{
														m_cbControlStyle = pAdminReq->m_cbControlStyle;
														m_cbExcuteTimes = pAdminReq->m_cbExcuteTimes;
														cResult.cbResult = CR_ACCEPT;
														cResult.cbAckType = ACK_SET_WIN_AREA;
														g_FishServer.SendNetCmdToControl(&cResult);
														break;
							}
							default:	//拒绝请求
							{
											cResult.cbResult = CR_REFUSAL;
											cResult.cbAckType = ACK_SET_WIN_AREA;
											g_FishServer.SendNetCmdToControl(&cResult);
											break;
							}
							}

							break;
	}
	case RQ_PRINT_SYN:
	{
						 cResult.cbResult = CR_ACCEPT;
						 cResult.cbAckType = ACK_PRINT_SYN;
						 tagCarAdminReq*pAdminReq = reinterpret_cast<tagCarAdminReq*>(cResult.cbExtendData);
						 pAdminReq->m_cbControlStyle = m_cbControlStyle;
						 pAdminReq->m_cbExcuteTimes = m_cbExcuteTimes;
						 pAdminReq->m_cbarea = m_cbarea;
						 g_FishServer.SendNetCmdToControl(&cResult);
						 break;
	}
	default:
	{
			   break;
	}
	}
	return true;
}

bool  CServerControlCar::NeedControl()//完成控制
{
	return m_cbControlStyle > 0 && m_cbExcuteTimes > 0;
}

bool  CServerControlCar::CompleteControl()
{
	if (m_cbExcuteTimes > 0)
		m_cbExcuteTimes--;
	return true;
}

BYTE CServerControlCar::GetSuitResult()
{
	return m_cbarea;
}