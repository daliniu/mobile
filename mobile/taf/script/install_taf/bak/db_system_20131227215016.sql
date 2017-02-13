-- MySQL dump 10.13  Distrib 5.5.31, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: db_system
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
-- Table structure for table `t_app`
--

DROP TABLE IF EXISTS `t_app`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_app` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `application` varchar(90) DEFAULT NULL,
  `flag` int(11) DEFAULT '1',
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `application` (`application`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_app`
--

LOCK TABLES `t_app` WRITE;
/*!40000 ALTER TABLE `t_app` DISABLE KEYS */;
INSERT INTO `t_app` VALUES (1,'MLOL',1,'2013-12-25 21:48:54','DevTest'),(4,'taf',1,'2013-12-25 22:25:07','DevTest');
/*!40000 ALTER TABLE `t_app` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_app_users`
--

DROP TABLE IF EXISTS `t_app_users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_app_users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `app` varchar(20) DEFAULT '',
  `userid` varchar(50) DEFAULT NULL,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `IDX_APP` (`app`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_app_users`
--

LOCK TABLES `t_app_users` WRITE;
/*!40000 ALTER TABLE `t_app_users` DISABLE KEYS */;
INSERT INTO `t_app_users` VALUES (1,'MLOL','DevTest','2013-12-25 17:21:02','DevTest'),(2,'taf','DevTest','2013-12-25 22:11:57','DevTest'),(3,'Test','DevTest','2013-12-25 22:16:41','DevTest'),(4,'taf','DevTest','2013-12-25 22:25:07','DevTest');
/*!40000 ALTER TABLE `t_app_users` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_loginers`
--

DROP TABLE IF EXISTS `t_loginers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_loginers` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userid` char(30) NOT NULL DEFAULT '',
  `nickname` char(50) DEFAULT NULL,
  `depid` char(5) DEFAULT NULL,
  `depname` char(80) DEFAULT NULL,
  `groups` char(255) DEFAULT NULL,
  `regtime` datetime DEFAULT NULL,
  `login` int(11) DEFAULT NULL,
  `lastlogin` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `IDX_USERID` (`userid`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_loginers`
--

LOCK TABLES `t_loginers` WRITE;
/*!40000 ALTER TABLE `t_loginers` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_loginers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_user_logs`
--

DROP TABLE IF EXISTS `t_user_logs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_user_logs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userid` varchar(50) DEFAULT '',
  `app` varchar(50) DEFAULT NULL,
  `about` varchar(255) DEFAULT NULL,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `IDX_USERID` (`userid`),
  KEY `IDX_APP` (`app`),
  KEY `IDX_POSTTIME` (`posttime`)
) ENGINE=MyISAM AUTO_INCREMENT=146 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_user_logs`
--

LOCK TABLES `t_user_logs` WRITE;
/*!40000 ALTER TABLE `t_user_logs` DISABLE KEYS */;
INSERT INTO `t_user_logs` VALUES (20,'DevTest','taf','删除应用 [taf]','2013-12-25 22:24:57',NULL),(21,'DevTest','Test','删除应用 [Test]','2013-12-25 22:24:59',NULL),(22,'DevTest','taf','添加应用 [taf]','2013-12-25 22:25:07',NULL),(23,'DevTest','taf','添加应用 [taf] 负责人: [DevTest]','2013-12-25 22:25:07',NULL),(24,'DevTest','taf','修改 [taf.default] 模板','2013-12-25 22:33:44',NULL),(25,'DevTest','taf','修改 [taf.default] 模板','2013-12-25 22:36:03',NULL),(26,'DevTest','taf','修改 [taf.config] 模板','2013-12-25 22:38:20',NULL),(27,'DevTest','taf','修改 [taf.log] 模板','2013-12-25 22:38:51',NULL),(28,'DevTest','taf','修改 [taf.notify] 模板','2013-12-25 22:39:39',NULL),(29,'DevTest','taf','修改 [taf.notify] 模板','2013-12-25 22:40:31',NULL),(30,'DevTest','taf','修改 [taf.stat] 模板','2013-12-25 22:41:27',NULL),(31,'DevTest','taf','修改 [taf.notify] 模板','2013-12-25 22:41:44',NULL),(32,'DevTest','taf','修改 [taf.stat] 模板','2013-12-25 22:42:11',NULL),(33,'DevTest','taf','修改 [taf.patch] 模板','2013-12-25 22:42:58',NULL),(34,'DevTest','taf','修改 [taf.property] 模板','2013-12-25 22:43:43',NULL),(35,'DevTest','taf','修改 [taf.property] 模板','2013-12-25 22:44:24',NULL),(36,'DevTest','taf','修改 [taf.tafconfig_172.19.103.14] 模板','2013-12-25 23:16:16',NULL),(37,'DevTest','taf','修改 [taf.tafconfig.taf.tafconfig.ConfigObjAdapter] Adapter','2013-12-25 23:18:46',NULL),(38,'DevTest','taf','修改 [taf.tafconfig_172.19.103.14] 模板','2013-12-26 11:09:38',NULL),(39,'DevTest','taf','修改 [taf.tafstat_172.19.103.14] 模板','2013-12-26 15:28:47',NULL),(40,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 16:07:51',NULL),(41,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 16:07:51',NULL),(42,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 16:34:30',NULL),(43,'DevTest','taf','修改 [taf.tafstat_172.19.103.14] 模板','2013-12-26 16:34:47',NULL),(44,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 16:34:57',NULL),(45,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 16:34:57',NULL),(46,'DevTest','taf','修改 [taf.tafproperty_172.19.103.14] 模板','2013-12-26 16:39:07',NULL),(47,'DevTest','taf','停止 [taf.tafproperty_172.19.103.14]','2013-12-26 16:39:19',NULL),(48,'DevTest','taf','启动 [taf.tafproperty_172.19.103.14]','2013-12-26 16:39:19',NULL),(49,'DevTest','taf','停止 [taf.tafproperty_172.19.103.14]','2013-12-26 16:39:34',NULL),(50,'DevTest','taf','启动 [taf.tafproperty_172.19.103.14]','2013-12-26 16:39:34',NULL),(51,'DevTest','taf','修改 [taf.property] 模板','2013-12-26 16:45:39',NULL),(52,'DevTest','taf','停止 [taf.tafproperty_172.19.103.14]','2013-12-26 16:46:03',NULL),(53,'DevTest','taf','启动 [taf.tafproperty_172.19.103.14]','2013-12-26 16:46:03',NULL),(54,'DevTest','taf','修改 [taf.taflog_172.19.103.14] 模板','2013-12-26 16:47:18',NULL),(55,'DevTest','taf','停止 [taf.taflog_172.19.103.14]','2013-12-26 16:47:31',NULL),(56,'DevTest','taf','启动 [taf.taflog_172.19.103.14]','2013-12-26 16:47:31',NULL),(57,'DevTest','taf','修改 [taf.tafpatch_172.19.103.14] 模板','2013-12-26 16:48:38',NULL),(58,'DevTest','taf','停止 [taf.tafpatch_172.19.103.14]','2013-12-26 16:48:46',NULL),(59,'DevTest','taf','启动 [taf.tafpatch_172.19.103.14]','2013-12-26 16:48:46',NULL),(60,'DevTest','taf','修改 [taf.tafnotify_172.19.103.14] 模板','2013-12-26 16:52:39',NULL),(61,'DevTest','taf','停止 [taf.tafnotify_172.19.103.14]','2013-12-26 16:52:49',NULL),(62,'DevTest','taf','启动 [taf.tafnotify_172.19.103.14]','2013-12-26 16:52:49',NULL),(63,'DevTest','taf','修改 [taf.taflog.taf.taflog.LogObjAdapter] Adapter','2013-12-26 16:57:03',NULL),(64,'DevTest','taf','修改 [taf.tafstat.taf.tafstat.StatObjAdapter] Adapter','2013-12-26 16:57:23',NULL),(65,'DevTest','taf','修改 [taf.tafproperty.taf.tafproperty.PropertyObjAdapter] Adapter','2013-12-26 16:57:54',NULL),(66,'DevTest','taf','修改 [taf.tafpatch.taf.tafpatch.PatchObjAdapter] Adapter','2013-12-26 16:58:24',NULL),(67,'DevTest','taf','停止 [taf.tafconfig_172.19.103.14]','2013-12-26 16:58:34',NULL),(68,'DevTest','taf','启动 [taf.tafconfig_172.19.103.14]','2013-12-26 16:58:36',NULL),(69,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 16:58:43',NULL),(70,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 16:58:43',NULL),(71,'DevTest','taf','停止 [taf.taflog_172.19.103.14]','2013-12-26 16:58:48',NULL),(72,'DevTest','taf','启动 [taf.taflog_172.19.103.14]','2013-12-26 16:58:51',NULL),(73,'DevTest','taf','停止 [taf.tafnotify_172.19.103.14]','2013-12-26 16:58:56',NULL),(74,'DevTest','taf','启动 [taf.tafnotify_172.19.103.14]','2013-12-26 16:58:59',NULL),(75,'DevTest','taf','停止 [taf.tafpatch_172.19.103.14]','2013-12-26 16:59:03',NULL),(76,'DevTest','taf','启动 [taf.tafpatch_172.19.103.14]','2013-12-26 16:59:05',NULL),(77,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 17:00:20',NULL),(78,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 17:00:20',NULL),(79,'DevTest','taf','修改 [taf.tafnotify.taf.tafnotify.NotifyObjAdapter] Adapter','2013-12-26 17:50:59',NULL),(80,'DevTest','taf','停止 [taf.tafnotify_172.19.103.14]','2013-12-26 17:51:15',NULL),(81,'DevTest','taf','启动 [taf.tafnotify_172.19.103.14]','2013-12-26 17:51:16',NULL),(82,'DevTest','taf','修改 [taf.tafnotify.taf.tafnotify.NotifyObjAdapter] Adapter','2013-12-26 18:52:48',NULL),(83,'DevTest','taf','修改 [taf.tafconfig.taf.tafconfig.ConfigObjAdapter] Adapter','2013-12-26 18:52:59',NULL),(84,'DevTest','taf','修改 [taf.taflog.taf.taflog.LogObjAdapter] Adapter','2013-12-26 18:53:10',NULL),(85,'DevTest','taf','修改 [taf.tafnotify.taf.tafnotify.NotifyObjAdapter] Adapter','2013-12-26 18:53:32',NULL),(86,'DevTest','taf','修改 [taf.tafnotify.taf.tafnotify.NotifyObjAdapter] Adapter','2013-12-26 18:53:47',NULL),(87,'DevTest','taf','修改 [taf.tafnotify_172.19.103.14] 模板','2013-12-26 18:54:13',NULL),(88,'DevTest','taf','停止 [taf.tafpatch_172.19.103.14]','2013-12-26 20:32:30',NULL),(89,'DevTest','taf','启动 [taf.tafpatch_172.19.103.14]','2013-12-26 20:32:30',NULL),(90,'DevTest','taf','停止 [taf.tafnotify_172.19.103.14]','2013-12-26 20:32:43',NULL),(91,'DevTest','taf','启动 [taf.tafnotify_172.19.103.14]','2013-12-26 20:32:44',NULL),(92,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 20:33:01',NULL),(93,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 20:33:01',NULL),(94,'DevTest','taf','停止 [taf.taflog_172.19.103.14]','2013-12-26 20:33:09',NULL),(95,'DevTest','taf','启动 [taf.taflog_172.19.103.14]','2013-12-26 20:33:09',NULL),(96,'DevTest','taf','停止 [taf.tafconfig_172.19.103.14]','2013-12-26 20:33:19',NULL),(97,'DevTest','taf','启动 [taf.tafconfig_172.19.103.14]','2013-12-26 20:33:19',NULL),(98,'DevTest','taf','停止 [taf.tafpatch_172.19.103.14]','2013-12-26 20:33:30',NULL),(99,'DevTest','taf','启动 [taf.tafpatch_172.19.103.14]','2013-12-26 20:33:30',NULL),(100,'DevTest','taf','修改 [taf.tafpatch_172.19.103.14] 模板','2013-12-26 20:35:40',NULL),(101,'DevTest','taf','停止 [taf.tafpatch_172.19.103.14]','2013-12-26 20:35:48',NULL),(102,'DevTest','taf','启动 [taf.tafpatch_172.19.103.14]','2013-12-26 20:35:48',NULL),(103,'DevTest','taf','修改 [taf.tafstat.taf.tafstat.StatQueryObjAdapter] Adapter','2013-12-26 20:41:36',NULL),(104,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 20:41:45',NULL),(105,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 20:41:45',NULL),(106,'DevTest','taf','向 [taf.tafstat_172.19.103.14] 发送 [taf.setloglevel INFO] 命令','2013-12-26 20:45:49',NULL),(107,'DevTest','taf','向 [taf.tafstat_172.19.103.14] 发送 [taf.setloglevel DEBUG] 命令','2013-12-26 20:59:44',NULL),(108,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 21:03:18',NULL),(109,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 21:03:19',NULL),(110,'DevTest','taf','修改 [taf.stat] 模板','2013-12-26 21:14:25',NULL),(111,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 21:14:33',NULL),(112,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 21:14:33',NULL),(113,'DevTest','taf','修改 [taf.stat] 模板','2013-12-26 21:18:02',NULL),(114,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 21:18:16',NULL),(115,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 21:18:16',NULL),(116,'DevTest','taf','修改 [taf.stat] 模板','2013-12-26 22:10:07',NULL),(117,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 22:10:13',NULL),(118,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 22:10:15',NULL),(119,'DevTest','MLOL','修改 [MLOL.TestServer_172.19.103.14] 模板','2013-12-26 22:12:53',NULL),(120,'DevTest','MLOL','修改 [MLOL.TestServer.MLOL.TestServer.TestServantObjAdapter] Adapter','2013-12-26 22:13:09',NULL),(121,'DevTest','MLOL','上传 [MLOL.TestServer], 文件名: [MLOL.TestServer_1388067288440.tgz]','2013-12-26 22:14:48',NULL),(122,'DevTest','taf','修改 [taf.stat] 模板','2013-12-26 22:16:33',NULL),(123,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-26 22:16:38',NULL),(124,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-26 22:16:39',NULL),(125,'DevTest','MLOL','发布 [MLOL.TestServer] 版本：1','2013-12-26 22:23:47',NULL),(126,'DevTest','MLOL','停止 [MLOL.TestServer]','2013-12-26 22:23:48',NULL),(127,'DevTest','MLOL','启动 [MLOL.TestServer]','2013-12-26 22:23:48',NULL),(128,'DevTest','MLOL','上传 [MLOL.TestServer], 文件名: [MLOL.TestServer_1388068093386.tgz]','2013-12-26 22:28:13',NULL),(129,'DevTest','MLOL','发布 [MLOL.TestServer] 版本：2','2013-12-26 22:28:22',NULL),(130,'DevTest','MLOL','停止 [MLOL.TestServer]','2013-12-26 22:28:22',NULL),(131,'DevTest','MLOL','启动 [MLOL.TestServer]','2013-12-26 22:28:22',NULL),(132,'DevTest','MLOL','停止 [MLOL.TestServer_172.19.103.14]','2013-12-26 22:28:36',NULL),(133,'DevTest','MLOL','发布 [MLOL.TestServer] 版本：2','2013-12-26 22:28:43',NULL),(134,'DevTest','MLOL','停止 [MLOL.TestServer]','2013-12-26 22:28:43',NULL),(135,'DevTest','MLOL','启动 [MLOL.TestServer]','2013-12-26 22:28:43',NULL),(136,'DevTest','taf','修改 [taf.stat] 模板','2013-12-27 10:30:59',NULL),(137,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-27 10:31:55',NULL),(138,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-27 10:31:55',NULL),(139,'DevTest','taf','修改 [taf.stat] 模板','2013-12-27 12:41:41',NULL),(140,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-27 12:41:46',NULL),(141,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-27 12:41:46',NULL),(142,'DevTest','taf','停止 [taf.tafstat_172.19.103.14]','2013-12-27 13:22:40',NULL),(143,'DevTest','taf','启动 [taf.tafstat_172.19.103.14]','2013-12-27 13:22:40',NULL),(144,'DevTest','MLOL','停止 [MLOL.TestServer_172.19.103.14]','2013-12-27 21:06:41',NULL),(145,'DevTest','MLOL','启动 [MLOL.TestServer_172.19.103.14]','2013-12-27 21:06:44',NULL);
/*!40000 ALTER TABLE `t_user_logs` ENABLE KEYS */;
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
