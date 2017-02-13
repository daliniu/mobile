-- MySQL dump 10.13  Distrib 5.5.31, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: db_ice
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
-- Table structure for table `t_app_servers`
--

DROP TABLE IF EXISTS `t_app_servers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_app_servers` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `APP` varchar(20) DEFAULT '',
  `SERVERNAME` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_app_servers`
--

LOCK TABLES `t_app_servers` WRITE;
/*!40000 ALTER TABLE `t_app_servers` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_app_servers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_auto_test_cases`
--

DROP TABLE IF EXISTS `t_auto_test_cases`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_auto_test_cases` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `CASENAME` varchar(200) DEFAULT NULL,
  `INTERFACEID` int(11) DEFAULT NULL,
  `REVALUE` text,
  `PARAMVALUE` text,
  PRIMARY KEY (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_auto_test_cases`
--

LOCK TABLES `t_auto_test_cases` WRITE;
/*!40000 ALTER TABLE `t_auto_test_cases` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_auto_test_cases` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_auto_test_interfaces`
--

DROP TABLE IF EXISTS `t_auto_test_interfaces`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_auto_test_interfaces` (
  `ID` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `OBJNAME` varchar(100) DEFAULT NULL,
  `FUNCNAME` varchar(50) DEFAULT NULL,
  `RETYPE` varchar(50) DEFAULT NULL,
  `REMODIFY` tinyint(3) DEFAULT NULL,
  `PARAMTYPE` text,
  `OUTPARAMTYPE` text,
  `POSTTIME` datetime DEFAULT NULL,
  `LASTUSER` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `IDX_OBJ_FUNC` (`OBJNAME`,`FUNCNAME`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_auto_test_interfaces`
--

LOCK TABLES `t_auto_test_interfaces` WRITE;
/*!40000 ALTER TABLE `t_auto_test_interfaces` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_auto_test_interfaces` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_config_historys`
--

DROP TABLE IF EXISTS `t_server_config_historys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_config_historys` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `CONFIGID` int(11) NOT NULL DEFAULT '0',
  `REASON` varchar(255) DEFAULT NULL,
  `content` longtext,
  `POSTTIME` datetime DEFAULT NULL,
  `LASTUSER` varchar(50) DEFAULT NULL,
  `REASON_SELECT` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `IDX_CONFIGID` (`CONFIGID`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_config_historys`
--

LOCK TABLES `t_server_config_historys` WRITE;
/*!40000 ALTER TABLE `t_server_config_historys` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_server_config_historys` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_configs`
--

DROP TABLE IF EXISTS `t_server_configs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_configs` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `SERVER` varchar(50) DEFAULT NULL,
  `FILENAME` varchar(100) DEFAULT NULL,
  `CONFIG` longtext,
  `POSTTIME` datetime DEFAULT NULL,
  `LASTUSER` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `IDX_APP_SERVER_FILENAME` (`SERVER`,`FILENAME`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_configs`
--

LOCK TABLES `t_server_configs` WRITE;
/*!40000 ALTER TABLE `t_server_configs` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_server_configs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_notifys`
--

DROP TABLE IF EXISTS `t_server_notifys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_notifys` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `TOPIC` varchar(50) DEFAULT NULL,
  `SERVERID` varchar(200) DEFAULT NULL,
  `OBJECTID` varchar(80) DEFAULT NULL,
  `THREADID` int(11) unsigned DEFAULT NULL,
  `COMMAND` varchar(120) DEFAULT NULL,
  `RESULT` text,
  `POSTTIME` datetime DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `IDX_TOPIC` (`TOPIC`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_notifys`
--

LOCK TABLES `t_server_notifys` WRITE;
/*!40000 ALTER TABLE `t_server_notifys` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_server_notifys` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_patchs`
--

DROP TABLE IF EXISTS `t_server_patchs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_patchs` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `SERVER` varchar(50) DEFAULT NULL,
  `TGZ` varchar(255) DEFAULT NULL,
  `UPDATETEXT` text,
  `PUBLISH` char(1) DEFAULT NULL,
  `PUBLISHTIME` datetime DEFAULT NULL,
  `UPLOADTIME` datetime DEFAULT NULL,
  `POSTTIME` datetime DEFAULT NULL,
  `LASTUSER` varchar(50) DEFAULT NULL,
  `REASON_SELECT` varchar(32) DEFAULT NULL,
  `DOCUMENT_COMPLETE` varchar(3) DEFAULT NULL,
  PRIMARY KEY (`ID`),
  KEY `IDX_APP` (`SERVER`),
  KEY `IDX_APP_SERVERNAME` (`SERVER`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_patchs`
--

LOCK TABLES `t_server_patchs` WRITE;
/*!40000 ALTER TABLE `t_server_patchs` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_server_patchs` ENABLE KEYS */;
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
