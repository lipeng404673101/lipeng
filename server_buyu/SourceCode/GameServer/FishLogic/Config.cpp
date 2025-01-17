#include "stdafx.h"
#include "Config.h"
#if !SIMPLE_SERVER_MODE
#include "..\Role.h"
#include "..\TableManager.h"
#endif
#include"FishScene\FishResManager.h"
#ifndef ASSERT
#define ASSERT assert
#endif
//#include"..\CommonFile\tinyxml\tinyxml.h"
#include"FishScene\tinyxml2.h"
#include"..\FishServer.h"
#include "..\CommonFile\FishServerConfig.h"
//#include "FishServer.h"
const float SPEED_SCALE = 1.0f / 255;
enum CANNON_TYPE
{
	CANNON_1,  //pao1
	CANNON_2,  //pao 2
	CANNON_3,  //pao 3
	CANNON_4,  //bin dong 
	CANNON_5,  //chuan tou
};
//float Con_Energy::s_energyratio = 0;
int Con_Combo::s_sustaintime = 0;
int Con_Combo::s_buff_cycle = 0;
float Con_Combo::s_bufferchance;

float Con_Cannon::s_ratiofactor = 0;
float Con_Cannon::s_maxratio = 0;
float Con_Cannon::s_finalratio = 0;
float Con_Cannon::s_lockratio = 0;

float Con_Fish::s_revies_Activity = 0;
int Con_Fish::s_flashfishid;
int Con_Fish::s_flashfishCatch;
int Con_Fish::s_flashfishradius;

float Con_Rp::s_cycle = 0;
float Con_Rp::s_effecttime = 0;

int Randomtime::s_cycle = 0;
float Randomtime::s_reviseratio1 = 0.0f;
float Randomtime::s_reviseratio2 = 0.0f;

const float PRECISION = 0.0001f;//控制精度
const float ENLARGE = 100000.0f;

int Con_LuckItem::s_threshold;
float Con_LuckItem::s_base;
float Con_LuckItem::s_part;


using namespace tinyxml2;
bool Attribute(const XMLElement*pElement, const char*pName, int&nvalue)
{
	nvalue = 0;
	const char*pItem = pElement->Attribute(pName);
	if (pItem)
	{
		nvalue = atol(pItem);
		return true;
	}
	return false;
}

bool Attribute(const XMLElement*pElement, const char*pName, float&fvalue)
{
	fvalue = 0;
	const char*pItem = pElement->Attribute(pName);
	if (pItem)
	{
		fvalue = static_cast<float>(atof(pItem));
		return true;
	}
	return false;
}


CConfig::CConfig(TableManager *pTableManager)
{
	m_nServerVersion = 0;
	//ZeroMemory(m_nRandomByTime, sizeof(m_nRandomByTime));
	m_pTableManager = pTableManager;
}

CConfig::~CConfig()
{
}

