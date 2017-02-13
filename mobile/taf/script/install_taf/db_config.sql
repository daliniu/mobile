-- MySQL dump 10.13  Distrib 5.5.31, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: db_config
-- ------------------------------------------------------
-- Server version	5.5.31-0ubuntu0.12.04.2

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `t_config_dbconnects`
--

DROP TABLE IF EXISTS `t_config_dbconnects`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_config_dbconnects` (
  `ID` smallint(6) unsigned NOT NULL AUTO_INCREMENT,
  `NAME` char(50) DEFAULT NULL,
  `HOST` char(50) NOT NULL DEFAULT '',
  `USER` char(50) NOT NULL DEFAULT '',
  `PASSWORD` char(50) NOT NULL DEFAULT '',
  `CHARSET` char(30) DEFAULT NULL,
  `DBNAME` char(50) NOT NULL DEFAULT '',
  `ABOUT` char(200) DEFAULT NULL,
  `POSTTIME` datetime DEFAULT NULL,
  `LASTUSER` char(50) DEFAULT NULL,
  `PORT` char(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `IDX_NAME` (`NAME`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=gbk ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_dbconnects`
--

LOCK TABLES `t_config_dbconnects` WRITE;
/*!40000 ALTER TABLE `t_config_dbconnects` DISABLE KEYS */;
INSERT INTO `t_config_dbconnects` VALUES (1,'Config.Menus','127.0.0.1','root','3e91eb88c2e115b0','gbk','db_config','113','2013-12-27 00:00:00','shiqingfan','3306'),(2,'Config.DbConnect','127.0.0.1','root','3e91eb88c2e115b0','gbk','db_config',NULL,'2013-12-27 00:00:00','shiqingfan','3306'),(4,'MDW.UNIQID','127.0.0.1','root','3e91eb88c2e115b0','utf8','db_mbox',' 11','2013-12-27 00:00:00','shiqiangfan','3306'),(5,'MDW.USER','127.0.0.1','root','3e91eb88c2e115b0','gbk','db_dw_user_0','1','0000-00-00 00:00:00','shiqingfan','3306'),(6,'MDW.FRIEND','172.19.103.15','root','asdfOMV763F','gbk','db_friends_0','1','0000-00-00 00:00:00','shiqingfan','6301');
/*!40000 ALTER TABLE `t_config_dbconnects` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_config_fields`
--

DROP TABLE IF EXISTS `t_config_fields`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_config_fields` (
  `ID` smallint(6) unsigned NOT NULL AUTO_INCREMENT,
  `QUERYID` int(11) NOT NULL DEFAULT '0',
  `NAME` varchar(50) NOT NULL DEFAULT '',
  `SEARCHNAME` varchar(50) NOT NULL DEFAULT '',
  `ABOUT` varchar(100) NOT NULL DEFAULT '',
  `ISMODIFY` char(1) NOT NULL DEFAULT '1',
  `ISSEARCH` char(1) DEFAULT NULL,
  `ISVISIBLE` char(1) DEFAULT NULL,
  `ISEQUAL` char(1) NOT NULL DEFAULT '0',
  `ISNULL` char(1) DEFAULT NULL,
  `ORDERBY` smallint(1) DEFAULT '0',
  `EDITWIDTH` int(11) DEFAULT NULL,
  `EDITHEIGHT` int(11) DEFAULT NULL,
  `SEARCHSIZE` int(11) DEFAULT NULL,
  `DATATYPE` int(11) DEFAULT NULL,
  `RAWVALUE` varchar(200) DEFAULT NULL,
  `OPTIONS` text,
  `POSTTIME` datetime DEFAULT NULL,
  `LASTUSER` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `IDX_QUERYID_NAME` (`QUERYID`,`NAME`),
  KEY `IDX_QUERYID` (`QUERYID`)
) ENGINE=MyISAM AUTO_INCREMENT=47 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_fields`
--

LOCK TABLES `t_config_fields` WRITE;
/*!40000 ALTER TABLE `t_config_fields` DISABLE KEYS */;
INSERT INTO `t_config_fields` VALUES (1,1,'NAME','NAME','NAME','1','1','1','0',NULL,1,100,0,10,1,NULL,NULL,'2013-12-27 22:04:03',NULL),(2,1,'HOST','HOST','HOST','1','1','1','0',NULL,2,100,0,10,1,NULL,NULL,'2013-12-27 22:04:03',NULL),(3,1,'USER','USER','USER','1','1','1','0',NULL,3,100,0,10,1,NULL,NULL,'2013-12-27 22:04:03',NULL),(4,1,'PASSWORD','PASSWORD','PASSWORD','1','1','1','0',NULL,4,100,0,10,1,NULL,NULL,'2013-12-27 22:04:03',NULL),(5,1,'CHARSET','CHARSET','CHARSET','1','1','1','0',NULL,5,100,0,10,1,NULL,NULL,'2013-12-27 22:04:03',NULL),(6,1,'DBNAME','DBNAME','DBNAME','1','1','1','0',NULL,6,100,0,10,1,NULL,NULL,'2013-12-27 22:04:03',NULL),(7,1,'ABOUT','ABOUT','ABOUT','1','1','1','0',NULL,7,100,0,10,1,NULL,NULL,'2013-12-27 22:04:03',NULL),(8,1,'POSTTIME','POSTTIME','POSTTIME','1','1','1','0',NULL,8,100,0,10,9,NULL,NULL,'2013-12-27 22:04:03',NULL),(9,1,'LASTUSER','LASTUSER','LASTUSER','1','1','1','0',NULL,9,100,0,10,1,NULL,NULL,'2013-12-27 22:04:03',NULL),(10,1,'PORT','PORT','PORT','1','1','1','0',NULL,10,100,0,10,1,NULL,NULL,'2013-12-27 22:04:03',NULL),(11,2,'PID','PID','PID','1','1','1','0','1',1,100,0,10,1,'','','2014-02-20 18:48:42','fanshiqing'),(12,2,'NAME','NAME','NAME','1','1','1','0','0',2,400,60,10,2,'','','2014-02-20 18:48:42','fanshiqing'),(13,2,'HREF','HREF','HREF','1','1','1','0','0',3,400,60,10,2,'','','2014-02-20 18:48:42','fanshiqing'),(14,2,'TYPE','TYPE','TYPE','1','1','1','0','0',4,100,0,10,1,'','','2014-02-20 18:48:42','fanshiqing'),(15,2,'ORDERBY','ORDERBY','ORDERBY','1','1','0','0','0',5,100,0,10,1,'','','2014-02-20 18:48:42','fanshiqing'),(16,2,'POSTTIME','POSTTIME','POSTTIME','1','1','0','0','0',6,100,0,10,9,'','','2014-02-20 18:48:42','fanshiqing'),(17,2,'LASTUSER','LASTUSER','LASTUSER','1','1','1','0','0',7,400,60,10,2,'','','2014-02-20 18:48:42','fanshiqing'),(18,3,'NAME','NAME','NAME','1','1','1','0',NULL,1,100,0,10,1,NULL,NULL,'2013-12-27 22:18:51',NULL),(19,3,'HOST','HOST','HOST','1','1','1','0',NULL,2,100,0,10,1,NULL,NULL,'2013-12-27 22:18:51',NULL),(20,3,'USER','USER','USER','1','1','1','0',NULL,3,100,0,10,1,NULL,NULL,'2013-12-27 22:18:51',NULL),(21,3,'PASSWORD','PASSWORD','PASSWORD','1','1','1','0',NULL,4,100,0,10,1,NULL,NULL,'2013-12-27 22:18:51',NULL),(22,3,'CHARSET','CHARSET','CHARSET','1','1','1','0',NULL,5,100,0,10,1,NULL,NULL,'2013-12-27 22:18:51',NULL),(23,3,'DBNAME','DBNAME','DBNAME','1','1','1','0',NULL,6,100,0,10,1,NULL,NULL,'2013-12-27 22:18:51',NULL),(24,3,'ABOUT','ABOUT','ABOUT','1','1','1','0',NULL,7,100,0,10,1,NULL,NULL,'2013-12-27 22:18:51',NULL),(25,3,'PORT','PORT','PORT','1','1','1','0',NULL,10,100,0,10,1,NULL,NULL,'2013-12-27 22:18:51',NULL),(26,4,'id','id','id','1','1','1','0',NULL,0,100,0,10,1,NULL,NULL,'2014-02-20 17:46:41',NULL),(27,4,'app_id','app_id','app_id','1','1','1','0',NULL,1,400,60,10,2,NULL,NULL,'2014-02-20 17:46:41',NULL),(28,4,'lock_flag','lock_flag','lock_flag','1','1','1','0',NULL,2,100,0,10,1,NULL,NULL,'2014-02-20 17:46:41',NULL),(29,4,'modify_time','modify_time','modify_time','1','1','1','0',NULL,3,100,0,10,9,NULL,NULL,'2014-02-20 17:46:41',NULL),(30,5,'yyuid','yyuid','yyuid','1','1','1','0','0',0,100,0,10,1,'','','2014-02-20 18:59:21','fanshiqing'),(31,5,'yynum','yynum','yynum','1','1','1','0','0',1,100,0,10,1,'','','2014-02-20 18:59:21','fanshiqing'),(32,5,'dwname','dwname','dwname','1','1','1','0','0',2,400,60,10,2,'','','2014-02-20 18:59:21','fanshiqing'),(33,5,'phone','phone','phone','1','1','1','0','0',3,400,60,10,2,'','','2014-02-20 18:59:21','fanshiqing'),(34,5,'nick','nick','nick','1','1','1','0','0',4,400,60,10,2,'','','2014-02-20 18:59:21','fanshiqing'),(35,5,'iconurl','iconurl','iconurl','1','1','1','0','0',5,400,60,10,2,'','','2014-02-20 18:59:21','fanshiqing'),(36,5,'constellation','constellation','constellation','1','1','1','0','0',6,400,60,10,2,'','','2014-02-20 18:59:21','fanshiqing'),(37,5,'remark','remark','remark','1','1','1','0','0',7,400,60,10,2,'','','2014-02-20 18:59:21','fanshiqing'),(38,5,'birth','birth','birth','1','1','1','0','0',8,100,0,10,8,'','','2014-02-20 18:59:21','fanshiqing'),(39,5,'extended','extended','extended','1','1','0','0','0',9,100,0,10,1,'','','2014-02-20 18:59:21','fanshiqing'),(40,5,'modify_time','modify_time','modify_time','1','1','1','0','0',10,100,0,10,9,'','','2014-02-20 18:59:21','fanshiqing'),(41,5,'gender','gender','gender','1','1','1','0','0',11,100,0,10,1,'','','2014-02-20 18:59:21','fanshiqing'),(42,5,'first_login_time','first_login_time','first_login_time','1','1','1','0','0',12,100,0,10,9,'','','2014-02-20 18:59:21','fanshiqing'),(43,5,'last_login_time','last_login_time','last_login_time','1','1','1','0','0',13,100,0,10,9,'','','2014-02-20 18:59:21','fanshiqing'),(44,6,'yyuid','yyuid','yyuid','1','1','1','0',NULL,0,100,0,10,1,NULL,NULL,'2014-02-20 19:03:07',NULL),(45,6,'friend_list','friend_list','friend_list','1','1','1','0',NULL,1,100,0,10,1,NULL,NULL,'2014-02-20 19:03:07',NULL),(46,6,'modify_time','modify_time','modify_time','1','1','1','0',NULL,2,100,0,10,9,NULL,NULL,'2014-02-20 19:03:07',NULL);
/*!40000 ALTER TABLE `t_config_fields` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_config_menus`
--

DROP TABLE IF EXISTS `t_config_menus`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_config_menus` (
  `ID` smallint(6) unsigned NOT NULL AUTO_INCREMENT,
  `PID` smallint(5) unsigned DEFAULT NULL,
  `NAME` varchar(50) NOT NULL DEFAULT '',
  `HREF` varchar(255) NOT NULL DEFAULT '',
  `TYPE` tinyint(3) NOT NULL DEFAULT '1',
  `ORDERBY` smallint(6) unsigned NOT NULL DEFAULT '1',
  `POSTTIME` datetime DEFAULT NULL,
  `LASTUSER` varchar(120) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_menus`
--

LOCK TABLES `t_config_menus` WRITE;
/*!40000 ALTER TABLE `t_config_menus` DISABLE KEYS */;
INSERT INTO `t_config_menus` VALUES (1,NULL,'MDW','MDW.UNIQ',2,1,'2013-12-27 00:00:00','shiqingfan'),(2,2,'MDW','MDW.UNIQ',0,0,'2014-02-20 00:00:00','shiqingfan'),(3,1,'唯一id','template/show.jsp?OPNAME=MDW.UNIQ',2,1,'0000-00-00 00:00:00','shiqingfan'),(4,1,'用户信息号','template/show.jsp?OPNAME=MDW.USER',2,1,'0000-00-00 00:00:00','shiqingfan'),(5,1,'好友信息','template/show.jsp?OPNAME=MDW.FRIENDS',2,2,'0000-00-00 00:00:00','shiqingfan');
/*!40000 ALTER TABLE `t_config_menus` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_config_querys`
--

DROP TABLE IF EXISTS `t_config_querys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_config_querys` (
  `ID` smallint(6) unsigned NOT NULL AUTO_INCREMENT,
  `OPNAME` varchar(200) DEFAULT NULL,
  `QUERYKEY` varchar(20) DEFAULT NULL,
  `QUERYPOSTTIME` varchar(50) DEFAULT NULL,
  `QUERYLASTUSER` varchar(50) DEFAULT NULL,
  `SERVERNAME` varchar(50) NOT NULL DEFAULT '',
  `DBCONNECTID` smallint(6) NOT NULL DEFAULT '0',
  `PAGESIZE` tinyint(3) unsigned NOT NULL DEFAULT '10',
  `MYSQL` text NOT NULL,
  `ORDERSQL` text NOT NULL,
  `TABLENAME` varchar(50) DEFAULT NULL,
  `OTHERCALL` text,
  `POSTTIME` datetime DEFAULT NULL,
  `LASTUSER` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `IDX_OPNAME` (`OPNAME`)
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_querys`
--

LOCK TABLES `t_config_querys` WRITE;
/*!40000 ALTER TABLE `t_config_querys` DISABLE KEYS */;
INSERT INTO `t_config_querys` VALUES (1,'Config.DbConnect','ID','20131227','shiqfang','',2,10,'select *  from t_config_dbconnects','','t_config_dbconnects',NULL,'2013-12-27 22:09:18','DevTest'),(2,'Config.Menus','ID','20131227','shiqingfan','',1,10,'select *  from t_config_menus','','t_config_menus',NULL,'2013-12-27 22:09:50','DevTest'),(3,'MLOL.Test','ID','POSTTIME','LASTUSER','',2,20,'select * from t_config_dbconnects ','','t_config_dbconnects',NULL,'2013-12-27 22:29:46','DevTest'),(4,'MDW.UNIQ','ID','POSTTIME','LASTUSER','',4,20,'select * from t_uniq_id','','t_uniq_id',NULL,'2014-02-20 17:48:16','fanshiqing'),(5,'MDW.USER','ID','POSTTIME','LASTUSER','',5,20,'select * from t_user_0','','t_user_0',NULL,'2014-02-20 18:59:00','fanshiqing'),(6,'MDW.FRIENDs','ID','POSTTIME','LASTUSER','',6,20,'select * from t_friends_0','','t_friends_0',NULL,'2014-02-20 19:03:07','fanshiqing');
/*!40000 ALTER TABLE `t_config_querys` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2014-04-15 16:03:48
