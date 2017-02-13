#if 0








	try
	{
		CommunicatorPtr pcomm = new Communicator();
		pcomm->setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 172.25.38.67 -p 17890");
		printf("%s|%s|%d\n", __FILE__, __FUNCTION__, __LINE__);
	
		TafRollLogger::getInstance()->setLogInfo("Test", "KevinTestServer", "./log", 100000, 10, pcomm, "taf.taflog.LogObj");
		printf("%s|%s|%d\n", __FILE__, __FUNCTION__, __LINE__);
	
		TafRollLogger::getInstance()->sync(false);
		printf("%s|%s|%d\n", __FILE__, __FUNCTION__, __LINE__);
	
		TafTimeLogger::getInstance()->setLogInfo(pcomm, "taf.taflog.LogObj", "Test", "KevinTestServer", "./log");
		
		
		TafDyeingSwitch dye;
		dye.enableDyeing();
		printf("%s|%s|%d\n", __FILE__, __FUNCTION__, __LINE__);
	
		LOG->debug() << __FILE__ << "|" << __LINE__ << "|" << endl; 
		printf("%s|%s|%d\n", __FILE__, __FUNCTION__, __LINE__);
		
		FDLOG("xx") << __FILE__ << "|" << __LINE__ << "|" << endl; 
		
		TafRollLogger::getInstance()->logger()->flush();
		
	}
	catch (std::exception & ex)
	{
		printf("%s|%s|%d|%s\n", __FILE__, __FUNCTION__, __LINE__, ex.what());
	}
	return 0;



struct shmHeader
{
    uint32_t        uid;            //连接标示
    char            ip[16];         //远程连接的ip
    uint16_t        port;           //远程连接的端口
    time_t          recvTimeStamp;  //接收到数据的时间
    bool            isOverload;     //是否已过载 
    bool            isClosed;       //是否已关闭 
};

int make(int inum)
{
	TC_Shm                  _shm;
	TC_MemQueue             _shmQueue;
	_shm.init(64000, 32001, false);

    if (_shm.iscreate())
    {
        _shmQueue.create(_shm.getPointer(), 64000);
    }
    else
    {
        _shmQueue.connect(_shm.getPointer(), 64000);
    }

	for (int i = 0; i < inum; i++)
	{
		try
		{
			Test::stUserInfo_t inUser, outUser;
				
			inUser.sUserID 		= "kevintian_" + TC_Common::tostr<int>(i);
			inUser.sUserName	= "kevin";
			inUser.sUserDesc	= "SZ.TENCENT";
			
			taf::JceOutputStream<taf::BufferWriter> _os;
			_os.write(inUser, 1);
			_os.write(outUser, 2);
	
			//////////////////
			RequestPacket request;
			request.iVersion		= taf::JCEVERSION;
			request.cPacketType		= taf::JCENORMAL;
			request.iMessageType	= 0;
			request.iRequestId		= 0;
			request.sServantName	= "Test.KevinTestServer.KevinTestServantObj";
			request.sFuncName		= "setUser";
			request.sBuffer.resize(_os.getLength());
			memcpy((char *)&request.sBuffer[0], _os.getBuffer(), _os.getLength());

			/////////////////
			
			JceOutputStream<BufferWriter> os;
			request.writeTo(os);
		
			/////////////////////////////
			struct shmHeader head;
			head.uid            = i;
			head.port           = 222;
			head.recvTimeStamp  = time(NULL);
			head.isOverload     = false;
			head.isClosed       = false;
			strncpy(head.ip, "172.25.38.68", sizeof(head.ip));
			
			//////////////////////////////
			string sBuffer;
			sBuffer.reserve(sizeof(struct shmHeader) + os.getLength());
			sBuffer.append((char *)&head, sizeof(struct shmHeader));
			sBuffer.append((char *)os.getBuffer(), os.getLength());

			if (_shmQueue.push_back((const char *)sBuffer.data(), sBuffer.size()) == false)
			{
				std::cout << "push_back error" << std::endl;
				return 1;
			}
		}
		catch (std::exception & ex)
		{
			std::cout << "error:" << ex.what() << std::endl;
			return -1;
		}
	}
	
	return 0;
}

int testhttprequest()
{
    try
	{
		Test::stUserInfo_t inUser;
				
		inUser.sUserID 		= "kevintian";
		inUser.sUserName	= "kevin";
		inUser.sUserDesc	= "SZ.TENCENT";
				
		//编码
		wup::TafUniPacket<> uni_req;
		uni_req.put("suin", 	string("kevin"));
		uni_req.put("inUser",	inUser);
		uni_req.setServantName("testserver");
		uni_req.setFuncName("setUser");
		uni_req.setRequestId(0);
				
		std::string sBuffer;
		uni_req.encode(sBuffer);
		
		
		
		//发送http请求
		TC_HttpRequest request;
		request.setPostRequest("http://172.25.38.67:30000", sBuffer);
		request.setHeader("KEVIN", "123456789");
		//request.setHeader("KEVIN", "987654321");
		
		TC_HttpResponse res;
        request.doRequest(res);
		
		//string strResponse = res.getContent() ;
	}
	catch (std::exception & ex)
	{
		std::cout << "exception:" << ex.what() << std::endl;
		return -1;
	}
	     
	return 0;
}
int testwuprequest()
{
	try
	{
		Communicator _comm;
		//获得proxy
		taf::ServantPrx proxy = _comm.stringToProxy<taf::ServantPrx>("Test.KevinTestServer.KevinTestServantObj@tcp -h 172.25.38.67 -p 28000");
		taf::ProxyProtocol pro;
		pro.requestFunc     = taf::ProxyProtocol::streamRequest;
		pro.responseFunc    = taf::ProxyProtocol::wupResponse;
		proxy->taf_set_protocol(pro);
		
		//
		uint32_t requestid 	= proxy->taf_gen_requestid();

		//组织个协议头
		std::map<std::string, std::string> http;
		http["Kevin"] = "123456";
		http["KEVIN"] = "654321";

		
		Test::stUserInfo_t inUser;
				
		inUser.sUserID 		= "kevintian";
		inUser.sUserName	= "kevin";
		inUser.sUserDesc	= "SZ.TENCENT";
				
		//编码
		wup::TafUniPacket<> uni_req;
		uni_req.put("suin", 	string("kevin"));
		uni_req.put("inUser",	inUser);
		uni_req.setServantName("Test.KevinTestServer.KevinTestServantObj");
		uni_req.setFuncName("setUser");
		uni_req.setRequestId(requestid);
		uni_req.setTafContext(http);
		
		std::string sBuffer;
		uni_req.encode(sBuffer);
		
		//调用
		ResponsePacket resp;
		proxy->rpc_call(requestid, "", sBuffer.data(), sBuffer.size(), resp);
		
		//获取返回
		wup::TafUniPacket<> uni_res;
		uni_res.decode((char *)&resp.sBuffer[0], resp.sBuffer.size());
		
			
		std::cout << "ret:#" << uni_res.get<string>(string("")) << "#" << std::endl;
		
		//const std::map<std::string, std::string> & status = uni_res.getTafStatus();
		
		//std::cout << "ret:#" << taf::TC_Common::tostr(status)  << "#" << std::endl;
	}
	catch (std::exception & ex)
	{
		std::cout << "error:" << ex.what() << std::endl;
		return -1;
	}

	return 0;
}
#endif
