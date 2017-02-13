-- MySQL dump 10.13  Distrib 5.5.31, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: db_taf
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
-- Table structure for table `t_adapter_conf`
--

DROP TABLE IF EXISTS `t_adapter_conf`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_adapter_conf` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `application` varchar(128) DEFAULT '',
  `server_name` varchar(128) DEFAULT '',
  `node_name` varchar(128) DEFAULT '',
  `adapter_name` varchar(64) DEFAULT '',
  `thread_num` int(11) DEFAULT '1',
  `endpoint` varchar(128) DEFAULT '',
  `max_connections` int(11) DEFAULT '1000',
  `allow_ip` varchar(255) NOT NULL DEFAULT '',
  `servant` varchar(128) DEFAULT '',
  `queuecap` int(11) DEFAULT NULL,
  `queuetimeout` int(11) DEFAULT NULL,
  `posttime` datetime DEFAULT '0000-00-00 00:00:00',
  `lastuser` varchar(30) DEFAULT NULL,
  `registry_timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `protocol` varchar(64) DEFAULT 'taf',
  `handlegroup` varchar(64) DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `application` (`application`,`server_name`,`node_name`,`adapter_name`)
) ENGINE=MyISAM AUTO_INCREMENT=9 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_adapter_conf`
--

LOCK TABLES `t_adapter_conf` WRITE;
/*!40000 ALTER TABLE `t_adapter_conf` DISABLE KEYS */;
INSERT INTO `t_adapter_conf` VALUES (1,'taf','tafconfig','172.19.103.14','taf.tafconfig.ConfigObjAdapter',5,'tcp -h 172.19.103.14 -t 60000 -p 12532',1024,'','taf.tafconfig.ConfigObj',10000,60000,'2013-12-26 18:52:59','DevTest','2013-12-26 10:52:59','taf',''),(2,'taf','taflog','172.19.103.14','taf.taflog.LogObjAdapter',5,'tcp -h 172.19.103.14 -t 60000 -p 12533',1024,'','taf.taflog.LogObj',10000,60000,'2013-12-26 18:53:10','DevTest','2013-12-26 10:53:10','taf',''),(3,'taf','tafstat','172.19.103.14','taf.tafstat.StatObjAdapter',5,'tcp -h 172.19.103.14 -t 60000 -p 12534',1024,'','taf.tafstat.StatObj',10000,60000,'2013-12-26 16:57:23','DevTest','2013-12-26 09:28:43','taf',''),(4,'taf','tafproperty','172.19.103.14','taf.tafproperty.PropertyObjAdapter',5,'tcp -h 172.19.103.14 -t 60000 -p 12535',1024,'','taf.tafproperty.PropertyObj',10000,60000,'2013-12-26 16:57:54','DevTest','2013-12-26 09:28:43','taf',''),(5,'taf','tafpatch','172.19.103.14','taf.tafpatch.PatchObjAdapter',5,'tcp -h 172.19.103.14 -t 60000 -p 13536',1024,'','taf.tafpatch.PatchObj',10000,60000,'2013-12-26 16:58:24','DevTest','2013-12-26 09:28:43','taf',''),(6,'taf','tafnotify','172.19.103.14','taf.tafnotify.NotifyObjAdapter',5,'tcp -h 172.19.103.14 -t 60000 -p  12536',1024,'','taf.tafnotify.NotifyObj',10000,60000,'2013-12-26 18:53:47','DevTest','2013-12-26 10:53:47','taf',''),(7,'taf','tafstat','172.19.103.14','taf.tafstat.StatQueryObjAdapter',5,'tcp -h 172.19.103.14 -t 60000 -p 12537',1024,'','taf.tafstat.StatQueryObj',10000,60000,'2013-12-26 20:41:36','DevTest','2013-12-26 12:41:36','taf',''),(8,'MLOL','TestServer','172.19.103.14','MLOL.TestServer.TestServantObjAdapter',5,'tcp -h 172.19.103.14 -t 60000 -p 12345',1024,'','MLOL.TestServer.TestServantObj',10000,60000,'2013-12-26 22:13:09','DevTest','2013-12-26 14:13:09','taf','');
/*!40000 ALTER TABLE `t_adapter_conf` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_ats_cases`
--

