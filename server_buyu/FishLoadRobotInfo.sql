DROP PROCEDURE IF EXISTS `FishLoadRobotInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishLoadRobotInfo`(IN `RobotBeginID` int unsigned,IN `RobotEndID` int unsigned,IN  `LogonTime` datetime)
BEGIN
		update accountinfo set IsOnline =1 ,OnlineLogonTime = LogonTime,LastLogonIp = ((((inet_aton(RsgIP)) & 0xff) << 24) + (((inet_aton(RsgIP) >> 8) & 0xff) << 16) + (((inet_aton(RsgIP) >> 16) & 0xff) << 8) + ((inet_aton(RsgIP) >> 24) & 0xff))
					where UserID <= RobotEndID and UserID >= RobotBeginID and IsRobot = 1 and IsOnline = 0; 

		select UserID,NickName,FishLevel,FishExp,FaceID,Gender,GlobalNum,MedalNum,
							CurrencyNum,AchievementPoint,TitleID,CharmArray,VipLevel,MonthCardID,MonthCardEndTime,szHeadHttp
			from accountinfo 
		  where IsRobot =1 and UserID <= RobotEndID and UserID >= RobotBeginID;
END
;;
DELIMITER ;