-- ----------------------------
-- Procedure structure for FishLoadUserInfo
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishLoadUserInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishLoadUserInfo`(IN `UserID` int unsigned,IN `LogonTime` datetime)
BEGIN
	declare NewGameID int unsigned default 0;
	update accountinfo set accountinfo.IsOnline = 1 ,accountinfo.OnlineLogonTime = LogonTime where AccountInfo.UserID  = UserID;
	-- set @GameID = fishgame.FishCreateGameID(UserID);
	-- set @GameID = 0;

	set @StatesLength = 6;
	set @GameSum = 0;
	select GameID into NewGameID from fishgameid where fishgameid.UserID = UserID;
	if NewGameID = 0 THEN
			select Max(fishgameid.States) into @StatesLength from fishgameid group by fishgameid.States;
			select count(GameID) into @GameSum from fishgameid where fishgameid.States = @StatesLength;
			if @GameSum >= (POW(10,@StatesLength -1) * 4) THEN -- �?高为不可以为 0
					set @StatesLength = @StatesLength + 1; -- 升级长度
			end if;
			set NewGameID = RAND() * (POW(10,@StatesLength)-1 - POW(10,@StatesLength -1) ) + POW(10,@StatesLength -1) ; -- 生成新的GameID
			while EXISTS (select GameID from fishgameid where fishgameid.GameID = NewGameID) DO
					set NewGameID = RAND() * (POW(10,@StatesLength)-1 - POW(10,@StatesLength -1) ) + POW(10,@StatesLength -1) ; -- 生成新的GameID
			end while;
			insert into fishgameid(GameID,States,UserID) values(NewGameID,@StatesLength,UserID); -- 插入生成的GameID 数据
	end if;

	select UserID,NickName,FaceID,FishLevel,FishExp,Gender,GlobalNum,MedalNum,
					CurrencyNum,Production,GameTime,LastLogonTime,TitleID,AchievementPoint,
					SendGiffSum,AcceptGiffSum,TaskStatus,AchievementStatus,ActionStatus,OnlineMin,OnlineRewardStates,CharmValue,CharmArray,IsCanChangeAccount,LastLogonIp,CheckData,IsShowIpAddress,IsRobot
					,ExChangeStates,TotalRechargeSum,TotalFishGlobelSum,RoleProtectSum,RoleProtectLogTime,IsFirstPayGlobel,IsFirstPayCurrcey,IsFreeze,FreezeEndTime,LotteryScore,LotteryFishSum,
					MonthCardID,MonthCardEndTime,GetMonthCardRewardTime,RateValue,VipLevel,CashSum,TotalUseMedal,OnlineLogonTime,ParticularStates,NewGameID as 'GameID',PhonePasswordCrc1,PhonePasswordCrc2,PhonePasswordCrc3
					,IsShareStates,TotalCashSum,szHeadHttp
		from AccountInfo
		where AccountInfo.UserID  = UserID;
END
;;
DELIMITER ;