DROP TABLE IF EXISTS `t_ats_cases`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ats_cases` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `casename` varchar(20) DEFAULT NULL,
  `retvalue` text,
  `paramvalue` text,
  `interfaceid` int(11) DEFAULT NULL,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_ats_cases`
--

LOCK TABLES `t_ats_cases` WRITE;
/*!40000 ALTER TABLE `t_ats_cases` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_ats_cases` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_ats_interfaces`
--

DROP TABLE IF EXISTS `t_ats_interfaces`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ats_interfaces` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `objname` varchar(255) DEFAULT NULL,
  `funcname` varchar(255) DEFAULT NULL,
  `retype` text,
  `paramtype` text,
  `outparamtype` text,
  `interfaceid` int(11) DEFAULT NULL,
  `postime` datetime DEFAULT NULL,
  `lastuser` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_ats_interfaces`
--

LOCK TABLES `t_ats_interfaces` WRITE;
/*!40000 ALTER TABLE `t_ats_interfaces` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_ats_interfaces` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_config_files`
--

DROP TABLE IF EXISTS `t_config_files`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_config_files` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `server_name` varchar(128) DEFAULT '',
  `host` varchar(20) NOT NULL DEFAULT '',
  `filename` varchar(128) DEFAULT NULL,
  `config` longtext,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(50) DEFAULT NULL,
  `level` int(11) DEFAULT '2',
  PRIMARY KEY (`id`),
  UNIQUE KEY `application` (`server_name`,`filename`,`host`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_files`
--

LOCK TABLES `t_config_files` WRITE;
/*!40000 ALTER TABLE `t_config_files` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_config_files` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_config_history_files`
--

DROP TABLE IF EXISTS `t_config_history_files`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_config_history_files` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `configid` int(11) DEFAULT NULL,
  `reason` varchar(128) DEFAULT '',
  `reason_select` varchar(20) NOT NULL DEFAULT '',
  `content` longtext,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_config_history_files`
--

LOCK TABLES `t_config_history_files` WRITE;
/*!40000 ALTER TABLE `t_config_history_files` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_config_history_files` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_group_priority`
--

DROP TABLE IF EXISTS `t_group_priority`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_group_priority` (
  `id` varchar(20) NOT NULL,
  `group_list` varchar(50) NOT NULL DEFAULT '',
  `station` varchar(90) NOT NULL,
  `list_order` int(11) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_group_priority`
--

LOCK TABLES `t_group_priority` WRITE;
/*!40000 ALTER TABLE `t_group_priority` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_group_priority` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_node_info`
--

DROP TABLE IF EXISTS `t_node_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_node_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `node_name` varchar(128) DEFAULT '',
  `node_obj` varchar(128) DEFAULT '',
  `endpoint_ip` varchar(16) DEFAULT '',
  `endpoint_port` int(11) DEFAULT '0',
  `data_dir` varchar(128) DEFAULT '',
  `load_avg1` float DEFAULT '0',
  `load_avg5` float DEFAULT '0',
  `load_avg15` float DEFAULT '0',
  `last_reg_time` datetime DEFAULT '1970-01-01 00:08:00',
  `last_heartbeat` datetime DEFAULT '1970-01-01 00:08:00',
  `setting_state` enum('active','inactive') DEFAULT 'inactive',
  `present_state` enum('active','inactive') DEFAULT 'inactive',
  `modify_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `template_name` varchar(50) DEFAULT '',
  `taf_version` varchar(30) DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `node_name` (`node_name`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_node_info`
--

LOCK TABLES `t_node_info` WRITE;
/*!40000 ALTER TABLE `t_node_info` DISABLE KEYS */;
INSERT INTO `t_node_info` VALUES (1,'172.19.103.14','taf.tafnode.NodeObj@tcp -h 172.19.103.14 -p 19385 -t 60000','172.19.103.14',19385,'/usr/local/app/taf/tafnode/data',0.57,0.8,0.76,'2013-12-25 19:10:42','2013-12-27 21:49:53','active','active','2013-12-27 13:49:53','','2.1.4.3');
/*!40000 ALTER TABLE `t_node_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_profile_template`
--

DROP TABLE IF EXISTS `t_profile_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_profile_template` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `template_name` varchar(128) DEFAULT '',
  `parents_name` varchar(128) DEFAULT '',
  `profile` text NOT NULL,
  `posttime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `lastuser` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `template_name` (`template_name`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_profile_template`
--

LOCK TABLES `t_profile_template` WRITE;
/*!40000 ALTER TABLE `t_profile_template` DISABLE KEYS */;
INSERT INTO `t_profile_template` VALUES (1,'taf.default','taf.default','<taf>\r\n  <application>\r\n    \r\n    #proxy需要的配置\r\n    <client>\r\n        #地址 tcp -h 127.0.0.1 -p 30000\r\n        locator                     = ${locator}\r\n        #最大超时时间(毫秒)\r\n        max-invoke-timeout          = 3000\r\n        #刷新端口时间间隔(毫秒)\r\n        refresh-endpoint-interval   = 10000\r\n        #模块间调用\r\n        stat                        = taf.tafstat.StatObj\r\n        #属性上报服务\r\n        property                    = taf.tafproperty.PropertyObj\r\n        #上报间隔时间(秒)\r\n        report-interval             = 60\r\n        #网络发送线程个数\r\n        sendthread                  = 1\r\n        #网络接收线程个数\r\n        recvthread                  = 1\r\n        #网络异步回调线程个数\r\n        asyncthread                 = 3\r\n        #模块名称\r\n        modulename                  = ${modulename}\r\n    </client>\r\n    \r\n    #定义所有绑定的IP\r\n    <server>\r\n        #应用名称\r\n        app      = ${app}\r\n        #服务名称\r\n        server   = ${server}\r\n        #本地ip地址\r\n        localip  = ${localip}\r\n        #服务可执行文件,配置文件目录\r\n        basepath = ${basepath}\r\n        #服务的数据目录\r\n	datapath = ${datapath}\r\n        #日志路径\r\n        logpath  = ${logpath}\r\n        #日志大小\r\n        logsize  = 50M\r\n        #日志个数\r\n        lognum   = 10\r\n        #网络线程个数\r\n        netthread = 2\r\n        #本地管理套接字[可选]\r\n        local  = ${local}\r\n        #配置中心的地址[可选]\r\n        config  = taf.tafconfig.ConfigObj\r\n        #通知信息中心的地址[可选]\r\n        notify  = taf.tafnotify.NotifyObj\r\n        #远程LogServer[可选]\r\n        log     = taf.taflog.LogObj\r\n    </server>            \r\n  </application>\r\n</taf>','2013-12-25 22:36:03','DevTest'),(2,'taf.config','taf.default','<taf>\r\n     <db>\r\n                dbhost = 127.0.0.1\r\n                dbname = db_taf\r\n                dbuser = root\r\n                dbpass = 3e91eb88c2e115b0\r\n                dbport = 3306\r\n                charset = gbk\r\n     </db>\r\n</taf>','2013-12-25 22:38:20','DevTest'),(3,'taf.log','taf.default','<taf>\r\n    <log>\r\n        #log文件目录\r\n        logpath   = /usr/local/app/taf/app_log\r\n	#log写线程数\r\n        logthread = 20\r\n    </log>  \r\n</taf>','2013-12-25 22:38:51','DevTest'),(4,'taf.notify','taf.default','<taf>\r\n    <db>\r\n        dbhost=127.0.0.1\r\n        dbname=db_taf\r\n        dbuser = root\r\n        dbpass = 3e91eb88c2e115b0\r\n        dbport = 3306\r\n        charset = gbk\r\n    </db>\r\n    <hash>\r\n        min_block=50\r\n        max_block=200\r\n        factor=1.5\r\n        file_path=notify.hmap\r\n        file_size=50M\r\n        max_page_num=30\r\n        max_page_size=20\r\n    </hash>\r\n</taf>','2013-12-25 22:41:44','DevTest'),(5,'taf.stat','taf.default','<taf> \r\n      <multidb>\r\n          <db1>\r\n                dbhost = 127.0.0.1\r\n                dbname = db_taf_stat\r\n                tbname = t_stat_realtime_\r\n                dbuser = root\r\n                dbpass = 3e91eb88c2e115b0\r\n                dbport = 3306\r\n                charset = gbk\r\n          </db1>\r\n       </multidb>\r\n       \r\n       <reapSql>\r\n                  CutType = day\r\n       </reapSql>\r\n\r\n        <hashmap>\r\n                file = hashmap.hmap\r\n                clonefile= hashmap_clone.hmap\r\n                masterfile=hashmap_master.hmap\r\n                slavefile=hashmap_slave.hmap\r\n                size = 10M\r\n                insertInterval =  5\r\n        </hashmap>\r\n         \r\n        sql = CREATE TABLE `${TABLE}`(`f_date`  varchar(15) default NULL,`f_tflag`  varchar(15) default NULL,`master_name` varchar(128) default NULL,`slave_name` varchar(128) default NULL,`interface_name` varchar(128) default NULL,`master_ip` varchar(15) default NULL,`slave_ip` varchar(15) default NULL,`slave_port` int(10) default NULL,`set_area`  varchar(10) default NULL,`set_id`  varchar(10) default NULL,`set_name`  varchar(10) default NULL,`taf_version`  varchar(10) default NULL,`return_value` int(11) default NULL,`succ_count` int(10) unsigned default NULL,`timeout_count` int(10) unsigned default NULL,`exce_count` int(10) unsigned default NULL,`interv_count` varchar(128) default NULL,`total_time` int(10) unsigned default NULL,`ave_time` int(10) unsigned default NULL,`maxrsp_time` int(10) unsigned default NULL,`minrsp_time` int(10) unsigned default NULL,KEY `IDX_TIME` (`f_date`),KEY `IDX_TIME2` (`f_tflag`),KEY `IDC_MASTER` (`master_name`),KEY `IDX_INTERFACENAME` (`interface_name`),KEY `IDX_SLAVEIP` (`slave_ip`),KEY `IDX_SLAVE` (`slave_name`),KEY `IDX_RETVALUE` (`return_value`)) ENGINE=MyISAM DEFAULT CHARSET=gbk\r\n  </taf>','2013-12-27 12:41:41','DevTest'),(6,'taf.patch','taf.default','<taf>\r\n    #发布目录\r\n    directory = /usr/local/app/patchs\r\n    #每次同步文件大小\r\n    size      = 1M\r\n</taf>  ','2013-12-25 22:42:58','DevTest'),(7,'taf.property','taf.default','<taf>\r\n    <db>\r\n        dbhost=127.0.0.1\r\n        dbname=db_taf_stat\r\n        dbuser = root\r\n        dbpass = 3e91eb88c2e115b0\r\n        dbport = 3306\r\n        charset = gbk\r\n    </db>\r\n   <hashmap>\r\n        file=hashmap.hmap\r\n    </hashmap>\r\n    sql = CREATE TABLE `${TABLE}` (`stattime` datetime default NULL,`master_name` varchar(128) NOT NULL default \'\',`master_ip` varchar(16) default NULL,`property_name` varchar(100) default NULL,`policy` varchar(20) default NULL,`value` varchar(255) default NULL,KEY `IDX_MASTER_NAME` (`master_name`),KEY `IDX_MASTER_IP` (`master_ip`),KEY `IDX_TIME` (`stattime`)) ENGINE=MyISAM\r\n# DEFAULT CHARSET=gbk\r\n</taf>','2013-12-26 16:45:39','DevTest');
/*!40000 ALTER TABLE `t_profile_template` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_registry_info`
--

DROP TABLE IF EXISTS `t_registry_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_registry_info` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `locator_id` varchar(128) NOT NULL DEFAULT '',
  `servant` varchar(128) NOT NULL DEFAULT '',
  `endpoint` varchar(128) NOT NULL DEFAULT '',
  `last_heartbeat` datetime DEFAULT '1970-01-01 00:08:00',
  `present_state` enum('active','inactive') DEFAULT 'inactive',
  `modify_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `taf_version` varchar(50) DEFAULT '',
  `enable_group` char(1) DEFAULT 'N',
  PRIMARY KEY (`id`),
  UNIQUE KEY `locator_id` (`locator_id`,`servant`)
) ENGINE=MyISAM AUTO_INCREMENT=58546 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_registry_info`
--

LOCK TABLES `t_registry_info` WRITE;
/*!40000 ALTER TABLE `t_registry_info` DISABLE KEYS */;
INSERT INTO `t_registry_info` VALUES (58543,'172.19.103.14:17890','taf.tafregistry.AdminRegObj','tcp -h 172.19.103.14 -p 17892 -t 12000','2013-12-27 21:50:14','active','2013-12-27 13:50:14','2.1.4.3','N'),(58544,'172.19.103.14:17890','taf.tafregistry.QueryObj','tcp -h 172.19.103.14 -p 17890 -t 10000','2013-12-27 21:50:14','active','2013-12-27 13:50:14','2.1.4.3','N'),(58545,'172.19.103.14:17890','taf.tafregistry.RegistryObj','tcp -h 172.19.103.14 -p 17891 -t 30000','2013-12-27 21:50:14','active','2013-12-27 13:50:14','2.1.4.3','N');
/*!40000 ALTER TABLE `t_registry_info` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_conf`
--

DROP TABLE IF EXISTS `t_server_conf`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_conf` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `application` varchar(128) DEFAULT '',
  `server_name` varchar(128) DEFAULT '',
  `node_name` varchar(128) NOT NULL DEFAULT '',
  `base_path` varchar(128) DEFAULT '',
  `exe_path` varchar(128) NOT NULL DEFAULT '',
  `template_name` varchar(128) NOT NULL DEFAULT '',
  `setting_state` enum('active','inactive') NOT NULL DEFAULT 'inactive',
  `present_state` enum('active','inactive','activating','deactivating','destroyed') NOT NULL DEFAULT 'inactive',
  `process_id` int(11) NOT NULL DEFAULT '0',
  `patch_version` varchar(128) NOT NULL DEFAULT '',
  `patch_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `patch_user` varchar(128) NOT NULL DEFAULT '',
  `posttime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `lastuser` varchar(30) DEFAULT NULL,
  `node_group` varchar(50) DEFAULT '',
  `bak_flag` int(11) DEFAULT '0',
  `taf_version` varchar(50) DEFAULT '',
  `server_type` varchar(50) DEFAULT '',
  `registry_timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `enable_group` char(1) DEFAULT 'N',
  `enable_set` char(1) DEFAULT 'N',
  `set_name` varchar(50) DEFAULT '',
  `set_area` varchar(30) DEFAULT '',
  `set_group` varchar(30) DEFAULT '',
  `ip_group_name` varchar(30) DEFAULT '',
  `async_thread_num` int(10) DEFAULT '3',
  `profile` text,
  `shmkey` int(10) DEFAULT '0',
  `shmcap` int(10) DEFAULT '0',
  `start_script_path` varchar(64) DEFAULT '',
  `stop_script_path` varchar(64) DEFAULT '',
  `monitor_script_path` varchar(64) DEFAULT '',
  `config_center_port` int(10) DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `application` (`application`,`server_name`,`node_name`),
  UNIQUE KEY `application_2` (`application`,`server_name`,`node_name`),
  KEY `node_name` (`node_name`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_conf`
--

LOCK TABLES `t_server_conf` WRITE;
/*!40000 ALTER TABLE `t_server_conf` DISABLE KEYS */;
INSERT INTO `t_server_conf` VALUES (1,'taf','tafconfig','172.19.103.14','','/usr/local/taf/bin/tafconfig','taf.default','active','active',25925,'','0000-00-00 00:00:00','','2013-12-26 11:09:38','DevTest','',0,'2.1.4.3','taf','2013-12-26 12:33:19','N','N','','','','',3,NULL,0,0,NULL,NULL,'',0),(2,'taf','tafstat','172.19.103.14','','/usr/local/taf/bin/tafstat','taf.stat','active','active',23668,'','0000-00-00 00:00:00','','2013-12-26 16:34:47','DevTest','',0,'2.1.4.3','taf','2013-12-27 05:22:40','N','N','','','','',3,NULL,0,0,NULL,NULL,'',0),(3,'taf','tafproperty','172.19.103.14','','/usr/local/taf/bin/tafproperty','taf.property','active','active',8744,'','0000-00-00 00:00:00','','2013-12-26 16:39:07','DevTest','',0,'2.1.4.3','taf','2013-12-26 09:25:14','N','N','','','','',3,NULL,0,0,'','','',0),(4,'taf','taflog','172.19.103.14','','/usr/local/taf/bin/taflog','taf.log','active','active',25863,'','0000-00-00 00:00:00','','2013-12-26 16:47:18','DevTest','',0,'2.1.4.3','taf','2013-12-26 12:33:09','N','N','','','','',3,NULL,0,0,'','','',0),(5,'taf','tafpatch','172.19.103.14','','/usr/local/taf/bin/tafpatch','taf.patch','active','active',26515,'','0000-00-00 00:00:00','','2013-12-26 20:35:40','DevTest','',0,'2.1.4.3','taf','2013-12-26 12:35:48','N','N','','','','',3,NULL,0,0,NULL,NULL,'',0),(6,'taf','tafnotify','172.19.103.14','','/usr/local/taf/bin/tafnotify','taf.notify','active','active',25735,'','0000-00-00 00:00:00','','2013-12-26 18:54:13','DevTest','',0,'2.1.4.3','taf','2013-12-26 12:32:44','N','N','','','','',3,NULL,0,0,NULL,NULL,'',0),(7,'MLOL','TestServer','172.19.103.14','','','taf.default','active','active',23817,'2','2013-12-26 22:28:43','DevTest','2013-12-26 22:12:53','DevTest','',0,'2.1.4.3','taf','2013-12-27 13:06:44','N','N','','','','',3,NULL,0,0,'','','',0);
/*!40000 ALTER TABLE `t_server_conf` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_group_rule`
--

DROP TABLE IF EXISTS `t_server_group_rule`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_group_rule` (
  `group_id` int(10) NOT NULL DEFAULT '0',
  `group_name` varchar(20) NOT NULL,
  `allow_ip_rule` varchar(50) NOT NULL DEFAULT '',
  `denny_ip_rule` varchar(90) NOT NULL,
  `ip_order` int(11) DEFAULT '0',
  PRIMARY KEY (`group_id`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_group_rule`
--

LOCK TABLES `t_server_group_rule` WRITE;
/*!40000 ALTER TABLE `t_server_group_rule` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_server_group_rule` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_notifys`
--

DROP TABLE IF EXISTS `t_server_notifys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_notifys` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `server_name` varchar(30) DEFAULT NULL,
  `server_id` varchar(50) DEFAULT NULL,
  `thread_id` varchar(20) DEFAULT NULL,
  `command` varchar(50) DEFAULT NULL,
  `result` text,
  `notifytime` datetime DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=35 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_notifys`
--

LOCK TABLES `t_server_notifys` WRITE;
/*!40000 ALTER TABLE `t_server_notifys` DISABLE KEYS */;
INSERT INTO `t_server_notifys` VALUES (1,'taf.tafnotify','taf.tafnotify_172.19.103.14','140198924236608',NULL,'restart','2013-12-26 17:51:16'),(2,'taf.tafnotify','taf.tafnotify_172.19.103.14','140112509568832',NULL,'restart','2013-12-26 20:32:44'),(3,'taf.tafstat','taf.tafstat_172.19.103.14','140720157939520',NULL,'stop','2013-12-26 20:33:01'),(4,'taf.tafstat','taf.tafstat_172.19.103.14','140058974586688',NULL,'restart','2013-12-26 20:33:01'),(5,'taf.taflog','taf.taflog_172.19.103.14','140716776400704',NULL,'stop','2013-12-26 20:33:09'),(6,'taf.taflog','taf.taflog_172.19.103.14','139710427080512',NULL,'restart','2013-12-26 20:33:09'),(7,'taf.tafconfig','taf.tafconfig_172.19.103.14','140165906675520',NULL,'stop','2013-12-26 20:33:19'),(8,'taf.tafconfig','taf.tafconfig_172.19.103.14','140453627791168',NULL,'restart','2013-12-26 20:33:19'),(9,'taf.tafpatch','taf.tafpatch_172.19.103.14','139953571268416',NULL,'restart','2013-12-26 20:35:48'),(10,'taf.tafstat','taf.tafstat_172.19.103.14','140058974586688',NULL,'stop','2013-12-26 20:41:45'),(11,'taf.tafstat','taf.tafstat_172.19.103.14','140224773195584',NULL,'restart','2013-12-26 20:41:46'),(12,'taf.tafstat','taf.tafstat_172.19.103.14','140224587314944',NULL,'AdminServant::notify:taf.setloglevel INFO','2013-12-26 20:45:49'),(13,'taf.tafstat','taf.tafstat_172.19.103.14','140224587314944',NULL,'AdminServant::notify:taf.setloglevel DEBUG','2013-12-26 20:59:44'),(14,'taf.tafstat','taf.tafstat_172.19.103.14','140224773195584',NULL,'stop','2013-12-26 21:03:18'),(15,'taf.tafstat','taf.tafstat_172.19.103.14','140003255306048',NULL,'restart','2013-12-26 21:03:19'),(16,'taf.tafstat','taf.tafstat_172.19.103.14','140003255306048',NULL,'stop','2013-12-26 21:14:33'),(17,'taf.tafstat','taf.tafstat_172.19.103.14','140257915418432',NULL,'restart','2013-12-26 21:14:33'),(18,'taf.tafstat','taf.tafstat_172.19.103.14','140257915418432',NULL,'stop','2013-12-26 21:18:16'),(19,'taf.tafstat','taf.tafstat_172.19.103.14','139914297915200',NULL,'restart','2013-12-26 21:18:17'),(20,'taf.tafstat','taf.tafstat_172.19.103.14','139919963309888',NULL,'restart','2013-12-26 21:18:34'),(21,'taf.tafstat','taf.tafstat_172.19.103.14','140605806229312',NULL,'restart','2013-12-26 21:43:26'),(22,'taf.tafstat','taf.tafstat_172.19.103.14','140605806229312',NULL,'stop','2013-12-26 22:10:13'),(23,'taf.tafstat','taf.tafstat_172.19.103.14','140319270192960',NULL,'restart','2013-12-26 22:10:15'),(24,'taf.tafstat','taf.tafstat_172.19.103.14','140319270192960',NULL,'stop','2013-12-26 22:16:38'),(25,'taf.tafstat','taf.tafstat_172.19.103.14','140313147352896',NULL,'restart','2013-12-26 22:16:39'),(26,'MLOL.TestServer','MLOL.TestServer_172.19.103.14','139897936533312',NULL,'restart','2013-12-26 22:28:44'),(27,'taf.tafstat','taf.tafstat_172.19.103.14','140313147352896',NULL,'stop','2013-12-27 10:31:55'),(28,'taf.tafstat','taf.tafstat_172.19.103.14','139812565182272',NULL,'restart','2013-12-27 10:31:55'),(29,'taf.tafstat','taf.tafstat_172.19.103.14','139812565182272',NULL,'stop','2013-12-27 12:41:46'),(30,'taf.tafstat','taf.tafstat_172.19.103.14','139998799095616',NULL,'restart','2013-12-27 12:41:46'),(31,'taf.tafstat','taf.tafstat_172.19.103.14','139998799095616',NULL,'stop','2013-12-27 13:22:40'),(32,'taf.tafstat','taf.tafstat_172.19.103.14','140344188602176',NULL,'restart','2013-12-27 13:22:40'),(33,'MLOL.TestServer','MLOL.TestServer_172.19.103.14','139897936533312',NULL,'stop','2013-12-27 21:06:43'),(34,'MLOL.TestServer','MLOL.TestServer_172.19.103.14','140093628835648',NULL,'restart','2013-12-27 21:06:44');
/*!40000 ALTER TABLE `t_server_notifys` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_server_patchs`
--

DROP TABLE IF EXISTS `t_server_patchs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_server_patchs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `server` varchar(50) DEFAULT NULL,
  `version` varchar(255) DEFAULT NULL,
  `tgz` text,
  `update_text` varchar(255) DEFAULT NULL,
  `reason_select` varchar(255) DEFAULT NULL,
  `document_complate` varchar(30) DEFAULT NULL,
  `publish` tinyint(3) DEFAULT NULL,
  `publish_time` datetime DEFAULT NULL,
  `upload_time` datetime DEFAULT NULL,
  `upload_user` varchar(30) DEFAULT NULL,
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(30) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_server_patchs`
--

LOCK TABLES `t_server_patchs` WRITE;
/*!40000 ALTER TABLE `t_server_patchs` DISABLE KEYS */;
INSERT INTO `t_server_patchs` VALUES (1,'MLOL.TestServer',NULL,'MLOL.TestServer_1388067288440.tgz',' ','TMP_CHG','0',0,'2013-12-26 22:23:48','2013-12-26 22:14:48','DevTest','2013-12-26 22:23:48','DevTest'),(2,'MLOL.TestServer',NULL,'MLOL.TestServer_1388068093386.tgz',' ','TMP_CHG','0',1,'2013-12-26 22:28:43','2013-12-26 22:28:13','DevTest','2013-12-26 22:28:43','DevTest');
/*!40000 ALTER TABLE `t_server_patchs` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_web_release_conf`
--

DROP TABLE IF EXISTS `t_web_release_conf`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_web_release_conf` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `server` varchar(100) NOT NULL DEFAULT '',
  `path` varchar(200) NOT NULL DEFAULT '',
  `server_dir` varchar(200) NOT NULL DEFAULT '',
  `user` varchar(200) NOT NULL DEFAULT '*',
  `posttime` datetime DEFAULT NULL,
  `lastuser` varchar(60) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `IDX_SERVER` (`server`)
) ENGINE=MyISAM DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_web_release_conf`
--

LOCK TABLES `t_web_release_conf` WRITE;
/*!40000 ALTER TABLE `t_web_release_conf` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_web_release_conf` ENABLE KEYS */;
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
