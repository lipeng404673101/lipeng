/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50717
Source Host           : localhost:3306
Source Database       : fishlog

Target Server Type    : MYSQL
Target Server Version : 50717
File Encoding         : 65001

Date: 2017-11-23 10:38:56
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `fishaddrechargelog`
-- ----------------------------
DROP TABLE IF EXISTS `fishaddrechargelog`;
CREATE TABLE `fishaddrechargelog` (
  `UserID` int(10) unsigned NOT NULL,
  `channelLabel` varchar(256) NOT NULL,
  `orderid` varchar(256) NOT NULL,
  `channelOrderid` varchar(256) NOT NULL,
  `UseRMBSum` int(10) unsigned NOT NULL,
  `AddGlobel` int(10) unsigned NOT NULL,
  `AddCurrcey` int(10) unsigned NOT NULL,
  `AddOrderUserName` varchar(16) NOT NULL,
  `AddOrderUserIP` varchar(16) NOT NULL,
  `AddOrderLogTime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishaddrechargelog
-- ----------------------------

-- ----------------------------
-- Table structure for `fishannouncementinfolog`
-- ----------------------------
DROP TABLE IF EXISTS `fishannouncementinfolog`;
CREATE TABLE `fishannouncementinfolog` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `NickName` varchar(11) NOT NULL,
  `ShopID` tinyint(4) unsigned NOT NULL,
  `ShopOnlyID` tinyint(4) unsigned NOT NULL,
  `LogTime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishannouncementinfolog
-- ----------------------------

-- ----------------------------
-- Table structure for `fishcartableinfo`
-- ----------------------------
DROP TABLE IF EXISTS `fishcartableinfo`;
CREATE TABLE `fishcartableinfo` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `BrandUserID` int(10) unsigned NOT NULL,
  `AreaGlobel` varchar(1024) NOT NULL,
  `ResultIndex` tinyint(3) unsigned NOT NULL,
  `BrandGlobel` bigint(20) unsigned NOT NULL,
  `SystemGlobel` bigint(20) unsigned NOT NULL,
  `RoleSum` int(10) unsigned NOT NULL,
  `LogTime` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishcartableinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `fishdialtableinfo`
-- ----------------------------
DROP TABLE IF EXISTS `fishdialtableinfo`;
CREATE TABLE `fishdialtableinfo` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `BrandUserID` int(10) unsigned NOT NULL,
  `AreaData` varchar(256) NOT NULL,
  `AreaGlobel` varchar(1024) NOT NULL,
  `ResultIndex` smallint(3) unsigned NOT NULL,
  `BrandGlobel` bigint(20) NOT NULL,
  `SystemGlobel` bigint(20) NOT NULL,
  `RoleSum` int(10) unsigned NOT NULL,
  `LogTime` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishdialtableinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `fishentityitemfinishlog`
-- ----------------------------
DROP TABLE IF EXISTS `fishentityitemfinishlog`;
CREATE TABLE `fishentityitemfinishlog` (
  `ID` int(10) unsigned NOT NULL,
  `OrderID` varchar(64) NOT NULL,
  `HandleTime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishentityitemfinishlog
-- ----------------------------

-- ----------------------------
-- Table structure for `fishentityitemlog`
-- ----------------------------
DROP TABLE IF EXISTS `fishentityitemlog`;
CREATE TABLE `fishentityitemlog` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `OrderStates` varchar(256) NOT NULL DEFAULT '',
  `UserID` int(11) unsigned NOT NULL,
  `ItemID` int(11) unsigned NOT NULL,
  `ItemSum` smallint(6) unsigned NOT NULL,
  `Address` varchar(256) NOT NULL DEFAULT '',
  `Phone` varchar(20) NOT NULL DEFAULT '',
  `IDEntity` varchar(256) NOT NULL DEFAULT '',
  `Name` varchar(256) NOT NULL DEFAULT '',
  `OrderNumber` varchar(256) NOT NULL DEFAULT '',
  `MedalNum` int(10) unsigned NOT NULL DEFAULT '0',
  `NowMedalNum` int(10) unsigned NOT NULL DEFAULT '0',
  `HandleIP` varchar(16) NOT NULL DEFAULT '',
  `ShopLogTime` datetime NOT NULL DEFAULT '1971-01-01 00:00:00',
  `HandleTime` datetime NOT NULL DEFAULT '1971-01-01 00:00:00',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishentityitemlog
-- ----------------------------

-- ----------------------------
-- Table structure for `fishexchangelog`
-- ----------------------------
DROP TABLE IF EXISTS `fishexchangelog`;
CREATE TABLE `fishexchangelog` (
  `UserID` int(10) unsigned NOT NULL,
  `TypeID` tinyint(3) unsigned NOT NULL,
  `ExChange` varchar(17) NOT NULL,
  `LogTime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishexchangelog
-- ----------------------------

-- ----------------------------
-- Table structure for `fishlog`
-- ----------------------------
DROP TABLE IF EXISTS `fishlog`;
CREATE TABLE `fishlog` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserID` int(10) unsigned NOT NULL,
  `TypeID` int(10) unsigned NOT NULL COMMENT '1表示 乐币 2表示 奖牌 3表示 奖励',
  `TypeSum` int(10) NOT NULL,
  `Param` int(10) unsigned NOT NULL DEFAULT '0',
  `Info` varchar(256) NOT NULL,
  `LogTime` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishlog
-- ----------------------------

-- ----------------------------
-- Table structure for `fishlogmonthinfo`
-- ----------------------------
DROP TABLE IF EXISTS `fishlogmonthinfo`;
CREATE TABLE `fishlogmonthinfo` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserID` int(10) unsigned NOT NULL,
  `MonthID` tinyint(3) unsigned NOT NULL,
  `MonthIndex` int(10) unsigned NOT NULL,
  `MonthScore` int(10) unsigned NOT NULL,
  `MonthSkillSum` int(10) unsigned NOT NULL,
  `MonthAddGlobelSum` tinyint(3) unsigned NOT NULL,
  `MonthRewardID` smallint(5) unsigned NOT NULL,
  `LogTime` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishlogmonthinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `fishlotteryinfo`
-- ----------------------------
DROP TABLE IF EXISTS `fishlotteryinfo`;
CREATE TABLE `fishlotteryinfo` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserID` int(10) unsigned NOT NULL,
  `LotteryID` tinyint(3) unsigned NOT NULL,
  `RewardID` smallint(5) unsigned NOT NULL,
  `LogTime` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishlotteryinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `fishmaillog`
-- ----------------------------
DROP TABLE IF EXISTS `fishmaillog`;
CREATE TABLE `fishmaillog` (
  `MailID` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `SrcUserID` int(11) unsigned NOT NULL,
  `DestUserID` int(11) unsigned NOT NULL,
  `Context` varchar(65) NOT NULL,
  `IsRead` bit(1) NOT NULL DEFAULT b'0',
  `SendTime` datetime NOT NULL,
  `RewardID` smallint(5) unsigned NOT NULL DEFAULT '0',
  `IsExistsReward` bit(1) NOT NULL DEFAULT b'0',
  PRIMARY KEY (`MailID`),
  KEY `Mail_Dest_Primary` (`DestUserID`),
  KEY `Mail_Src_Primary` (`SrcUserID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishmaillog
-- ----------------------------

-- ----------------------------
-- Table structure for `fishniuniutableinfo`
-- ----------------------------
DROP TABLE IF EXISTS `fishniuniutableinfo`;
CREATE TABLE `fishniuniutableinfo` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `BrandArray1` varchar(32) NOT NULL,
  `BrandArray2` varchar(32) NOT NULL,
  `BrandArray3` varchar(32) NOT NULL,
  `BrandArray4` varchar(32) NOT NULL,
  `BrandArray5` varchar(32) NOT NULL,
  `ChangeGlobelSum` bigint(20) NOT NULL DEFAULT '0',
  `SystemGlobelSum` bigint(20) NOT NULL,
  `LogTime` datetime NOT NULL,
  `UserSum` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishniuniutableinfo
-- ----------------------------

-- ----------------------------
-- Table structure for `fishphonepaylog`
-- ----------------------------
DROP TABLE IF EXISTS `fishphonepaylog`;
CREATE TABLE `fishphonepaylog` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `OrderID` bigint(20) unsigned NOT NULL DEFAULT '0',
  `UserID` int(10) unsigned NOT NULL,
  `PhoneNumber` bigint(20) unsigned NOT NULL,
  `OrderStates` varchar(256) NOT NULL DEFAULT '',
  `FacePrice` int(10) unsigned NOT NULL DEFAULT '0',
  `LogTime` datetime NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishphonepaylog
-- ----------------------------

-- ----------------------------
-- Table structure for `fishrechargelog`
-- ----------------------------
DROP TABLE IF EXISTS `fishrechargelog`;
CREATE TABLE `fishrechargelog` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `OrderStates` varchar(256) NOT NULL DEFAULT '',
  `UserID` int(10) unsigned NOT NULL,
  `Price` int(10) unsigned NOT NULL DEFAULT '0',
  `FreePrice` int(10) unsigned NOT NULL DEFAULT '0',
  `OldGlobelNum` int(10) unsigned NOT NULL DEFAULT '0',
  `OldCurrceyNum` int(10) unsigned NOT NULL DEFAULT '0',
  `orderid` varchar(256) NOT NULL,
  `channelOrderid` varchar(256) NOT NULL,
  `channelLabel` varchar(256) NOT NULL,
  `ShopItemID` int(10) unsigned NOT NULL DEFAULT '0',
  `ChannelCode` varchar(256) NOT NULL,
  `AddGlobelSum` int(10) unsigned NOT NULL DEFAULT '0',
  `AddCurrceySum` int(10) unsigned NOT NULL,
  `LogTime` datetime NOT NULL,
  `AddRewardID` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=39 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishrechargelog
-- ----------------------------
INSERT INTO `fishrechargelog` VALUES ('1', '游戏服务器内部充值成功', '20416', '3000', '0', '572340', '5', 'testsV7Ien1NyDZiBkIwL2b3DBUJkhMhQE', '', '', '3', '', '360000', '0', '2017-07-29 10:57:35', '0');
INSERT INTO `fishrechargelog` VALUES ('2', '游戏服务器内部充值成功', '20351', '600', '0', '315700', '14862', 'testMkmcfPduNwaeC68mQQqEyGHZnEFPpq', '', '', '2', '', '60000', '0', '2017-07-29 10:57:59', '0');
INSERT INTO `fishrechargelog` VALUES ('3', '游戏服务器内部充值成功', '20351', '600', '0', '381700', '14862', 'testvOZk8WPf19jjVzln0Enn0noeBR037L', '', '', '2', '', '60000', '0', '2017-07-29 14:05:39', '0');
INSERT INTO `fishrechargelog` VALUES ('4', '游戏服务器内部充值成功', '20351', '600', '0', '447700', '14862', 'testHKHLzD6Be2lwrjREO3LioYSeJuOmOj', '', '', '2', '', '60000', '0', '2017-07-29 14:06:56', '0');
INSERT INTO `fishrechargelog` VALUES ('5', '游戏服务器内部充值成功', '20351', '3000', '0', '843700', '14862', 'testfEP8KgR2MC7wUFqADiD3Be2OMpNEoC', '', '', '3', '', '360000', '0', '2017-07-29 14:07:05', '0');
INSERT INTO `fishrechargelog` VALUES ('6', '游戏服务器内部充值成功', '20351', '600', '0', '909700', '14862', 'testDxocNoVWTonF0lKmf4rM4OSU92vk9U', '', '', '2', '', '60000', '0', '2017-07-29 14:07:11', '0');
INSERT INTO `fishrechargelog` VALUES ('7', '游戏服务器内部充值成功', '20351', '600', '0', '975700', '14862', 'testRDL1h4U5R4pm1VGhxMuIqcoBubxzPX', '', '', '2', '', '60000', '0', '2017-07-29 14:07:19', '0');
INSERT INTO `fishrechargelog` VALUES ('8', '游戏服务器内部充值成功', '20351', '100', '0', '1008700', '14862', 'testPnEJwRuekXySRU3DkM9vEnjyWFJ4Zb', '', '', '1', '', '30000', '0', '2017-07-29 14:07:32', '0');
INSERT INTO `fishrechargelog` VALUES ('9', '游戏服务器内部充值成功', '20351', '3000', '0', '1404700', '14862', 'testv2g8jUApF9b7AtZNSJrYiFrv6QPEzt', '', '', '3', '', '360000', '0', '2017-07-29 14:07:37', '0');
INSERT INTO `fishrechargelog` VALUES ('10', '游戏服务器内部充值成功', '20351', '3000', '0', '1800700', '14862', 'testfJl3QYlHe0DM1Sh3OcDNIdc4p8Cmyo', '', '', '3', '', '360000', '0', '2017-07-29 14:07:54', '0');
INSERT INTO `fishrechargelog` VALUES ('11', '游戏服务器内部充值成功', '20351', '6800', '0', '2680700', '14862', 'testV9EhyKEpjJfPPbury0WdEZTH83iwtK', '', '', '4', '', '800000', '0', '2017-07-29 14:08:03', '0');
INSERT INTO `fishrechargelog` VALUES ('12', '游戏服务器内部充值成功', '20351', '100', '0', '2680700', '14895', 'testtk9yl50IJCECesIPD6KUP21xUYhelC', '', '', '7', '', '0', '30', '2017-07-29 14:08:15', '0');
INSERT INTO `fishrechargelog` VALUES ('13', '游戏服务器内部充值成功', '20416', '100', '0', '572340', '38', 'testpFoUowncJpM0BWszncwyv49Hcas2Du', '', '', '7', '', '0', '30', '2017-07-29 14:08:44', '0');
INSERT INTO `fishrechargelog` VALUES ('14', '游戏服务器内部充值成功', '20416', '3000', '0', '572340', '434', 'testmmUAQiWHi9xPZUT7co9PaKW0NvxftV', '', '', '9', '', '0', '360', '2017-07-29 14:08:53', '0');
INSERT INTO `fishrechargelog` VALUES ('15', '游戏服务器内部充值成功', '20492', '100', '0', '34000', '0', 'testCeer4HTIkVqWjhPFuRh6pQrWFLzUAX', '', '', '1', '', '30000', '0', '2017-07-29 14:10:25', '0');
INSERT INTO `fishrechargelog` VALUES ('16', '游戏服务器内部充值成功', '20492', '3000', '0', '430000', '0', 'testoDDOp9ZaSe52PIrnojq5sOGu5UCeTA', '', '', '3', '', '360000', '0', '2017-07-29 14:10:34', '0');
INSERT INTO `fishrechargelog` VALUES ('17', '游戏服务器内部充值成功', '20492', '32800', '0', '5160000', '0', 'testeFrTpCyslRhKQGXbsIoeT3NEE2B69Y', '', '', '6', '', '4300000', '0', '2017-07-29 14:10:44', '0');
INSERT INTO `fishrechargelog` VALUES ('18', '游戏服务器内部充值成功', '20492', '32800', '0', '5160000', '4730', 'test521JqDD8yFlH1ulmH36Bu5DkfyvkHo', '', '', '12', '', '0', '4300', '2017-07-29 14:10:51', '0');
INSERT INTO `fishrechargelog` VALUES ('19', '游戏服务器内部充值成功', '20492', '3000', '0', '5160000', '5126', 'testgcDKAmYbkY9OreyGcaCfBpd1j8zd75', '', '', '9', '', '0', '360', '2017-07-29 14:11:01', '0');
INSERT INTO `fishrechargelog` VALUES ('20', '游戏服务器内部充值成功', '20492', '6800', '0', '6040000', '5126', 'testkBqob5cdKr5hpsvDdKTZmJb6a6Aj1y', '', '', '4', '', '800000', '0', '2017-07-29 14:14:49', '0');
INSERT INTO `fishrechargelog` VALUES ('21', '游戏服务器内部充值成功', '20492', '3000', '0', '6436000', '5126', 'testkqI5idCBHEfAjcIi2hWF7dzeFSktUu', '', '', '3', '', '360000', '0', '2017-07-29 14:15:22', '0');
INSERT INTO `fishrechargelog` VALUES ('22', '游戏服务器内部充值成功', '20375', '32800', '0', '5160249', '5', 'testzrcb5kioS0e6UsM29b5vpAba2DPZdY', '', '', '6', '', '4300000', '0', '2017-07-29 15:56:11', '0');
INSERT INTO `fishrechargelog` VALUES ('23', '游戏服务器内部充值成功', '20375', '12800', '0', '5175249', '1770', 'testJp4SiBFbNWsUDHcqUDfQAE8pNaoaHE', '', '', '11', '', '0', '1600', '2017-07-29 15:56:37', '0');
INSERT INTO `fishrechargelog` VALUES ('24', '游戏服务器内部充值成功', '20375', '600', '0', '5175249', '1836', 'test53ON5z7r3NYgA3oLtgDEsIp5dnzlAV', '', '', '8', '', '0', '60', '2017-07-29 15:56:55', '0');
INSERT INTO `fishrechargelog` VALUES ('25', '游戏服务器内部充值成功', '20375', '3000', '0', '5175249', '2232', 'testJ3AZCcQgnOgPi4fXLtBihpsYUk0N54', '', '', '9', '', '0', '360', '2017-07-29 15:57:13', '0');
INSERT INTO `fishrechargelog` VALUES ('26', '游戏服务器内部充值成功', '20375', '3000', '0', '5175249', '2628', 'testGzdOORspJ8UxoVQ7e45w0Y8cmLhNmh', '', '', '9', '', '0', '360', '2017-07-29 15:57:36', '0');
INSERT INTO `fishrechargelog` VALUES ('27', '游戏服务器内部充值成功', '20375', '32800', '0', '5175249', '7358', 'testuibb99zuempEoseRhQAeLKNKAnBkD7', '', '', '12', '', '0', '4300', '2017-07-29 15:57:46', '0');
INSERT INTO `fishrechargelog` VALUES ('28', '游戏服务器充值成功:玩家在线', '20534', '32800', '0', '0', '0', '10014', '', '', '6', '', '4300000', '0', '2017-08-31 17:17:28', '0');
INSERT INTO `fishrechargelog` VALUES ('29', '游戏服务器充值成功:玩家在线', '20536', '100', '0', '0', '0', '10014', '', '', '1', '', '30000', '0', '2017-09-03 13:24:40', '0');
INSERT INTO `fishrechargelog` VALUES ('30', '游戏服务器充值成功:玩家在线', '20536', '100', '0', '0', '0', '10015', '', '', '7', '', '0', '30', '2017-09-03 13:27:03', '0');
INSERT INTO `fishrechargelog` VALUES ('31', '游戏服务器充值成功:玩家在线', '20550', '32800', '0', '0', '0', '10016', '', '', '6', '', '4300000', '0', '2017-09-04 15:31:04', '0');
INSERT INTO `fishrechargelog` VALUES ('32', '游戏服务器充值成功:玩家在线', '20550', '600', '0', '0', '0', '10014', '', '', '8', '', '0', '60', '2017-09-12 10:54:42', '0');
INSERT INTO `fishrechargelog` VALUES ('33', '游戏服务器充值成功:玩家在线', '20484', '100', '0', '0', '0', '10015', '', '', '7', '', '0', '30', '2017-09-13 15:53:36', '0');
INSERT INTO `fishrechargelog` VALUES ('34', '游戏服务器充值成功:玩家在线', '20484', '600', '0', '0', '0', '10016', '', '', '8', '', '0', '60', '2017-09-13 15:53:41', '0');
INSERT INTO `fishrechargelog` VALUES ('35', '游戏服务器充值成功:玩家在线', '20558', '100', '0', '0', '0', '10017', '', '', '7', '', '0', '30', '2017-09-13 21:27:16', '0');
INSERT INTO `fishrechargelog` VALUES ('36', '游戏服务器充值成功:玩家在线', '20562', '100', '0', '0', '0', '10018', '', '', '7', '', '0', '30', '2017-09-14 16:46:25', '0');
INSERT INTO `fishrechargelog` VALUES ('37', '游戏服务器充值成功:玩家在线', '20562', '100', '0', '0', '0', '10019', '', '', '1', '', '30000', '0', '2017-09-14 17:31:29', '0');
INSERT INTO `fishrechargelog` VALUES ('38', '游戏服务器充值成功:玩家在线', '20562', '600', '0', '0', '0', '10020', '', '', '8', '', '0', '60', '2017-09-14 17:42:00', '0');

-- ----------------------------
-- Table structure for `fishroleonlinelog`
-- ----------------------------
DROP TABLE IF EXISTS `fishroleonlinelog`;
CREATE TABLE `fishroleonlinelog` (
  `UserID` int(10) unsigned NOT NULL,
  `IsOnline` bit(1) NOT NULL,
  `GlobelSum` int(10) unsigned NOT NULL DEFAULT '0',
  `MadelSum` int(10) unsigned NOT NULL DEFAULT '0',
  `CurrceySum` int(10) unsigned NOT NULL DEFAULT '0',
  `MacAddress` varchar(57) NOT NULL,
  `IpAddress` varchar(17) NOT NULL,
  `LogTime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishroleonlinelog
-- ----------------------------

-- ----------------------------
-- Table structure for `fishroletablelog`
-- ----------------------------
DROP TABLE IF EXISTS `fishroletablelog`;
CREATE TABLE `fishroletablelog` (
  `UserID` int(10) unsigned NOT NULL,
  `TableTypeID` tinyint(3) unsigned NOT NULL,
  `TableMonthID` tinyint(3) unsigned NOT NULL,
  `GlobelNum` int(10) unsigned NOT NULL,
  `MedalNum` int(10) unsigned NOT NULL,
  `CurrceyNum` int(10) unsigned NOT NULL,
  `JoinOrLeave` bit(1) NOT NULL,
  `LogTime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of fishroletablelog
-- ----------------------------

-- ----------------------------
-- Table structure for `totalfishentityitemfinishlog`
-- ----------------------------
DROP TABLE IF EXISTS `totalfishentityitemfinishlog`;
CREATE TABLE `totalfishentityitemfinishlog` (
  `ID` int(10) unsigned NOT NULL,
  `OrderID` varchar(64) NOT NULL,
  `HandleTime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of totalfishentityitemfinishlog
-- ----------------------------

-- ----------------------------
-- Table structure for `totalfishentityitemlog`
-- ----------------------------
DROP TABLE IF EXISTS `totalfishentityitemlog`;
CREATE TABLE `totalfishentityitemlog` (
  `ID` int(10) unsigned NOT NULL,
  `UserID` int(11) unsigned NOT NULL,
  `ItemID` int(11) unsigned NOT NULL,
  `ItemSum` smallint(6) unsigned NOT NULL,
  `Address` varchar(129) NOT NULL,
  `Phone` varchar(33) NOT NULL,
  `Name` varchar(11) NOT NULL,
  `ShopTime` datetime NOT NULL,
  `MedalNum` int(10) unsigned NOT NULL,
  `NowMedalNum` int(10) unsigned NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of totalfishentityitemlog
-- ----------------------------

-- ----------------------------
-- Table structure for `totalfishphonepaylog`
-- ----------------------------
DROP TABLE IF EXISTS `totalfishphonepaylog`;
CREATE TABLE `totalfishphonepaylog` (
  `UserID` int(10) unsigned NOT NULL,
  `PhoneNumber` bigint(20) unsigned NOT NULL,
  `PhoneMoney` int(10) unsigned NOT NULL,
  `LogTime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of totalfishphonepaylog
-- ----------------------------

-- ----------------------------
-- Table structure for `totalfishrechargelog`
-- ----------------------------
DROP TABLE IF EXISTS `totalfishrechargelog`;
CREATE TABLE `totalfishrechargelog` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `UserID` int(10) unsigned NOT NULL,
  `UseRMBSum` int(10) unsigned NOT NULL,
  `orderid` varchar(256) NOT NULL,
  `channelOrderid` varchar(256) NOT NULL,
  `channelLabel` varchar(256) NOT NULL,
  `ChannelCode` varchar(256) NOT NULL,
  `AddGlobel` int(10) unsigned NOT NULL,
  `AddCurrceySum` int(10) unsigned NOT NULL,
  `LogTime` datetime NOT NULL,
  `OldGlobel` int(10) unsigned NOT NULL DEFAULT '0',
  `OldCurrcey` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of totalfishrechargelog
-- ----------------------------

-- ----------------------------
-- Procedure structure for `FishAddAnnouncementInfoLog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddAnnouncementInfoLog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddAnnouncementInfoLog`(IN `NickName` varchar(11),IN `ShopID` tinyint unsigned,IN `ShopOnlyID` tinyint unsigned,IN `NowTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('fishannouncementinfoLog','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like fishannouncementinfoLog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(NickName,ShopID,ShopOnlyID,LogTime) values('
					,"'",NickName,"'",',',"'",ShopID,"'",',',"'",ShopOnlyID,"'",',',"'",NowTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddEntityItemLog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddEntityItemLog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddEntityItemLog`(IN `ID` int unsigned,IN `OrderStates` varchar(256),IN `ItemID` int unsigned,IN `ItemSum` int unsigned,IN `ShopLogTime` datetime,IN `HandleTime` datetime,IN `UserID` int unsigned,IN `Address` varchar(256),IN `Phone` bigint unsigned,IN `IDEntity` varchar(256),IN `Name` varchar(256),IN `OrderNumber` varchar(256),IN `MedalNum` int unsigned,IN `NowMedalNum` int unsigned,IN `HandleIP` varchar(16))
BEGIN
	insert into fishentityitemlog(ID,OrderStates,ItemID,ItemSum,ShopLogTime,HandleTime,UserID,Address,Phone,IDEntity,Name,OrderNumber,MedalNum,NowMedalNum,HandleIP)
	values(ID,OrderStates,ItemID,ItemSum,ShopLogTime,HandleTime,UserID,Address,Phone,IDEntity,Name,OrderNumber,MedalNum,NowMedalNum,HandleIP);
	select ROW_COUNT();
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddLogCarTableInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddLogCarTableInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddLogCarTableInfo`(IN `BrandUserID` int unsigned,IN `AreaGlobel` varchar(1024),IN `ResultIndex` smallint unsigned,IN `BrandGlobel` bigint,IN `SystemGlobel` bigint,IN `RoleSum` int unsigned,IN `LogTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('fishcartableinfo','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like fishcartableinfo;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(BrandUserID,AreaGlobel,ResultIndex,BrandGlobel,SystemGlobel,RoleSum,LogTime) values('
					,"'",BrandUserID,"'",',',"'",AreaGlobel,"'",',',"'",ResultIndex,"'",',',"'",BrandGlobel,"'",',',"'",SystemGlobel,"'",',',"'",RoleSum,"'",',',"'",LogTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;

END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddLogDialTableInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddLogDialTableInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddLogDialTableInfo`(IN `BrandUserID` int unsigned,IN `AreaData` varchar(256),IN `AreaGlobel` varchar(1024),IN `ResultIndex` smallint unsigned,IN `BrandGlobel` bigint,IN `SystemGlobel` bigint,IN `RoleSum` int unsigned,IN `LogTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('fishdialtableinfo','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like fishdialtableinfo;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(BrandUserID,AreaData,AreaGlobel,ResultIndex,BrandGlobel,SystemGlobel,RoleSum,LogTime) values('
					,"'",BrandUserID,"'",',',"'",AreaData,"'",',',"'",AreaGlobel,"'",',',"'",ResultIndex,"'",',',"'",BrandGlobel,"'",',',"'",SystemGlobel,"'",',',"'",RoleSum,"'",',',"'",LogTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddLogInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddLogInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddLogInfo`(IN `UserID` int unsigned,IN `TypeID` int unsigned,IN `TypeSum` int,IN `Param` int unsigned,IN `Info` varchar(25),IN `LogTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('FishLog','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like FishLog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(UserID,TypeID,TypeSum,Info,Param,LogTime) values('
					,"'",UserID,"'",',',"'",TypeID,"'",',',"'",TypeSum,"'",',',"'",Info,"'",',',"'",Param,"'",",","'",LogTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddLogMonthInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddLogMonthInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddLogMonthInfo`(IN `UserID` int unsigned,IN `MonthID` tinyint unsigned,IN `MonthIndex` int unsigned,IN `MonthScore` int unsigned,IN `MonthSkillSum` int unsigned,IN `MonthAddGlobelSum` tinyint unsigned,IN `MonthRewardID` smallint unsigned,IN `LogTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('FishLogMonthInfo','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like FishLogMonthInfo;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(UserID,MonthID,MonthIndex,MonthScore,MonthSkillSum,MonthAddGlobelSum,MonthRewardID,LogTime) values('
					,"'",UserID,"'",',',"'",MonthID,"'",',',"'",MonthIndex,"'",',',"'",MonthScore,"'",',',"'",MonthSkillSum,"'",',',"'",MonthAddGlobelSum,"'",',',"'",MonthRewardID,"'",",","'",LogTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddLogNiuNiuTableInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddLogNiuNiuTableInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddLogNiuNiuTableInfo`(IN `BrandArray1` varchar(32),IN `BrandArray2` varchar(32),IN `BrandArray3` varchar(32),IN `BrandArray4` varchar(32),IN `BrandArray5` varchar(32),IN `UserSum` int unsigned,IN `ChangeGlobelSum` bigint,IN `SystemGlobelSum` bigint,IN `LogTime` datetime)
BEGIN

	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('fishniuniutableinfo','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like fishniuniutableinfo;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(BrandArray1,BrandArray2,BrandArray3,BrandArray4,BrandArray5,ChangeGlobelSum,SystemGlobelSum,LogTime,UserSum) values('
					,"'",BrandArray1,"'",',',"'",BrandArray2,"'",',',"'",BrandArray3,"'",',',"'",BrandArray4,"'",',',"'",BrandArray5,"'",',',"'",ChangeGlobelSum,"'",',',"'",SystemGlobelSum,"'",',',"'",LogTime,"'",',',"'",UserSum,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddLotteryInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddLotteryInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddLotteryInfo`(IN `UserID` int unsigned,IN `LotteryID` tinyint unsigned,IN `RewardID` smallint unsigned,IN `LogTime` datetime)
BEGIN
	insert into fishlotteryinfo(UserID,LotteryID,RewardID,LogTime)
	values (UserID,LotteryID,RewardID,LogTime);
	select ROW_COUNT();
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddPhonePayLog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddPhonePayLog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddPhonePayLog`(IN `OrderID` bigint unsigned,IN `FacePrice` int unsigned,IN `UserID` int unsigned,IN `PhoneNumber` bigint unsigned,IN `OrderStates` varchar(256),IN `LogTime` datetime)
BEGIN
	set @LogOrderID = 0;
	select OrderID into @LogOrderID from FishPhonePayLog where FishPhonePayLog.OrderID = OrderID;
	if @LogOrderID = 0 then
			insert into FishPhonePayLog(OrderID,UserID,FacePrice,PhoneNumber,OrderStates,LogTime)
			values(OrderID,UserID,FacePrice,PhoneNumber,OrderStates,LogTime);
	else
			update FishPhonePayLog set FishPhonePayLog.OrderStates = OrderStates,FishPhonePayLog.UserID= UserID,
									FishPhonePayLog.PhoneNumber = PhoneNumber,FishPhonePayLog.LogTime = LogTime where FishPhonePayLog.OrderID = OrderID;
	end if;
	SELECT ROW_COUNT();
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddRechargeInfoLog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddRechargeInfoLog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddRechargeInfoLog`(IN `UserID` int unsigned,IN `channelLabel` varchar(256),IN `OrderID` varchar(256),IN `channelorderid` varchar(256),IN `UseRMBSum` int unsigned,IN `AddGlobel` int unsigned,IN `AddCurrcey` int unsigned,IN `AddOrderUserName` varchar(16),IN `AddOrderUserIP` varchar(16),IN `AddOrderLogTime` datetime)
BEGIN
	insert into FishAddRechargeLog(UserID,channelLabel,orderid,channelOrderid,UseRMBSum,AddGlobel,AddCurrcey,AddOrderUserName,AddOrderUserIP,AddOrderLogTime)
		values(UserID,channelLabel,OrderID,channelorderid,UseRMBSum,AddGlobel,AddCurrcey,AddOrderUserName,AddOrderUserIP,AddOrderLogTime);
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddRechargeLog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddRechargeLog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddRechargeLog`(IN `OrderStates` varchar(256),IN `OrderID` varchar(256),IN `UserID`  int unsigned,IN `ChannelCode` varchar(256),IN `ChannelOrderID` varchar(256),IN `ChannelLabel` varchar(256),IN `ShopItemID` int unsigned,IN `Price` int unsigned,IN `FreePrice` int unsigned,IN `OldGlobelNum` int unsigned,IN `OldCurrceyNum` int unsigned,IN `AddGlobelNum` int unsigned,IN `AddCurrceyNum` int unsigned,IN `LogTime` datetime,IN `AddRewardID` int unsigned)
BEGIN
  insert into FishRechargeLog(OrderStates,OrderID,UserID,ChannelCode,channelOrderid,channelLabel,ShopItemID,Price,FreePrice,OldGlobelNum,OldCurrceyNum,AddGlobelSum,AddCurrceySum,LogTime,AddRewardID)
		                   values(OrderStates,OrderID,UserID,ChannelCode,ChannelOrderID,ChannelLabel,ShopItemID,Price,FreePrice,OldGlobelNum,OldCurrceyNum,AddGlobelNum,AddCurrceyNum,LogTime,AddRewardID);
	Select ROW_COUNT();
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `Fishaddroleentityitemfinishlog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `Fishaddroleentityitemfinishlog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `Fishaddroleentityitemfinishlog`(IN `UserID` int unsigned,IN `ItemID` int unsigned,IN `ItemSum` smallint unsigned,IN `Address` varchar(128),IN `Phone` varchar(32),IN `Name` varchar(11),IN `NowTime` datetime,IN `OrderID` varchar(64),IN `HandleTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('Fishentityitemfinishlog','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like Fishentityitemfinishlog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	insert into totalfishentityitemfinishlog(UserID,OrderID,HandleTime)
		values(UserID,OrderID,HandleTime);

	set @InsertStr = concat('insert into ',@LogName,'(UserID,OrderID,HandleTime) values('
					,"'",UserID,"'",',',"'",OrderID,"'",',',',',"'",HandleTime,"'",',',');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddRoleEntityItemLog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddRoleEntityItemLog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddRoleEntityItemLog`(IN `UserID` int unsigned,IN `ItemID` int unsigned,IN `ItemSum` smallint unsigned,IN `Address` varchar(128),IN `Phone` varchar(32),IN `Name` varchar(11),IN `NowTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('FishEntityItemLog','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like FishEntityItemLog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(UserID,ItemID,ItemSum,Address,Phone,Name,ShopTime) values('
					,"'",UserID,"'",',',"'",ItemID,"'",',',"'",ItemSum,"'",',',"'",Address,"'",',',"'",Phone,"'",',',"'",Name,"'",',',"'",NowTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddRoleOnlineLog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddRoleOnlineLog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddRoleOnlineLog`(IN `UserID` int unsigned,IN `IsOnline` bit,IN `GlobelSum` int unsigned,IN `MedalSum` int unsigned,IN `CurrceySum` int unsigned,IN `MacAddress` varchar(57),IN `IPAddress` varchar(17),IN `LogTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('Fishroleonlinelog','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like Fishroleonlinelog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(UserID,IsOnline,GlobelSum,MadelSum,CurrceySum,MacAddress,IpAddress,LogTime) values('
					,"'",UserID,"'",',',"'",IsOnline,"'",',',"'",GlobelSum,"'",',',"'",MedalSum,"'",',',"'",CurrceySum,"'",',',"'",MacAddress,"'",',',"'",IPAddress,"'",',',"'",LogTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddRoleTableLog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddRoleTableLog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddRoleTableLog`(IN `UserID` int unsigned,IN `TableTypeID` tinyint unsigned,IN `TableMonthID` tinyint unsigned,IN `GlobelNum` int unsigned,IN `MedalNum` int unsigned,IN `CurrceyNum` int unsigned,IN `JoinOrLeave` bit,IN `LogTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('fishroletablelog','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like fishroletablelog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(UserID,TableTypeID,TableMonthID,GlobelNum,MedalNum,CurrceyNum,JoinOrLeave,LogTime) values('
					,"'",UserID,"'",',',"'",TableTypeID,"'",',',"'",TableMonthID,"'",',',"'",GlobelNum,"'",',',"'",MedalNum,"'",',',"'",CurrceyNum,"'",',',"'",JoinOrLeave,"'",',',"'",LogTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishAddUserMailLog`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishAddUserMailLog`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishAddUserMailLog`(IN `SrcUserID` int unsigned,IN `DestUserID` int unsigned,IN `Context` varchar(64),IN `RewardID`  smallint unsigned,IN `NowTime` datetime)
BEGIN
	
	
  set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('FishMailLog','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like FishMailLog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(SrcUserID,DestUserID,Context,RewardID,IsRead,IsExistsReward,SendTime) values('
					,"'",SrcUserID,"'",',',"'",DestUserID,"'",',',"'",Context,"'",',',"'",RewardID,"'",',',0,',',(RewardID!=0),',',"'",NowTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishCreateLogTable`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishCreateLogTable`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishCreateLogTable`(IN `NowYear` int unsigned,IN `NowMonth` int unsigned,IN `NowDay` int unsigned)
BEGIN
	
	
	set @LogName = CONCAT('FishLog','_',NowYear,'_',NowMonth,'_',NowDay);
	set @SqlStr = concat('create table if not exists ',@LogName,' like FishLog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @LogName = CONCAT('fishroletablelog','_',NowYear,'_',NowMonth,'_',NowDay);
	set @SqlStr = concat('create table if not exists ',@LogName,' like fishroletablelog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;


	set @LogName = CONCAT('Fishroleonlinelog','_',NowYear,'_',NowMonth,'_',NowDay);
	set @SqlStr = concat('create table if not exists ',@LogName,' like Fishroleonlinelog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @LogName = CONCAT('fishniuniutableinfo','_',NowYear,'_',NowMonth,'_',NowDay);
	set @SqlStr = concat('create table if not exists ',@LogName,' like fishniuniutableinfo;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @LogName = CONCAT('FishExChangeLog','_',NowYear,'_',NowMonth,'_',NowDay);
	set @SqlStr = concat('create table if not exists ',@LogName,' like FishExChangeLog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @LogName = CONCAT('FishLogMonthInfo','_',NowYear,'_',NowMonth,'_',NowDay);
	set @SqlStr = CONCAT('create table if not exists ',@LogName,' like FishLogMonthInfo;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @LogName = CONCAT('fishdialtableinfo','_',NowYear,'_',NowMonth,'_',NowDay);
	set @SqlStr = CONCAT('create table if not exists ',@LogName,' like fishdialtableinfo;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @LogName = CONCAT('fishcartableinfo','_',NowYear,'_',NowMonth,'_',NowDay);
	set @SqlStr = CONCAT('create table if not exists ',@LogName,' like fishcartableinfo;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	select 1;
END
;;
DELIMITER ;

-- ----------------------------
-- Procedure structure for `FishLogExChangeInfo`
-- ----------------------------
DROP PROCEDURE IF EXISTS `FishLogExChangeInfo`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishLogExChangeInfo`(IN `UserID` int unsigned,IN `TypeID` tinyint unsigned,IN `ExChangeCode` varchar(17),IN `LogTime` datetime)
BEGIN
	set @NowYear = year(now());
	set @NowMonth = month(now());
	set @NowDay = day(now());
	set @LogName = CONCAT('FishExChangeLog','_',@NowYear,'_',@NowMonth,'_',@NowDay);

	set @SqlStr = concat('create table if not exists ',@LogName,' like FishExChangeLog;');
	prepare stmt from @SqlStr;
	EXECUTE stmt;

	set @InsertStr = concat('insert into ',@LogName,'(UserID,TypeID,ExChange,LogTime) values('
					,"'",UserID,"'",',',"'",TypeID,"'",',',"'",ExChangeCode,"'",',',"'",LogTime,"'",');');
	
	prepare stmt2 from @InsertStr;
	EXECUTE stmt2;

	select 1;

END
;;
DELIMITER ;
