DROP PROCEDURE IF EXISTS `FishAccountRsg`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAccountRsg`(IN `Account` varchar(32),Mac varchar(56),IN `PasswordCrc1` int unsigned,IN `PasswordCrc2` int unsigned,IN `PasswordCrc3` int unsigned,IN `NickName` varchar(8),IN `Gender` bit,IN `InitGlobelSum` int unsigned,IN `InitRateValue`  blob,IN `RsgIP` varchar(16),IN `LogTime` datetime,IN `FishCode` int unsigned)
BEGIN
	set @OutUserID = 0;
	set @OutUserID1 = 0;
	
	select Id into @OutUserID1 from Fishadmin where Fishadmin.UserID=FishCode;
	if @OutUserID1>0 THEN
      if not exists (select accountinfo.UserID from accountinfo where accountinfo.AccountName = Account) THEN
          INSERT accountinfo(AccountName,MacAddress,PasswordCrc1,PasswordCrc2,PasswordCrc3,NickName,Gender,IsCanChangeAccount,GlobalNum,MedalNum,RsgIP,RsgLogTime,RateValue,FishCode)
          values (Account,Mac,PasswordCrc1,PasswordCrc2,PasswordCrc3,NickName,Gender,0,InitGlobelSum,0,RsgIP,LogTime,InitRateValue,FishCode);
          set @OutUserID = @@IDEntity;

          insert into FishUserEntity(UserID,Name,Phone,Email,IdentityID,EntityItemUseName,EntityItemUsePhone,EntityItemUseAddress) values(@OutUserID,'','0','','','','0','');
      ELSE
          set @OutUserID = 0;
      end if;
      select @OutUserID;
  ELSE
  set @OutUserID=0;
  end if;
  select @OutUserID;
END
;;
DELIMITER ;