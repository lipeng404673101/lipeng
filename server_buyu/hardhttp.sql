

/*添加图片信息 获得userID添加指定的图片*/

DROP PROCEDURE IF EXISTS `FishUpdateHardhttp`;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `FishUpdateHardhttp`(IN `UserID` int unsigned,IN `szHeadHttp` varchar(1000))
BEGIN
	update accountinfo set accountinfo.szHeadHttp =szHeadHttp  where accountinfo.UserID = UserID;
	select ROW_COUNT();
END
;;
DELIMITER ;