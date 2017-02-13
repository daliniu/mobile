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
) ENGINE=MyISAM AUTO_INCREMENT=4 DEFAULT CHARSET=gbk ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_dbconnects`
--

LOCK TABLES `t_config_dbconnects` WRITE;
/*!40000 ALTER TABLE `t_config_dbconnects` DISABLE KEYS */;
INSERT INTO `t_config_dbconnects` VALUES (1,'Config.Menus','127.0.0.1','root','3e91eb88c2e115b0','gbk','db_config',NULL,'2013-12-27 00:00:00','shiqingfan','3306'),(2,'Config.DbConnect','127.0.0.1','root','3e91eb88c2e115b0','gbk','db_config',NULL,'2013-12-27 00:00:00','shiqingfan','3306');
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
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_fields`
--

LOCK TABLES `t_config_fields` WRITE;
/*!40000 ALTER TABLE `t_config_fields` DISABLE KEYS */;
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
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_menus`
--

LOCK TABLES `t_config_menus` WRITE;
/*!40000 ALTER TABLE `t_config_menus` DISABLE KEYS */;
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
  `SQL` text NOT NULL,
  `ORDERSQL` text NOT NULL,
  `TABLENAME` varchar(50) DEFAULT NULL,
  `OTHERCALL` text,
  `POSTTIME` datetime DEFAULT NULL,
  `LASTUSER` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `IDX_OPNAME` (`OPNAME`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_querys`
--

LOCK TABLES `t_config_querys` WRITE;
/*!40000 ALTER TABLE `t_config_querys` DISABLE KEYS */;
INSERT INTO `t_config_querys` VALUES (1,'Config.DbConnect','ID',NULL,NULL,'',2,10,'select *  from t_config_dbconnects','','t_config_dbconnects',NULL,'2013-12-27 00:00:00','shiqingfan'),(2,'Config.Menus','ID',NULL,NULL,'',1,10,'select *  from t_config_menus','','t_config_menus',NULL,'2013-12-27 00:00:00','shiqingfan');
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

-- Dump completed on 2013-12-27 21:50:16