bool CConfig::LoadConfig(char szDir[])
{
	{
		m_Energy.clear();
		m_Skill.clear();
		m_Rate.clear();
		m_Cannon.clear();
		m_Fish.clear();
		m_Rp.clear();
		m_production.clear();
		m_Rank.clear();
		m_PlayTime.clear();
		m_Money.clear();
		//m_vecgood.clear();
		
	}

	tinyxml2::XMLDocument xml_doc;
	char szConfig[MAX_PATH] = { 0 };
	sprintf_s(szConfig, sizeof(szConfig), "%s\\config.xml", szDir);

	if (xml_doc.LoadFile(szConfig) != XML_NO_ERROR) return false;

	XMLElement* xml_element = xml_doc.FirstChildElement("Config");
	if (xml_element == NULL) return false;
	
	const XMLElement* xml_child = NULL;
	for (xml_child = xml_element->FirstChildElement(); xml_child; xml_child = xml_child->NextSiblingElement())
	{
		if (!strcmp(xml_child->Value(), "ServerVersion"))
		{
			Attribute(xml_child, "version", m_nServerVersion);

			int nMaxBullet;
			int nMaxFish;
			Attribute(xml_child, "maxbullet", nMaxBullet);
			Attribute(xml_child, "maxfish", nMaxFish);
		}
		else if (!strcmp(xml_child->Value(), "Combo")) //联合体
		{
			Attribute(xml_child, "sustain_time", Con_Combo::s_sustaintime);
			Attribute(xml_child, "buff_cycle", Con_Combo::s_buff_cycle);
			Attribute(xml_child, "bufferchance", Con_Combo::s_bufferchance);						
		}
		else if (!strcmp(xml_child->Value(), "Energy"))
		{
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Energy  energy_item = { 0 };
				Attribute(xml_data, "id", energy_item.nid);

				Attribute(xml_data, "cditme", energy_item.ncdtime);
				Attribute(xml_data, "mincatch", energy_item.nmincatch);
				Attribute(xml_data, "maxcatch", energy_item.nmaxcatch);
				Attribute(xml_data, "revise", energy_item.nrevise);
				Attribute(xml_data, "energyratio", energy_item.denergyratio);

				Attribute(xml_data, "radius", energy_item.nradius);

				//xml_data->Attribute("playtime", energy_item.nplaytime);
				Attribute(xml_data, "speed", energy_item.npseed);
				Attribute(xml_data, "time1", energy_item.ntime1);
				Attribute(xml_data, "time2", energy_item.ntime2);
				Attribute(xml_data, "time3", energy_item.ntime3);

				m_Energy.push_back(energy_item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Skill"))
		{

			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Skill skill_item = { 0 };
				Attribute(xml_data, "id", skill_item.nid);

				Attribute(xml_data, "cditme", skill_item.ncdtime);
				Attribute(xml_data, "mincatch", skill_item.nmincatch);
				Attribute(xml_data, "maxcatch", skill_item.nmaxcatch);
				Attribute(xml_data, "revise", skill_item.nrevise);

				Attribute(xml_data, "goodsid", skill_item.ngoodsid);
				Attribute(xml_data, "multiple", skill_item.multiple);


				char *pInfo = const_cast<char*>(xml_data->Attribute("goodsconsume"));
				while (pInfo&&*pInfo)
				{
					SkillConsume consume = { 0 };
					consume.byorder = ConvertIntToBYTE(strtol(pInfo, &pInfo, 10));
					consume.byCount = ConvertIntToBYTE(strtol(pInfo, &pInfo, 10));
					skill_item.vecConsume.push_back(consume);
				}

				Attribute(xml_data, "playtime", skill_item.nplaytime);
				Attribute(xml_data, "speed", skill_item.npseed);
				Attribute(xml_data, "time1", skill_item.ntime1);
				Attribute(xml_data, "time2", skill_item.ntime2);
				Attribute(xml_data, "time3", skill_item.ntime3);

				m_Skill.push_back(skill_item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Rate"))
		{
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				//Con_Rate rate_item = { 0 };
				//Attribute(xml_data, "id", rate_item.nid);
				//Attribute(xml_data, "value", rate_item.nValue);
				//Attribute(xml_data, "unlock", rate_item.nUnlock);
				//Attribute(xml_data, "unlockreward", rate_item.unlockreward);
				//Attribute(xml_data, "drop", rate_item.drop);
				//m_Rate.push_back(rate_item);
				//////////////////////////////////////////////////////////////////////////修改
				Con_Rate rate_item = { 0 };
				Attribute(xml_data, "id", rate_item.nid);
				Attribute(xml_data, "value", rate_item.nValue);
				Attribute(xml_data, "unlock", rate_item.nUnlock);
				Attribute(xml_data, "unlockreward", rate_item.unlockreward);
				Attribute(xml_data, "drop", rate_item.drop);
				m_Rate.push_back(rate_item);

				//////////////////////////////////////////////////////////////////////////

			}
		}
		else if (!strcmp(xml_child->Value(), "Cannon"))
		{
			Attribute(xml_child, "ratiofactor", Con_Cannon::s_ratiofactor);
			Attribute(xml_child, "maxratio", Con_Cannon::s_maxratio);
			Attribute(xml_child, "finalration", Con_Cannon::s_finalratio);
			Attribute(xml_child, "lockratio", Con_Cannon::s_lockratio);
			
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Cannon cannon_item = { 0 };
				Attribute(xml_data, "id", cannon_item.nid);

				Attribute(xml_data, "bulletspeed", cannon_item.bulletspeed);		

				Attribute(xml_data, "launcherinterval", cannon_item.dlauncherinterval);

				Attribute(xml_data, "consume", cannon_item.nconsume);
				Attribute(xml_data, "radius", cannon_item.nradius);
				Attribute(xml_data, "catchradius", cannon_item.ncatchradius);
				Attribute(xml_data, "maxcatch", cannon_item.nmaxcatch);

				Attribute(xml_data, "itemid", cannon_item.nItemId);
				Attribute(xml_data, "itemcount", cannon_item.nItemCount);
				Attribute(xml_data, "skill", cannon_item.nskill);			
				//
				Attribute(xml_data, "speed", cannon_item.npseed);
				Attribute(xml_data, "time1", cannon_item.ntime1);
				Attribute(xml_data, "time2", cannon_item.ntime2);
				Attribute(xml_data, "time3", cannon_item.ntime3);

				//读取多次碰撞几率
				char* pCollision = const_cast<char*>(xml_data->Attribute("collisionrate"));
				while (pCollision&&*pCollision)
				{
					cannon_item.vCollision.push_back(static_cast<float>(strtod(pCollision, &pCollision)));
				}
				m_Cannon.push_back(cannon_item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Fish"))
		{
			Attribute(xml_child, "revies_Activity", Con_Fish::s_revies_Activity);
			Attribute(xml_child,"flashfishid", Con_Fish::s_flashfishid);
			Attribute(xml_child,"flashfishCatch", Con_Fish::s_flashfishCatch);
			Attribute(xml_child,"flashfishradius", Con_Fish::s_flashfishradius);

			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Fish fish_item = { 0 };
				Attribute(xml_data, "id", fish_item.nid);
				Attribute(xml_data, "value", fish_item.nvalue);
				Attribute(xml_data, "maxcount", fish_item.maxcount);
				//Attribute(xml_data, "reviseratio", fish_item.reviseratio);
				Attribute(xml_data, "chance", fish_item.chance);
				Attribute(xml_data, "type", fish_item.type);
				Attribute(xml_data, "flashchance", fish_item.flashchance);			

				char* pfishrate = const_cast<char*>(xml_data->Attribute("fishrate"));
				while (pfishrate&&*pfishrate)
				{
					fish_item.fishrate.push_back((float)strtod(pfishrate, &pfishrate));
				}

				m_Fish.push_back(fish_item);

			}
		}
		else if (!strcmp(xml_child->Value(), "Rp"))//抽奖概率
		{
			Attribute(xml_child, "cycle", Con_Rp::s_cycle);
			Attribute(xml_child, "effecttime", Con_Rp::s_effecttime);

			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Rp rp_item = { 0 };
				Attribute(xml_data, "id", rp_item.nid);

				Attribute(xml_data, "rp", rp_item.rp);
				Attribute(xml_data, "reviseratio1", rp_item.reviseratio1);

				if (!Attribute(xml_data, "reviseratio2", rp_item.reviseratio2))//没有默认
				{
					rp_item.reviseratio2 = rp_item.reviseratio1 + PRECISION;
				}
				m_Rp.push_back(rp_item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Randomtime"))
		{
			Attribute(xml_child, "cycle", Randomtime::s_cycle);
			Attribute(xml_child, "reviseratio1", Randomtime::s_reviseratio1);
			Attribute(xml_child, "reviseratio2", Randomtime::s_reviseratio2);
		}
		else if (!strcmp(xml_child->Value(), "Production"))
		{
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Production item = { 0 };
				Attribute(xml_data, "id", item.nid);

				Attribute(xml_data, "income", item.nincome);
				Attribute(xml_data, "reviseratio1", item.reviseratio1);

				if (!Attribute(xml_data, "reviseratio2", item.reviseratio2))//没有默认
				{
					item.reviseratio2 = item.reviseratio1 + PRECISION;
				}
				m_production.push_back(item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Rank"))
		{
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Rank item = { 0 };
				Attribute(xml_data, "id", item.nid);

				item.level = item.nid;
				Attribute(xml_data, "experience", item.experience);
				Attribute(xml_data, "xml_data,reviseratio", item.reviseratio);

				m_Rank.push_back(item);
			}
		}
		else if (!strcmp(xml_child->Value(), "PlayTime"))
		{
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Playtime item = { 0 };
				Attribute(xml_data, "id", item.nid);

				Attribute(xml_data, "time", item.ntime);
				Attribute(xml_data, "reviseratio1", item.reviseratio1);
				if (!Attribute(xml_data, "reviseratio2", item.reviseratio2))//没有默认
				{
					item.reviseratio2 = item.reviseratio1 + PRECISION;
				}
				Attribute(xml_data, "drop", item.drop);
				m_PlayTime.push_back(item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Money"))
		{
			//xml_child->
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Money item = { 0 };

				Attribute(xml_data, "id", item.nid);

				Attribute(xml_data, "money", item.money);
				Attribute(xml_data, "reviseratio1", item.reviseratio1);
				if (!Attribute(xml_data, "reviseratio2", item.reviseratio2))//没有默认
				{
					item.reviseratio2 = item.reviseratio1 + PRECISION;
				}
				m_Money.push_back(item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Luck"))
		{
			Attribute(xml_child,"threshold", Con_LuckItem::s_threshold);
			Attribute(xml_child,"base", Con_LuckItem::s_base);
			Attribute(xml_child,"part", Con_LuckItem::s_part);

			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_LuckItem luck_item = { 0 };
				Attribute(xml_data,"id", luck_item.nid);
				Attribute(xml_data,"nluck", luck_item.nluck);
				Attribute(xml_data,"reviseratio1", luck_item.reviseratio1);
				if (!Attribute(xml_data,"reviseratio2", luck_item.reviseratio2))//没有默认
				{
					luck_item.reviseratio2 = luck_item.reviseratio1 + PRECISION;
				}
				m_VceLuck.push_back(luck_item);
			}
		}
		else if (!strcmp(xml_child->Value(), "Pool"))
		{
			for (const XMLElement* xml_data = xml_child->FirstChildElement(); xml_data; xml_data = xml_data->NextSiblingElement())
			{
				Con_Pool pool_item = { 0 };				
				Attribute(xml_data,"id", pool_item.nid);
				Attribute(xml_data,"pool", pool_item.pool);
				Attribute(xml_data,"reviseratio1", pool_item.reviseratio1);
				Attribute(xml_data,"reviseratio2", pool_item.reviseratio2);

				m_GoldPool.push_back(pool_item);
			}
		}
		else  if (!strcmp(xml_child->Value(), "FishDeadTime"))//
		{
			DeadStayTime temp = { 0 };
			Attribute(xml_child, "TianZai_DouDong", temp.TianZai_DouDong_Time);           //天灾抖动时间

			Attribute(xml_child, "TianZai_Stay_Min", temp.TianZai_Stay_Time1);             //天灾等待陨石降落停留最短时间
			Attribute(xml_child, "TianZai_Stay_Max", temp.TianZai_Stay_Time2);             //天灾等待陨石降落停留最长时间

			Attribute(xml_child, "TianZai_Dead_Min", temp.TianZai_Dead_Time1);             //天灾死亡停留最短时间
			Attribute(xml_child, "TianZai_Dead_Max", temp.TianZai_Dead_Time2);             //天灾死亡停留最长时间

			Attribute(xml_child, "Bullet_BingDong_Dead_Min", temp.Bullet_BingDong_Dead_Time1);     //冰冻炮击中死亡停留最短时间
			Attribute(xml_child, "Bullet_BingDong_Dead_Max", temp.Bullet_BingDong_Dead_Time2);     //冰冻炮击中死亡停留长短时间

			Attribute(xml_child, "BingDong_Dead_Min", temp.BingDong_Dead_Time1);            //冰冻技能死亡最短时间
			Attribute(xml_child, "BingDong_Dead_Max", temp.BingDong_Dead_Time2);            //冰冻技能死亡最长时间

			Attribute(xml_child, "ShanDian_Dead_Min", temp.ShanDian_Dead_Time1);            //闪电技能死亡最短时间
			Attribute(xml_child, "ShanDian_Dead_Max", temp.ShanDian_Dead_Time2);            //闪电技能死亡最长时间

			Attribute(xml_child, "LongJuanFeng_Dead_Min", temp.LongJuanFeng_Dead_Time1);        //龙卷风技能死亡最短时间
			Attribute(xml_child, "LongJuanFeng_Dead_Max", temp.LongJuanFeng_Dead_Time2);        //龙卷风技能死亡最长时间

			Attribute(xml_child, "JiGuang_Stay_Time", temp.JiGuang_Stay_Time);
			Attribute(xml_child, "JiGuang_Dead_Min", temp.JiGuang_Dead_Time1);             //激光大招死亡最短时间
			Attribute(xml_child, "JiGuang_Dead_Max", temp.JiGuang_Dead_Time2);             //激光大招死亡最长时间

			Attribute(xml_child, "ShandDian_Speed", temp.ShandDian_Speed);
			Attribute(xml_child, "JiGuang_Speed", temp.JiGuang_Speed);
			Attribute(xml_child, "JiGuang_BingDong_Speed", temp.JiGuang_BingDong_Speed);
			Attribute(xml_child, "TianZai_Speed", temp.TianZai_Speed);
			Attribute(xml_child, "BingDong_Speed", temp.BingDong_Speed);

			FishResManager::Inst()->SetDeadStayTime(temp);

		}
	}
	if (m_Energy.size() != MAX_LAUNCHER_NUM || m_Cannon.size() != MAX_LAUNCHER_NUM)
	{
		cout << "MAX_LAUNCHER_NUM" << endl;
		return false;
	}
	if (m_Skill.size() != SKILL_MAX)
	{
		cout << "SKILL_MAX" << endl;
		return false;
	}
	/*if (m_Fish.size() != FISH_TYPES)
	{
		cout << "FISH_TYPES" << endl;
		return false;
	}*/	

	sort(m_Energy.begin(), m_Energy.end(), SortByid<Con_Energy>);
	sort(m_Skill.begin(), m_Skill.end(), SortByid<Con_Skill>);
	sort(m_Rate.begin(), m_Rate.end(), SortByid<Con_Rate>);
	sort(m_Cannon.begin(), m_Cannon.end(), SortByid<Con_Cannon>);
	sort(m_Fish.begin(), m_Fish.end(), SortByid<Con_Fish>);
	sort(m_Rp.begin(), m_Rp.end(), SortByid<Con_Rp>);
	sort(m_production.begin(), m_production.end(), SortByid<Con_Production>);
	sort(m_Rank.begin(), m_Rank.end(), SortByid<Con_Rank>);
	sort(m_PlayTime.begin(), m_PlayTime.end(), SortByid<Con_Playtime>);
	sort(m_Money.begin(), m_Money.end(), SortByid<Con_Money>);
	sort(m_VceLuck.begin(), m_VceLuck.end(), SortByid<Con_LuckItem>);
	sort(m_GoldPool.begin(), m_GoldPool.end(), SortByid<Con_Pool>);


	int nFishs = 0;
	int nFishValues = 0;
	for (Fish_Vec::iterator it = m_Fish.begin(); it != m_Fish.end(); it++)
	{
		nFishs += it->maxcount;
		nFishValues += it->maxcount*it->nvalue;
	}

	for (Energy_Vec::iterator it = m_Energy.begin(); it != m_Energy.end(); it++)
	{
		Con_Energy &item = *it;
		item.nincome = ConvertFloatToInt((item.nmincatch + item.nmaxcatch) / 2.0f / nFishs*nFishValues + item.nrevise);
		item.nthreshold = ConvertFloatToInt(item.nincome*item.denergyratio);//大招
	}

	for (Skill_Vec::iterator it = m_Skill.begin(); it != m_Skill.end(); it++)
	{
		Con_Skill &item = *it;
		item.nincome = ConvertFloatToInt((item.nmincatch + item.nmaxcatch) / 2.0f / nFishs*nFishValues + item.nrevise);
	}

	//read goods
	//if (!xml_doc.LoadFile(szGood, TIXML_ENCODING_UTF8)) return false;
	//xml_element = xml_doc.FirstChildElement("Goods");
	//if (xml_element == NULL) return false;
	//for (xml_child = xml_element->FirstChildElement(); xml_child; xml_child = xml_child->NextSiblingElement())
	//{
	//	FISH_GOODS item = { 0 };
	//	xml_child->Attribute("id", &item.nid);
	//	//xml_child->Attribute("name", item.chname);
	//	xml_child->Attribute("price", &item.nprice);
	//	
	//	m_vecgood.push_back(item);
	//}
	//sort(m_vecgood.begin(), m_vecgood.end(), SortByid<FISH_GOODS>);

	return true;
}

//pei zhi
UINT  CConfig::GetVersion()
{
	return m_nServerVersion;
}

float CConfig::BulletRadius(USHORT bulletID)				//子弹基本半径
{
	ASSERT(bulletID < m_Cannon.size());
	return m_Cannon[bulletID].nradius;
}

float CConfig::BulletCatchRadius(USHORT bulletID)					//子弹碰撞之后的捕获半径
{
	ASSERT(bulletID < m_Cannon.size());
	return m_Cannon[bulletID].ncatchradius;
}

UINT  CConfig::MaxCatch(USHORT bulletID)				//子弹的最大捕鱼数量
{
	ASSERT(bulletID < m_Cannon.size());
	return m_Cannon[bulletID].nmaxcatch;
}

float CConfig::CatchChance(PlayerID player, USHORT bulletId, byte bulletType, byte fishType, int byOrder, int byMaxCatch, BYTE byPackageType,bool bLocked)
{
#if SIMPLE_SERVER_MODE
	return 0.5f;
#else

	ASSERT(bulletType < m_Cannon.size());
	ASSERT(fishType<m_Fish.size());
	ASSERT(ConvertIntToDWORD(byOrder) < m_Cannon[bulletType].vCollision.size()/2);

	if (bulletType >= m_Cannon.size())
	{
		return 0;
	}
	if (fishType >= m_Fish.size())
	{
		return 0;
	}
	if (ConvertIntToDWORD(byOrder) >= m_Cannon[bulletType].vCollision.size()/2)
	{
		return 0;
	}

	CRole *pPlayer = m_pTableManager->SearchUser(player);
	if (pPlayer)
	{
		if (pPlayer->IsComboBuff(bulletId))
		{
			if (g_FishServer.GetFishConfig().GetChestConfig().ChestMap.find(byPackageType) == g_FishServer.GetFishConfig().GetChestConfig().ChestMap.end())
			{
				return Con_Combo::s_bufferchance;				
			}
		}

		float fbase = CalBaseRatio(bulletType, fishType);//基础概率
		//////////////////////////////////////////////////////////////////////////
		//  需要修改  
		if (m_pTableManager->RNumrateF != 0 && m_pTableManager->RNumrateF <= 100 && m_pTableManager->RNumrateS<=100)
		{
			fbase = m_pTableManager->RNumrateF;
			Con_Cannon::s_finalratio = 100;//m_pTableManager->RNumrateS;//最大值
			//LogInfoToFile("Control.txt", "CatchChance捕鱼概率修改后的:::::::%f:::::::::::%f::::::::::::::::::::", fbase, Con_Cannon::s_finalratio);

		}		
		//////////////////////////////////////////////////////////////////////////添加特殊鱼的特殊概率
		//if (m_pTableManager->KuCunNum<0)
		//{
		//	fbase = fbase / 2;
		//}
		//else if (m_pTableManager->KuCunNum>)
		//{
		//}
//////////////////////////////////////////////////////////////////////////
		float fRandTime = pPlayer->RandTimeRate(Randomtime::s_reviseratio1, Randomtime::s_reviseratio2, fishType);
		float fLockRate = bLocked ? Con_Cannon::s_lockratio : 0;//锁定增加概率

		int64 nPoolGold;//奖池   
		int nLuck = pPlayer->GetRoleLuckyValue();		
		//////////////////////////////////////////////////////////////////////////
		if (m_pTableManager->QueryPool(pPlayer->GetTableID(), nPoolGold) && nLuck!=0)//奖池触发,有幸运值
		{						
			float fluck = LuckRate(nLuck);
			fbase *= (1 + fRandTime + fluck + fLockRate);
		}
		else
		{
			//float frp = RpRate(pPlayer->GetRp());
			//float fProduction = ProductionRate(pPlayer->GetProduction());
			//float fMoney = MoneyRate(pPlayer->GetScore());			
			float fRank = RankRate(pPlayer->GetLevel());//等级
			float fGameTime = GameTimeRate(pPlayer->GetGameTime());		//游戏时间	
			float fPool = PoolRate((int)nPoolGold / pPlayer->TableRate());//库存   改变捕鱼概率
			if (pPlayer->GetRoleExInfo() && pPlayer->GetRoleExInfo()->GetRoleMonth().IsInMonthTable())//比赛场库存不影响概率
			{
				fPool=0;
			}
			fbase *= (1 + fRandTime + fRank + fGameTime + fPool + fLockRate);//概率的计算

	/*		{
				char szKey[20] = { 0 };
				char szValue[128] = { 0 };
				char szFile[128] = { 0 };
				sprintf_s(szKey, sizeof(szKey), "%d",timeGetTime());

				sprintf_s(szValue, sizeof(szValue), "%d,rp%f,randtime%f,production%f,rank%f,gametime%f,money%f,Pool%f,base%f", fishType, frp, fRandTime, fProduction, fRank, fGameTime, fMoney, fPool, fbase);
				sprintf_s(szFile, sizeof(szFile), "%s\\%d.txt", g_FishServerConfig.GetFTPServerConfig()->FTPFilePath, pPlayer->GetRoleExInfo()->GetRoleInfo().dwUserID);
				WritePrivateProfileStringA("fish", szKey, szValue, szFile);
			}*/
		}
		byte byIndex = 2 * byOrder;			
		if (!pPlayer->HaveNormalLauncher(bulletType))//临时炮
		{
			byIndex += 1;
		}
		fbase *= m_Cannon[bulletType].vCollision[byIndex];

		//fbase *= 10;//修改概率

		//fbase *= PackageFactor(byPackageType);//宝箱  宝箱概率

		fbase = min(fbase, Con_Cannon::s_finalratio);
		//////////////////////////////////////////////////////////////////////////
		int64 Tablerate = 1;
		WORD   TableIDShu = 1;//g_FishServer.GetTableManager()->GetTable(GetTableID());
		float	Index = 0;

		GameTable* pTable = g_FishServer.GetTableManager()->GetTable(pPlayer->GetTableID());
		if (!pTable)
		{
			return 0;
		}
		double   XuanZeBeilv = (BulletConsume(pPlayer->GetLauncherType())*BulletMultiple(pPlayer->GetRateIndex()));
		//LogInfoToFile("Control.txt", "看看桌子:::::::%f:::::::::::%u::::::::::%u::::::%d::::", fbase, m_pTableManager->KucunNum[pPlayer->GetTableID()], TableIDShu, pPlayer->GetTableID());
		if (pTable->GetTableTypeID() == 0)
		{
			XuanZeBeilv *= 0.001f;
			Index = 0.2f;
			Tablerate = 1;
			//fbase = fbase*1.1f;
			//10000000正一万  负2000
			if (m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-2000000 )
			{
				fbase *= RandFloat()*0.00001f;

				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()]>=-2000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1800000)
			{
				fbase *= RandFloat()*0.0001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()]>=-1800000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1600000)
			{
				fbase *= RandFloat()*0.0005f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()]>=-1600000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1400000)
			{
				fbase *= RandFloat()*0.001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()]>=-1400000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1200000)
			{
				fbase *= RandFloat()*0.002f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -1200000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1000000)
			{
				fbase *= RandFloat()*0.003f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -1000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-800000)
			{
				fbase *= RandFloat()*0.004f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -800000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-600000)
			{
				fbase *= RandFloat()*0.005f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -600000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-400000)
			{
				fbase *= RandFloat()*0.016f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -400000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-200000)
			{
				fbase *= RandFloat()*0.017f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -200000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-100000)
			{
				fbase *= RandFloat()*0.018f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -100000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-50000)
			{
				fbase *= RandFloat()*0.019f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -50000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<0)
			{
				fbase *= RandFloat()*0.12f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 0 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<10000)
			{
				fbase *= RandFloat()*0.65f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 10000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<50000)
			{
				fbase *= RandFloat()*0.82f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 50000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<100000)
			{
				fbase *= RandFloat()*0.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 100000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<500000)
			{
				fbase *= RandFloat()*1.35f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 500000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<1500000)
			{
				fbase *= RandFloat()*1.45f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 1500000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<3000000)
			{
				fbase *= RandFloat()*1.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 3000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<4000000)
			{
				fbase *= RandFloat()*1.65f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 4000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<5000000)
			{
				fbase *= RandFloat()*1.75f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 5000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<6000000)
			{
				fbase *= RandFloat()*1.85f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 6000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<7000000)
			{
				fbase *= RandFloat()*1.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 7000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<8000000)
			{
				fbase *= RandFloat()*2.05f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 8000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<9000000)
			{
				fbase *= RandFloat()*2.15f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 9000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<10000000)
			{
				fbase *= RandFloat()*2.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 10000000 )
			{
				fbase *= RandFloat()*5.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}
			//LogInfoToFile("Control.txt", "看看桌子1:::::::%f:::::::::::%u::::::::::%d::::::%u::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], pTable->GetTableTypeID(), Tablerate);
		}
		else if (pTable->GetTableTypeID() == 1)
		{
			XuanZeBeilv *= 0.0001f;
			Index = 10;
			Tablerate = 10;
			//zheng正2万 负5000
			//10000000正一万  负2000
			if (m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-5000000)
			{
				fbase *= RandFloat()*0.00001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -5000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-4800000)
			{
				fbase *= RandFloat()*0.0001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -4800000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-3600000)
			{
				fbase *= RandFloat()*0.0005f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -3600000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-2400000)
			{
				fbase *= RandFloat()*0.001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -2400000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1200000)
			{
				fbase *= RandFloat()*0.002f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -1200000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1000000)
			{
				fbase *= RandFloat()*0.003f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -1000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-800000)
			{
				fbase *= RandFloat()*0.004f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -800000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-600000)
			{
				fbase *= RandFloat()*0.005f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -600000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-400000)
			{
				fbase *= RandFloat()*0.016f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -400000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-200000)
			{
				fbase *= RandFloat()*0.017f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -200000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-100000)
			{
				fbase *= RandFloat()*0.018f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -100000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-50000)
			{
				fbase *= RandFloat()*0.019f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -50000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<0)
			{
				fbase *= RandFloat()*0.12f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 0 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<100000)
			{
				fbase *= RandFloat()*0.65f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 100000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<500000)
			{
				fbase *= RandFloat()*0.82f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 500000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<1000000)
			{
				fbase *= RandFloat()*0.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 1000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<3000000)
			{
				fbase *= RandFloat()*1.35f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 3000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<5000000)
			{
				fbase *= RandFloat()*1.45f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 5000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<7000000)
			{
				fbase *= RandFloat()*1.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 7000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<10000000)
			{
				fbase *= RandFloat()*1.65f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 10000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<12000000)
			{
				fbase *= RandFloat()*1.75f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 12000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<14000000)
			{
				fbase *= RandFloat()*1.85f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 14000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<16000000)
			{
				fbase *= RandFloat()*1.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 16000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<18000000)
			{
				fbase *= RandFloat()*2.05f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 18000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<19000000)
			{
				fbase *= RandFloat()*2.15f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 19000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<20000000)
			{
				fbase *= RandFloat()*2.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 20000000)
			{
				fbase *= RandFloat()*5.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}
			//fbase = fbase*0.99;
			//LogInfoToFile("Control.txt", "看看桌子2:::::::%f:::::::::::%ld::::::::::%d::::::%u::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], pTable->GetTableTypeID(), Tablerate);
		}
		else if (pTable->GetTableTypeID() == 2)
		{
			XuanZeBeilv *= 0.0001f;
			Index = 60;
			Tablerate = 90;
			///正3万  负1万
			//10000000正一万  负2000
			if (m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-10000000)
			{
				fbase *= RandFloat()*0.00001f;

				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -10000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-8000000)
			{
				fbase *= RandFloat()*0.0001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -8000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-6000000)
			{
				fbase *= RandFloat()*0.0005f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -6000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-4000000)
			{
				fbase *= RandFloat()*0.001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -4000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-2000000)
			{
				fbase *= RandFloat()*0.002f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -2000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1000000)
			{
				fbase *= RandFloat()*0.003f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -1000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-800000)
			{
				fbase *= RandFloat()*0.004f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -800000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-600000)
			{
				fbase *= RandFloat()*0.005f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -600000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-400000)
			{
				fbase *= RandFloat()*0.016f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -400000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-200000)
			{
				fbase *= RandFloat()*0.017f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -200000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-100000)
			{
				fbase *= RandFloat()*0.018f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -100000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-50000)
			{
				fbase *= RandFloat()*0.019f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -50000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<0)
			{
				fbase *= RandFloat()*0.22f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 0 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<10000)
			{
				fbase *= RandFloat()*0.65f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 10000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<50000)
			{
				fbase *= RandFloat()*0.82f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 50000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<100000)
			{
				fbase *= RandFloat()*0.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 100000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<300000)
			{
				fbase *= RandFloat()*1.15f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 300000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<500000)
			{
				fbase *= RandFloat()*1.20f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 500000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<1000000)
			{
				fbase *= RandFloat()*1.25f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 1000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<2000000)
			{
				fbase *= RandFloat()*1.23f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 2000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<3000000)
			{
				fbase *= RandFloat()*1.30f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 3000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<4000000)
			{
				fbase *= RandFloat()*1.35f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 4000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<5000000)
			{
				fbase *= RandFloat()*1.40f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 5000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<8500000)
			{
				fbase *= RandFloat()*1.45f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 8500000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<10000000)
			{
				fbase *= RandFloat()*1.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 10000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<12000000)
			{
				fbase *= RandFloat()*1.65f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 12000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<15000000)
			{
				fbase *= RandFloat()*1.75f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 15000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<20000000)
			{
				fbase *= RandFloat()*1.85f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 20000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<23000000)
			{
				fbase *= RandFloat()*1.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 23000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<26000000)
			{
				fbase *= RandFloat()*2.05f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 26000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<29000000)
			{
				fbase *= RandFloat()*2.15f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 29000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<30000000)
			{
				fbase *= RandFloat()*2.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 30000000)
			{
				fbase *= RandFloat()*5.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}
			//fbase = fbase*0.98;
			//LogInfoToFile("Control.txt", "看看桌子3:::::::%f:::::::::::%u::::::::::%d::::::%u::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], pTable->GetTableTypeID(), Tablerate);
		}
		else if (pTable->GetTableTypeID() == 3)
		{
			XuanZeBeilv *= 0.00001f;
			Index = 900;
			Tablerate = 320;
			//正5万  负2万
			//10000000正一万  负2000
			if (m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-20000000)
			{
				fbase *= RandFloat()*0.000001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -20000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-18000000)
			{
				fbase *= RandFloat()*0.00001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -18000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-16000000)
			{
				fbase *= RandFloat()*0.00002f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -16000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-14000000)
			{
				fbase *= RandFloat()*0.00004f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -14000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-12000000)
			{
				fbase *= RandFloat()*0.00006f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -12000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-10000000)
			{
				fbase *= RandFloat()*0.00008f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -10000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-8000000)
			{
				fbase *= RandFloat()*0.0001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -8000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-6000000)
			{
				fbase *= RandFloat()*0.0002f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}
			else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -6000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-4000000)
			{
				fbase *= RandFloat()*0.0004f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -4000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-2000000)
			{
				fbase *= RandFloat()*0.0005f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -2000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1400000)
			{
				fbase *= RandFloat()*0.001f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -1400000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1200000)
			{
				fbase *= RandFloat()*0.002f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -1200000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-1000000)
			{
				fbase *= RandFloat()*0.003f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -1000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-800000)
			{
				fbase *= RandFloat()*0.004f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -800000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-600000)
			{
				fbase *= RandFloat()*0.005f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -600000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-400000)
			{
				fbase *= RandFloat()*0.016f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;
			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -400000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-200000)
			{
				fbase *= RandFloat()*0.017f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -200000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-100000)
			{
				fbase *= RandFloat()*0.018f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -100000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<-50000)
			{
				fbase *= RandFloat()*0.019f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= -50000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<0)
			{
				fbase *= RandFloat()*0.12f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 0 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<10000)
			{
				fbase *= RandFloat()*0.65f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 10000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<50000)
			{
				fbase *= RandFloat()*0.82f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 50000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<100000)
			{
				fbase *= RandFloat()*0.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 100000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<500000)
			{
				fbase *= RandFloat()*1.35f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 500000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<1500000)
			{
				fbase *= RandFloat()*1.45f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 1500000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<3000000)
			{
				fbase *= RandFloat()*1.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 3000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<4000000)
			{
				fbase *= RandFloat()*1.65f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 4000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<5000000)
			{
				fbase *= RandFloat()*1.75f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 5000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<6000000)
			{
				fbase *= RandFloat()*1.85f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 6000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<7000000)
			{
				fbase *= RandFloat()*1.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 7000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<8000000)
			{
				fbase *= RandFloat()*2.05f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 8000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<9000000)
			{
				fbase *= RandFloat()*2.15f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 9000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<10000000)
			{
				fbase *= RandFloat()*2.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 10000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<15000000)
			{
				fbase *= RandFloat()*2.75f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 15000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<20000000)
			{
				fbase *= RandFloat()*2.85f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 20000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<25000000)
			{
				fbase *= RandFloat()*2.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 25000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<30000000)
			{
				fbase *= RandFloat()*3.15f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 30000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<35000000)
			{
				fbase *= RandFloat()*3.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 35000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<40000000)
			{
				fbase *= RandFloat()*3.75f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 40000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<45000000)
			{
				fbase *= RandFloat()*3.95f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 45000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<49000000)
			{
				fbase *= RandFloat()*4.05f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 49000000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<50000000)
			{
				fbase *= RandFloat()*4.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= 50000000)
			{
				fbase *= RandFloat()*5.55f;
				fbase = fbase + fbase*RandFloat()*0.00001f*XuanZeBeilv;

			}
			//fbase = fbase*0.95;
			//LogInfoToFile("Control.txt", "看看桌子4:::::::%f:::::::::::%u::::::::::%d::::::%u::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], pTable->GetTableTypeID(), Tablerate);

		}
		//LogInfoToFile("Control.txt", "看看配置炮的大小:::::::::::::::%d::::::%d::::", BulletConsume(pPlayer->GetLauncherType()), BulletMultiple(pPlayer->GetRateIndex()));

		//double   XuanZeBeilv = (BulletConsume(pPlayer->GetLauncherType())*BulletMultiple(pPlayer->GetRateIndex()));
		//float   XuanZeBeilv11 = 0.02f;//XuanZeBeilv * 1;
		//float   XuanZeBeilv12 = 0.003f;
		//float   XuanZeBeilv13 = 0.0005f;
		//double   Num = rand() % 650 + 100;
		//XuanZeBeilv = Index*Num;
		////LogInfoToFile("Control.txt", "CatchChance相应参数::::%d::::::%d:::::%ld:::", XuanZeBeilv, XuanZeBeilv11, m_pTableManager->KucunNum[pTable->GetTableTypeID()]);

		////if (XuanZeBeilv<)

		//float	suiji = 0;//随机概率
		//suiji = RandFloat()* 10;
		////if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] <= -200000)
		////{

		////	fbase = 0.00001f;//修改概率
		////}

		////if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] <= -100000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]>-200000)
		////{			

		////	fbase *= RandFloat()*0.002f;//修改概率
		////}

		////if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] <= -50000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]>-100000)
		////{

		////	fbase *= RandFloat()*0.005f;//修改概率
		////}

		////if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] <= -5000 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]>-50000)
		////{

		////	fbase *= RandFloat()*0.009f;//修改概率
		////}

		//if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] <= 0)
		//{

		//	fbase = 0;
		//}
		//////库存变化  概率变化 
		//if (m_pTableManager->KucunNum[pTable->GetTableTypeID()]>0 && m_pTableManager->KucunNum[pTable->GetTableTypeID()]<5000)
		//{
		//	fbase *= RandFloat()*0.01f;
		//}
		////if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] <=5000)
		////{
		////	
		////	fbase *=0.01f;//修改概率
		////}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (10 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (5 * XuanZeBeilv))
		//{
		//	//fbase *= (1 - suiji * XuanZeBeilv11);
		//	fbase = fbase - fbase*(suiji*XuanZeBeilv11);

		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.001);

		//	//LogInfoToFile("Control.txt", "CatchChance库存的变化5555555555555:::::::%f:::::::::::%u::::::::::%u::::::%d::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], Tablerate, pTable->GetTableTypeID());
		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (20 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (10 * XuanZeBeilv))
		//{
		//	//fbase = fbase *(1 - suiji * XuanZeBeilv11);

		//	fbase = fbase - fbase*(suiji*XuanZeBeilv11);
		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.005);
		//	//LogInfoToFile("Control.txt", "CatchChance库存的变化444444444444:::::::%f:::::::::::%u::::::::::%u::::::%d::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], Tablerate, pTable->GetTableTypeID());

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (40 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (20 * XuanZeBeilv))
		//{

		////	fbase = fbase *(1 - suiji * XuanZeBeilv11);

		//	fbase = fbase - fbase*(suiji*XuanZeBeilv11);
		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//	//	LogInfoToFile("Control.txt", "CatchChance库存的变化66666666666666:::::::%f:::::::::::%u::::::::::%u::::::%d::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], Tablerate, pTable->GetTableTypeID());
		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (80 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (40 * XuanZeBeilv))
		//{
		//	//fbase = fbase *(1 - suiji * XuanZeBeilv11);

		//	fbase = fbase - fbase*(suiji*XuanZeBeilv11);
		////	m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);
		//	//LogInfoToFile("Control.txt", "CatchChance库存的变化22222222222222:::::::%f:::::::::::%u::::::::::%u::::::%d::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], Tablerate, pTable->GetTableTypeID());
		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (120 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (80 * XuanZeBeilv))
		//{
		//	//fbase = fbase *(1 + 1 / XuanZeBeilv);
		//	//fbase *= (1 - suiji * XuanZeBeilv11);

		//	fbase = fbase - fbase*(suiji*XuanZeBeilv11);

		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] /= 0.7;
		//	//LogInfoToFile("Control.txt", "CatchChance库存的变化88888888:::::::%f:::::::::::%u::::::::::%u::::::%d::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], Tablerate, pTable->GetTableTypeID());


		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (160 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (120 * XuanZeBeilv))
		//{
		//	suiji = RandFloat()* 100;
		////	fbase *= (1 - suiji * (XuanZeBeilv12));
		//	fbase = fbase - fbase*(suiji*XuanZeBeilv12);

		////	m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0002);
		//	//LogInfoToFile("Control.txt", "CatchChance库存的变化99999999:::::::%f:::::::::::%u::::::::::%u::::::%d::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], Tablerate, pTable->GetTableTypeID());
		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (250 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (160 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 100;
		//	//fbase *= (1 - suiji * XuanZeBeilv12);

		//	fbase = fbase - fbase*(suiji*XuanZeBeilv12);
		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0002);
		//	//LogInfoToFile("Control.txt", "CatchChance库存的变化333333333:::::::%f:::::::::::%u::::::::::%u::::::%d::::", fbase, m_pTableManager->KucunNum[pTable->GetTableTypeID()], Tablerate, pTable->GetTableTypeID());
		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (340 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (250 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 100;
		//	//fbase *= (1 - suiji * XuanZeBeilv12);

		//	fbase = fbase - fbase*(suiji*XuanZeBeilv12);
		////	m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0002);
		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (340 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (430 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 100;
		//	//fbase *= (1 - suiji * XuanZeBeilv12);
		//	fbase = fbase - fbase*(suiji*XuanZeBeilv12);
		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0002);

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (430 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (520 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 100;
		//	//fbase *= (1 - suiji* XuanZeBeilv12);
		//	fbase = fbase - fbase*(suiji*XuanZeBeilv12);
		////	m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (520 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (610 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 1000;
		//	//fbase *= (1 + suiji* XuanZeBeilv13);

		//	fbase = fbase + fbase*(suiji*XuanZeBeilv13);
		////	m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (610 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (700 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 1000;
		//	//fbase *= (1 + suiji * XuanZeBeilv13);
		//	fbase = fbase + fbase*(suiji*XuanZeBeilv13);
		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (700 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (790 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 1000;
		//	//fbase *= (1 + suiji * XuanZeBeilv13);
		//	fbase = fbase + fbase*(suiji*XuanZeBeilv13);
		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (790 * XuanZeBeilv) && m_pTableManager->KucunNum[pTable->GetTableTypeID()] < (860 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 1000;
		////	fbase *= (1 + suiji * XuanZeBeilv13);

		//	fbase = fbase + fbase*(suiji*XuanZeBeilv13);
		//	//m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (860 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 1000;
		////	fbase *= (1 + suiji*0.001);
		//	fbase = fbase + fbase*(suiji*0.001f);
		//	m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (1000 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 1000;
		//	//fbase *= (1 + suiji*0.002);

		//	fbase = fbase + fbase*(suiji*0.002f);
		//	m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (2000 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 1000;
		//	//fbase *= (1 + suiji*0.005);

		//	fbase = fbase + fbase*(suiji*0.05f);
		//	m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0001);

		//}
		//else if (m_pTableManager->KucunNum[pTable->GetTableTypeID()] >= (3000 * XuanZeBeilv))
		//{
		//	suiji = RandFloat() * 1000;
		//	//fbase *= (1 + suiji*0.01);
		//	fbase = fbase + fbase*(suiji*0.1f);
		//	m_pTableManager->KucunNum[pTable->GetTableTypeID()] = m_pTableManager->KucunNum[pTable->GetTableTypeID()] * (1 - 0.0005);

		//}

		//////////////////////////////////////////////////////////////////////////
		//添加控制概率
		//if (m_pTableManager->OnChangeGailv(player))
		//{
		//	
		//	fbase=
		//}
		//m_pTableManager->RNumrateF

		//LogInfoToFile("Control.txt", "CatchChance捕鱼概率:::::::%f:::::::::::%f::::::::::::::::::::", fbase, Con_Cannon::s_finalratio);
		//60 80 100 150 200 250 300 500 600 700			23  0  4  13  22  23
		Index = 1;
		float SuiJIShu = RandFloat() + 0.9f;

		float SuiJIShu1 = RandFloat()*50 + 4;
		float SuiJIShu2 = RandFloat()*80 + 10;

		if (fishType == 8)
		{
			fbase = ((fbase*Index) / SuiJIShu)*1.5f;
		}
		else if (fishType == 7)
		{
			fbase = ((fbase*Index) / SuiJIShu)*1.2f;
		}
		else if (fishType == 15)
		{
			fbase = ((fbase*Index) / SuiJIShu)*1.2f;
		}
		else if (fishType == 21)
		{
			fbase =( (fbase*Index) / SuiJIShu);
		}
		else if (fishType == 5)
		{
			fbase = ((fbase*Index) / SuiJIShu);
		}
		else if (fishType == 2)
		{
			fbase = (fbase*Index) / SuiJIShu;
		}
		else if (fishType == 11)
		{
			fbase = (fbase*Index) / SuiJIShu1;
		}
		else if (fishType == 14)
		{
			
			fbase = (fbase*Index) / SuiJIShu1;
		}
		else if (fishType == 4)
		{
			fbase = (fbase*Index) / SuiJIShu1;
			
		}
		else if (fishType == 17)
		{
			fbase = (fbase*Index) / SuiJIShu1;
			
		}
		else if (fishType == 12)
		{
			fbase = (fbase*Index) / SuiJIShu1;
			
		}
		else if (fishType == 0)
		{
			fbase = (fbase*Index) / SuiJIShu1;
			
		}
		else if (fishType == 6)
		{
			fbase = (fbase*Index) / SuiJIShu1;
			
		}
		else if (fishType == 13)
		{
			fbase = (fbase*Index) / SuiJIShu1;

		}
		else if (fishType == 22)
		{
			fbase = (fbase*Index) / SuiJIShu1;

		}
		else if (fishType == 23)
		{
			fbase = (fbase*Index) / SuiJIShu1;

		}
		else if (fishType == 16)
		{
			fbase = (fbase*Index) / SuiJIShu1;

		}
		else if (fishType == 10)
		{
			fbase = (fbase*Index) / SuiJIShu2;

		}
		else if (fishType == 9)
		{
			fbase = (fbase*Index) / SuiJIShu2;

		}
		else if (fishType == 18)
		{
			fbase = (fbase*Index) / SuiJIShu2;

		}
		else if (fishType == 20)
		{
			fbase = (fbase*Index) / SuiJIShu2;

		}
		else if (fishType == 1)
		{
			fbase = ((fbase*Index) / SuiJIShu2)*0.5f;

		}
		else if (fishType == 3)
		{
			fbase = ((fbase*Index) / SuiJIShu2)*0.3f;

		}
		else if (fishType == 19)
		{
			fbase = ((fbase*Index) / SuiJIShu2)*0.2f;

		}
		else if (fishType == 25)
		{
			fbase = (fbase*Index) / SuiJIShu2;

		}

		
		//pPlayer->m_pRoleEx->IsRobot();
		//机器人概率加大
		if (pPlayer->GetRoleExInfo()->IsRobot())
		{
			fbase *= 2;
			if (pPlayer->GetScore() < 1000)
			{
				fbase = fbase + fbase*2.1f;
			}
			else if (pPlayer->GetScore() < 3000 && pPlayer->GetScore()>1000)
			{
				fbase = fbase + fbase*1.5f;
			}
			else if (pPlayer->GetScore() < 5000 && pPlayer->GetScore()>3000)
			{
				fbase = fbase + fbase*1.4f;
			}
			else if (pPlayer->GetScore() < 10000 && pPlayer->GetScore()>5000)
			{
				fbase = fbase + fbase*1.3f;
			}
			else if (pPlayer->GetScore() < 25000 && pPlayer->GetScore()>10000)
			{
				fbase = fbase + fbase*1.2f;
			}
			else if (pPlayer->GetScore() < 50000 && pPlayer->GetScore()>25000)
			{
				fbase = fbase + fbase*1.1f;
			}
			else if (pPlayer->GetScore() < 70000 && pPlayer->GetScore()>50000)
			{
				fbase = fbase + fbase*1.0f;
			}
			


		}
		else
		{
			//玩家金币  
			if (pPlayer->GetScore() < 1000)
			{
				fbase = fbase + fbase*0.9f;
			}
			else if (pPlayer->GetScore() < 3000 && pPlayer->GetScore()>1000)
			{
				fbase = fbase + fbase*0.5f;
			}

			//玩家支出和收入
			//int64 ChangeNum = pPlayer->m_nPay - pPlayer->m_nRevenue;
			////如果正数那就是  支出大于收入  玩家输钱

			////王者娱乐的

			//if (ChangeNum<-100000)//200
			//{
			//	fbase = fbase + fbase*0.5f;
			//}
			//if (ChangeNum>200000)
			//{
			//	fbase = fbase - fbase*0.4f;
			//}

			///////////////////////////////////////////
			//if (ChangeNum<-200000)//200
			//{
			//	fbase = fbase + fbase*1.4f;
			//}
			//else if (ChangeNum >-150000 && ChangeNum<-100000)
			//{
			//	fbase = fbase + fbase*1.3f;
			//}
			//else if (ChangeNum >-100000 && ChangeNum<-80000)
			//{
			//	fbase = fbase + fbase*1.2f;
			//}
			//else if (ChangeNum >-80000 && ChangeNum<-50000)
			//{
			//	fbase = fbase + fbase*1.1f;
			//}
			//else if (ChangeNum >-50000 && ChangeNum<-30000)
			//{
			//	fbase = fbase + fbase*1.0f;
			//}
			//else if (ChangeNum >-30000 && ChangeNum<-10000)
			//{
			//	fbase = fbase + fbase*0.9f;
			//}
			//else if (ChangeNum >-10000 && ChangeNum<0)//输钱
			//{
			//	fbase = fbase + fbase*0.8f;
			//}
			//else if (ChangeNum >0 && ChangeNum<10000)//赢钱
			//{
			//	fbase = fbase - fbase*0.001f;
			//}
			//else if (ChangeNum >10000 && ChangeNum<20000)
			//{
			//	fbase = fbase - fbase*0.002f;
			//}
			//else if (ChangeNum >20000 && ChangeNum<50000)
			//{
			//	fbase = fbase - fbase*0.01f;
			//}
			//else if (ChangeNum >50000 && ChangeNum<100000)
			//{
			//	fbase = fbase - fbase*0.05f;
			//}
			//else if (ChangeNum >100000 && ChangeNum<150000)
			//{
			//	fbase = fbase - fbase*0.08f;
			//}
			//else if (ChangeNum >150000 && ChangeNum<250000)
			//{
			//	fbase = fbase - fbase*0.09f;
			//}

			
		}

		//黑名单
		if (m_pTableManager->IsBlackUser(player))
		{
			LogInfoToFile("Control.txt", "CatchChance捕鱼概率黑名单:::::::%f:::::::::::%f::::::::::::::::::::", fbase, Con_Cannon::s_finalratio);
			fbase /= 4;
		}
		if (m_pTableManager->IsWhightUser(player))
		{
			LogInfoToFile("Control.txt", "CatchChance捕鱼概率白名单:::::::%f:::::::::::%f::::::::::::::::::::", fbase, Con_Cannon::s_finalratio);
			fbase *= 5;
		}
		//if (m_pTableManager->Isabhor(player))//黑名单  概率减半
		//{
		//	fbase /= 10;
		//}
		return fbase;
	}
	return 0;
#endif
}


SkillFailedType  CConfig::UseSkill(PlayerID playerID, SkillType skill)
{
#if SIMPLE_SERVER_MODE
	return SFT_OK;
#else
	CRole *pUser = m_pTableManager->SearchUser(playerID);
	if (!pUser)
	{
		return SFT_INVALID;
	}

	if ((size_t)skill >= m_Skill.size())
	{
		return SFT_INVALID;
	}
	return pUser->UseSkill(skill);
#endif
}

LaunchFailedType  CConfig::UseLaser(PlayerID playerID, byte launcherType)
{
#if SIMPLE_SERVER_MODE
	return true;
#else
	ASSERT(launcherType < m_Energy.size());

	CRole *pUser = m_pTableManager->SearchUser(playerID);
	if (!pUser)
	{
		return LFT_INVALID;
	}
	return pUser->UseLaser(launcherType);
#endif
}

void  CConfig::GetSkillRange(SkillType skill, int &minNum, int &maxNum)
{
	ASSERT(static_cast<DWORD>(skill) < m_Skill.size());
	minNum = m_Skill[skill].nmincatch;
	maxNum = m_Skill[skill].nmaxcatch;
}

void  CConfig::GetLaserRange(byte laser, int &minNum, int &maxNum)
{
	ASSERT(laser<m_Energy.size());
	minNum = m_Energy[laser].nmincatch;
	maxNum = m_Energy[laser].nmaxcatch;
}

void  CConfig::GetSkillFreezeReduction(byte &speedScaling, byte &duration1, byte& duration2, byte &duration3)
{
	ASSERT(SKILL_FREEZE<m_Skill.size());
	speedScaling = ConvertFloatToBYTE(m_Skill[SKILL_FREEZE].npseed / SPEED_SCALE);
	duration1 = ConvertFloatToBYTE(m_Skill[SKILL_FREEZE].ntime1 / FISH_DURATION_TIME);
	duration2 = ConvertFloatToBYTE(m_Skill[SKILL_FREEZE].ntime2 / FISH_DURATION_TIME);
	duration3 = ConvertFloatToBYTE(m_Skill[SKILL_FREEZE].ntime3 / FISH_DURATION_TIME);
}

float CConfig::GetSkillChance(SkillType skill, USHORT fishIndex, BYTE byPackageType)
{
	ASSERT(ConvertIntToDWORD(skill)<m_Skill.size());
	ASSERT(fishIndex<m_Fish.size());
	return  1.0f*m_Skill[skill].nincome / (m_Fish[fishIndex].nvalue* m_Skill[skill].nmaxcatch)*PackageFactor(byPackageType);
}

float CConfig::GetLaserChance(byte launcherType, USHORT fishIndex, BYTE byPackageType)
{
	ASSERT(launcherType<m_Energy.size());
	ASSERT(fishIndex<m_Fish.size());
	return  1.0f*m_Energy[launcherType].nincome / (m_Fish[fishIndex].nvalue* m_Energy[launcherType].nmaxcatch) *PackageFactor(byPackageType);
}

float CConfig::GetLaserRadius(byte launcherType)
{
	ASSERT(launcherType<m_Energy.size());
	return m_Energy[launcherType].nradius * 1.0f;
}

BulletType CConfig::GetBulletType(USHORT bulletID)
{
	if (bulletID == CANNON_5)
	{
		return BULLET_TYPE_PENETRATION;
	}

	if (bulletID == CANNON_4)//???
	{
		return BULLET_TYPE_FREEZE;
	}

	return BULLET_TYPE_NORMAL;
}

void CConfig::GetBulletFreezeReduction(byte &speedScaling, byte &duration1, byte& duration2, byte &duration3)
{
	// san hao pao shi bin dong  ?????
	ASSERT(CANNON_4<m_Cannon.size());
	speedScaling = ConvertFloatToBYTE(m_Cannon[CANNON_4].npseed / SPEED_SCALE);
	duration1 = ConvertFloatToBYTE(m_Cannon[CANNON_4].ntime1 / FISH_DURATION_TIME);
	duration2 = ConvertFloatToBYTE(m_Cannon[CANNON_4].ntime2 / FISH_DURATION_TIME);
	duration3 = ConvertFloatToBYTE(m_Cannon[CANNON_4].ntime3 / FISH_DURATION_TIME);
}

//
//notify 
// 该函数对类绑定太紧不符合 （高内聚低耦合）
//
UINT   CConfig::CatchFish(PlayerID playerID, USHORT fishIndx, CatchType catchType, byte subType, byte byPackageType, byte byRateIndex)
{
#if SIMPLE_SERVER_MODE
	return fishIndx;
#else
	ASSERT(fishIndx<m_Fish.size());

	CRole *pUser = m_pTableManager->SearchUser(playerID);
	if (pUser&&fishIndx<m_Fish.size())
	{
		int nIncome = m_Fish[fishIndx].nvalue;//bu kao lv beishu *pUser->
		int nMultiple = BulletMultiple(byRateIndex);
		//////////////////////////////////////////////////////////////////////////
		////王者捕鱼正常倍率   微聚需要解开
		//if (catchType == CATCH_SKILL)//技能特有倍率
		//{
		//	nMultiple = SkillMultiple(subType);
		//}
//////////////////////////////////////////////////////////////////////////

		pUser->OnCatchFish(catchType, subType, fishIndx, byPackageType, nIncome*nMultiple, nIncome);
		return nIncome*nMultiple;
	}
	return 0;
#endif
}

USHORT    CConfig::GetCombo(PlayerID playerID, USHORT bulletID)
{
#if SIMPLE_SERVER_MODE
	return 0;
#else
	CRole *pUser = m_pTableManager->SearchUser(playerID);
	if (pUser)
	{
		return pUser->Combo(bulletID);
	}
	return 0;
#endif
}
bool CConfig::CanUploadImg(PlayerID playerID, USHORT imgSize)
{
	return true;
}
byte  CConfig::GetSkillCDTime(SkillType st)
{
	ASSERT(ConvertIntToDWORD(st)<m_Skill.size());
	return ConvertFloatToBYTE(m_Skill[st].ncdtime);
}

void  CConfig::GetLaserReduction(byte LauncherType, byte &speedScaling, byte &duration1, byte& duration2, byte &duration3)
{
	ASSERT(LauncherType<m_Energy.size());
	speedScaling = ConvertFloatToBYTE(m_Energy[LauncherType].npseed / SPEED_SCALE);
	duration1 = ConvertFloatToBYTE(m_Energy[LauncherType].ntime1 / FISH_DURATION_TIME);
	duration2 = ConvertFloatToBYTE(m_Energy[LauncherType].ntime2 / FISH_DURATION_TIME);
	duration3 = ConvertFloatToBYTE(m_Energy[LauncherType].ntime3 / FISH_DURATION_TIME);
}

byte  CConfig::GetBulletSpeed(byte LauncherType)
{
	ASSERT(LauncherType<m_Cannon.size());
	return ConvertIntToBYTE(m_Cannon[LauncherType].bulletspeed); //Con_Cannon::s_bulletspeed;
}

byte  CConfig::GetLauncherInterval(byte LauncherType)
{
	ASSERT(LauncherType<m_Cannon.size());
	return ConvertFloatToBYTE(m_Cannon[LauncherType].dlauncherinterval / FISH_DURATION_TIME);
}

byte  CConfig::GetLaserCDTime(byte LauncherType)
{
	ASSERT(LauncherType<m_Energy.size());
	return ConvertFloatToBYTE(m_Energy[LauncherType].ncdtime / FISH_DURATION_TIME);
}
UINT  CConfig::LaunchPackage(UINT fishid_packageid)
{
	return 1;
}
byte  CConfig::GetRateIndex(byte seat, PlayerID id)
{
#if SIMPLE_SERVER_MODE
	return 0;
#else
	CRole *pPlayer = m_pTableManager->SearchUser(id);
	if (pPlayer)
	{
		return pPlayer->GetRateIndex();
	}
	return 0;
#endif
}
void  CConfig::BulletGain(PlayerID id, byte BulletType, UINT goldNum)
{
#if SIMPLE_SERVER_MODE
#else
	CRole *pPlayer = m_pTableManager->SearchUser(id);
	if (pPlayer)
	{
		pPlayer->BulletGain(BulletType, goldNum);
	}
#endif
}

float CConfig::GameTimeDrop(int nTime)
{
	for (Playtime_Vec::reverse_iterator it = m_PlayTime.rbegin(); it != m_PlayTime.rend(); it++)
	{
		if (nTime >= it->ntime)
		{
			return it->drop;
		}
	}
	return 0;
}
USHORT CConfig::FishRewardDrop(PlayerID id, BYTE byPackageType,USHORT byType)
{
	ASSERT(byType < m_Fish.size());
	CRole *pPlayer = m_pTableManager->SearchUser(id);
	if (pPlayer)
	{
		CRoleEx *pRoleEx = pPlayer->GetRoleExInfo();
		if (pRoleEx&&!pRoleEx->GetRoleMonth().IsInMonthTable())
		{
			HashMap<BYTE, tagChestConfig>::iterator it = g_FishServer.GetFishConfig().GetChestConfig().ChestMap.find(byPackageType);
			if (it != g_FishServer.GetFishConfig().GetChestConfig().ChestMap.end() && (it->second.ImmediateRewardid == 0 || g_FishServer.GetFishConfig().GetFishRewardConfig().RewardMap.count(it->second.ImmediateRewardid) > 0))
			{
				return 0;
			}			
			float fDropChance = m_Fish[byType].chance;//base	
			bool bBossFish = m_Fish[byType].type == 3;//过滤boss鱼
			if (!bBossFish)
			{
				float fdropother = 1;
				fdropother += GameTimeDrop(pPlayer->GetGameTime());// GameTimeDrop
				fdropother += MoneyRate(pPlayer->GetProduction());//Produ			
				BYTE cbMaxRate = pRoleEx->GetRoleRate().GetCanUseMaxRate();
				if (cbMaxRate < m_Rate.size())
				{
					fdropother += m_Rate[cbMaxRate].drop;
				}
				fDropChance *= fdropother;
			}
			
			if (RandFloat() < fDropChance)
			{
				HashMap<BYTE, tagFishDropOnce>::iterator it = g_FishServer.GetFishConfig().GetFishDropConfig().FishDropMap.find(m_Fish[byType].type);
				if (it != g_FishServer.GetFishConfig().GetFishDropConfig().FishDropMap.end())
				{					
					ushort nReward = it->second.GetFishDrop();
					HashMap<WORD, tagRewardOnce>::iterator Iter = g_FishServer.GetFishConfig().GetFishRewardConfig().RewardMap.find(nReward);
					if (Iter != g_FishServer.GetFishConfig().GetFishRewardConfig().RewardMap.end())
					{
						if (!bBossFish)
						{
							for (vector<tagItemOnce>::iterator it = Iter->second.RewardItemVec.begin(); it != Iter->second.RewardItemVec.end(); it++)
							{
								if (g_FishServer.GetFishConfig().GetItemType(it->ItemID) == EItemType::IT_Currey)
								{
									pRoleEx->ChangeRoleProduction(it->ItemSum);
								}
							}
						}				
						pPlayer->AddDropReward(nReward);
						return nReward;
					}																	
				}
			}
		}
	}
	return 0;
}


byte  CConfig::BulletConsume(byte LauncherType)
{
	ASSERT(LauncherType<m_Cannon.size());
	return ConvertIntToBYTE(m_Cannon[LauncherType].nconsume);
}

ushort  CConfig::BulletMultiple(byte byIndex)
{
	ASSERT(byIndex<m_Rate.size());
	return ConvertIntToWORD(m_Rate[byIndex].nValue);
}



//捕鱼的概率基础   和炮的倍数鱼的价值设定值有关系
float CConfig::CalBaseRatio(BYTE cbCannoIndex, BYTE cbFishIndex)
{
	ASSERT(cbCannoIndex<m_Cannon.size());
	ASSERT(cbFishIndex<m_Fish.size());

	//float fratio = 1.0f*m_Cannon[cbCannoIndex].nconsume / m_Fish[cbFishIndex].nvalue*Con_Cannon::s_ratiofactor;

	//<Cannon ratiofactor = "0.800" maxratio = "0.800" finalration = "0.800" lockratio = "0.050">
	//	<item id = "0" name = "玄光" bulletspeed = "50" launcherinterval = "0.200" consume = "1" radius = "0.300" catchradius = "2.000" maxcatch = "2" itemid = "1300" itemcount = "0" skill = "255" speed = "0.000" time1 = "0.000" time2 = "0.000" time3 = "0.000" collisionrate = "0.500 1.000 0.500 1.000" / >
	//	<item id = "1" name = "碧影" bulletspeed = "50" launcherinterval = "0.200" consume = "1" radius = "0.300" catchradius = "2.000" maxcatch = "2" itemid = "1301" itemcount = "1" skill = "255" speed = "0.000" time1 = "0.000" time2 = "0.000" time3 = "0.000" collisionrate = "0.500 0.000 0.500 0.000" / >
	//	<item id = "2" name = "旋刃" bulletspeed = "50" launcherinterval = "0.200" consume = "1" radius = "0.300" catchradius = "3.000" maxcatch = "3" itemid = "1302" itemcount = "1" skill = "2" speed = "0.000" time1 = "0.000" time2 = "0.000" time3 = "0.000" collisionrate = "0.400 0.400 0.300 0.300 0.300 0.300" / >
	//	<item id = "3" name = "凝霜" bulletspeed = "50" launcherinterval = "0.200" consume = "1" radius = "0.300" catchradius = "3.000" maxcatch = "3" itemid = "1303" itemcount = "1" skill = "1" speed = "0.200" time1 = "0.000" time2 = "1.000" time3 = "0.500" collisionrate = "0.400 0.500 0.300 0.400 0.300 0.400" / >
	//	<item id = "4" name = "穿云" bulletspeed = "60" launcherinterval = "0.200" consume = "1" radius = "0.300" catchradius = "1.000" maxcatch = "4" itemid = "1304" itemcount = "1" skill = "3" speed = "0.000" time1 = "0.000" time2 = "0.000" time3 = "0.000" collisionrate = "0.400 0.500 0.200 0.400 0.200 0.400 0.200 0.400" / >
	//	< / Cannon>

	//<Fish revies_Activity = "1.000" flashfishid = "25" flashfishCatch = "4" flashfishradius = "10">
	//	<item rank = "0" id = "8" value = "10" maxcount = "10" chance = "0.010" type = "1" flashchance = "0.500" fishrate = "1.000 1.000 1.000 1.000 0.700" name = "小黄鱼" declare = "我速度快，打不到打不到……" / >
	//	<item rank = "1" id = "7" value = "10" maxcount = "10" chance = "0.010" type = "1" flashchance = "0.500" fishrate = "1.000 1.000 1.000 1.000 0.700" name = "小草鱼" declare = "成群的来，结队的走，摇一摇" / >
	//	< / Fish>
	//修改
	float fratio =10*1.0f*m_Cannon[cbCannoIndex].nconsume / m_Fish[cbFishIndex].nvalue*Con_Cannon::s_ratiofactor;
	if (fratio > Con_Cannon::s_maxratio)
	{
		fratio = Con_Cannon::s_maxratio;
	}

	if (cbCannoIndex < m_Fish[cbFishIndex].fishrate.size())
	{
		fratio *= m_Fish[cbFishIndex].fishrate[cbCannoIndex];
	}

	//fratio *= m_Fish[cbFishIndex].reviseratio;
	fratio *= Con_Fish::s_revies_Activity;//概率倍数
	return fratio;
}
float CConfig::CalRandom(float d1, float d2)
{
	if (d1 == d2)
		return d1;
	return d1 + rand() % ((int)((d2 - d1)*ENLARGE)) / ENLARGE;
}



int CConfig::FishCount()
{
	return m_Fish.size();
}

int CConfig::CannonCount()
{
	return m_Cannon.size();
}

int CConfig::SkillCount()
{
	return m_Skill.size();
}

int CConfig::RankCount()
{
	return m_Rank.size();
}

int CConfig::RateCount()
{
	return m_Rate.size();
}

//根据经验得到等级
int CConfig::GetLevle(int nExperience)
{
	for (Rank_Vec::reverse_iterator it = m_Rank.rbegin(); it != m_Rank.rend(); it++)
	{
		if (nExperience >= it->experience)
		{
			return it->level;
		}
	}
	return 0;
}
int CConfig::RandomCatchCycle()
{
	return Randomtime::s_cycle;
}
int CConfig::RpCycle()
{
	return ConvertFloatToInt(Con_Rp::s_cycle);
}
int CConfig::RpEffectCycle()
{
	return ConvertFloatToInt(Con_Rp::s_effecttime);
}

//rp影响概率
float CConfig::RpRate(float nRp)
{
	for (Rp_Vec::iterator it = m_Rp.begin(); it != m_Rp.end(); it++)
	{
		if (nRp < it->rp)
		{
			return CalRandom(it->reviseratio1, it->reviseratio2);
		}
	}
	return 0;
}
////随机时间影响
//float CConfig::RandomTimeRate(int nFishKind)
//{
//	return 0;
//}



//产量影响概率
float CConfig::ProductionRate(int nProducntion)
{
	//从后往前找
	for (Production_Vec::reverse_iterator it = m_production.rbegin(); it != m_production.rend(); it++)
	{
		if (nProducntion >= it->nincome)
		{
			return CalRandom(it->reviseratio1, it->reviseratio2);
		}
	}
	return 0;
}

//等级影响概率
float CConfig::RankRate(int nLevel)
{
	//从后往前找
	for (Rank_Vec::reverse_iterator it = m_Rank.rbegin(); it != m_Rank.rend(); it++)
	{
		if (nLevel >= it->level)
		{			
			return it->reviseratio;
		}
	}
	return 0;
}

//在线时间也许爱那个概率,最后一次调整
float CConfig::GameTimeRate(int nMinutes)
{
	//return
	for (Playtime_Vec::reverse_iterator it = m_PlayTime.rbegin(); it != m_PlayTime.rend(); it++)
	{
		if (nMinutes >= it->ntime)
		{
			return CalRandom(it->reviseratio1, it->reviseratio2);
		}
	}
	return 0;
}

float CConfig::MoneyRate(int nMoney)
{
	for (Money_Vec::reverse_iterator it = m_Money.rbegin(); it != m_Money.rend(); it++)
	{
		if (nMoney >= it->money)
		{
			return CalRandom(it->reviseratio1, it->reviseratio2);
		}
	}
	return 0;
}
float CConfig::LuckRate(int nLuck)
{
	for (Luck_Vec::reverse_iterator it = m_VceLuck.rbegin(); it!= m_VceLuck.rend(); it++)
	{
		if (nLuck > it->nluck)
		{
			return CalRandom(it->reviseratio1,it->reviseratio2);
		}
	}
	return 0;
}

float CConfig::PoolRate(int nPool)
{
	for (Pool_Vec::iterator it = m_GoldPool.begin(); it != m_GoldPool.end();it++)
	{
		if (nPool < it->pool)
		{
			return CalRandom(it->reviseratio1, it->reviseratio2);
		}
	}
	return 0;
}


int  CConfig::LaserThreshold(byte byIndex)
{
	ASSERT(byIndex<m_Energy.size());
	return m_Energy[byIndex].nthreshold;
}

float  CConfig::LauncherInterval(byte byIndex)
{
	ASSERT(byIndex<m_Cannon.size());
	return m_Cannon[byIndex].dlauncherinterval;
}
float  CConfig::LaserCdTime(byte byIndex)
{
	ASSERT(byIndex<m_Energy.size());
	return m_Energy[byIndex].ncdtime;
}
float  CConfig::SkillCdTime(byte byIndex)
{
	ASSERT(byIndex<m_Skill.size());
	return m_Skill[byIndex].ncdtime;
}

void CConfig::GoodsInfo(int nSkill, int norder, int &nGoodsid, int &nGoodsConsume)
{
	nGoodsid = m_Skill[nSkill].ngoodsid;

	if (m_Skill[nSkill].vecConsume.size() == 0)
	{
		nGoodsConsume = 0;
		return;
	}
	for (size_t i = 0; i < m_Skill[nSkill].vecConsume.size(); i++)
	{
		if (norder < m_Skill[nSkill].vecConsume[i].byorder)
		{
			nGoodsConsume = m_Skill[nSkill].vecConsume[i].byCount; //ngoodsconsume;
			return;
		}
	}

	nGoodsConsume = m_Skill[nSkill].vecConsume.back().byCount;//the last one
}
void CConfig::GoodsInfo(int nCannon, int &nGoodsid, int &nGoodsCount)
{
	ASSERT(ConvertIntToDWORD(nCannon)<m_Cannon.size());
	nGoodsid = m_Cannon[nCannon].nItemId;
	nGoodsCount = m_Cannon[nCannon].nItemCount;
}

bool CConfig::FindCannon(byte nSkill, int& nCannon)
{
	for (Cannon_Vec::iterator it = m_Cannon.begin(); it != m_Cannon.end(); it++)
	{
		if (it->nskill == nSkill)
		{
			nCannon = it->nid;
			return true;
		}
	}
	return false;
}

bool CConfig::LevelUp(WORD cbLevel, DWORD nExp, WORD& nNewLevel, DWORD&nNewExp)
{
	ASSERT(cbLevel<m_Rank.size());
	int nSize = m_Rank.size();
	nNewLevel = cbLevel;
	nNewExp = nExp;
	while (nNewLevel + 1 < (WORD)nSize && (nNewExp >= (DWORD)(m_Rank[nNewLevel + 1].experience - m_Rank[nNewLevel].experience)))
	{
		nNewExp -= (DWORD)(m_Rank[nNewLevel + 1].experience - m_Rank[nNewLevel].experience);
		nNewLevel += 1;
	}
	return nNewLevel != cbLevel;
}
float CConfig::ComboSustainTime()
{
	return Con_Combo::s_sustaintime* 1.0f;
}
int CConfig::ComboBuffCycle()
{
	return Con_Combo::s_buff_cycle;
}

float CConfig::PackageFactor(BYTE cbPackage)
{	
	HashMap<BYTE, tagChestConfig>::iterator it=g_FishServer.GetFishConfig().GetChestConfig().ChestMap.find(cbPackage);
	if (it == g_FishServer.GetFishConfig().GetChestConfig().ChestMap.end())
	{
		//ASSERT(false);
		return 1.0f;
	}
	if (it->second.CatchChance==0)
	{
		//ASSERT(false);
		return 1.0f;
	}
	return 1.0f / it->second.CatchChance;
}
USHORT CConfig::RateUnlock(BYTE byIndex)
{
	ASSERT(byIndex<m_Rate.size());
	return m_Rate[byIndex].nUnlock;
}

ushort CConfig::SkillMultiple(byte byIndex)
{
	ASSERT(byIndex<m_Skill.size());
	return m_Skill[byIndex].multiple;
}
USHORT CConfig::UnlockRateReward(BYTE byIndex)
{
	if (byIndex < m_Rate.size())
	{
		return m_Rate[byIndex].unlockreward;
	}
	return 0;
}

bool CConfig::IsLightingFish(USHORT fishIndex)
{
	return fishIndex == Con_Fish::s_flashfishid;
}
int  CConfig::IsLightingFish(USHORT fishIndex, PlayerID id)
{
	if (IsLightingFish(fishIndex))
	{
		return Con_Fish::s_flashfishCatch;
	}
	return false;
	//如果是闪电鱼，返回大于0的捕获数量
	//return (fishIndex < 8) ? 5 : 0;
}
//某一条鱼是否可以被闪电捕获，fishIndex鱼ID，玩家ID，dist:屏幕距离。
bool CConfig::CanCatchLightingFish(USHORT fishIndex, PlayerID id, float dist)
{
	if (fishIndex >= m_Fish.size())
	{
		return false;		
	}
	return dist<Con_Fish::s_flashfishradius&&RandFloat()<m_Fish[fishIndex].flashchance;
	//return RandRange(0, 10) > 7 && dist < 25;
}
BYTE CConfig::CannonOwnSkill(BYTE byIndex)
{
	if (byIndex < m_Cannon.size())
	{
		return m_Cannon[byIndex].nskill;
	}
	return 255;
}