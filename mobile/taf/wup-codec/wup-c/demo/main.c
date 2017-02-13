#include <sys/time.h>
#include <time.h>
#include "unistd.h"
#include "wup_c.h"

#include "Test.h"

void testJString()
{
	JString* src  = JString_new();
	JString* dst  = JString_new();
	printf("-----------------testJString------------------------------\n");
	JString_assign(src,"abcd",sizeof("abcd"));
	printf("after assign \"abcd\":%s\n",JString_data(src));

	JString_insert(src, 2,'i');
	printf("after insert \"i\": %s\n",JString_data(src));

	JString_append(src,"ef", sizeof("ef"));
	printf("after append \"ef\": %s\n",JString_data(src));

	JString_resize(dst, JString_size(src));
	JString_copyChar(dst, JString_data(src), JString_size(src));
	printf("copy src:%s  to dst: %s\n",JString_data(src),JString_data(dst));

	JString_clear(src);
	printf("after clear, JString size=%d\n",JString_size(src));
	JString_del(&src);
	JString_del(&dst);
}

void testJArray()
{
	JArray * arr = JArray_new("testarray");

	printf("-----------------testJArray------------------------------\n");
	JArray_pushBackString(arr,"JArray_pushBackString1111");
	printf("JArray_pushBackString: %s ,len=%d\n",JArray_getPtr(arr, 0),JArray_getLength(arr,0));

	JArray_pushBackString(arr,"JArray_pushBackString22");
	printf("JArray_pushBackString: %s,len=%d\n",JArray_getPtr(arr, 1),JArray_getLength(arr,1));

	JArray_del(&arr);
}

void  testStruct()
{
    char *buff=NULL;
    uint32_t len=0;

	printf("-----------------testStruct------------------------------\n");
    //encode
    {
		JceOutputStream* os = JceOutputStream_new();

        UniAttribute * attr = UniAttribute_new();
        Test_TestInfo *st = Test_TestInfo_new();

		//基本类型的赋值
        st->ibegin = 1111;
        st->ii = 54321;
        st->iend = 9999;
		st->f = 0.999f;
		st->d = 123.66f;
		st->uii = 234234;

        //string 类型的成员赋值
        JString_assign(st->s, "teststring", sizeof("teststring"));

		//vector<byte> 类型的成员赋值
		JString_assign(st->vb, "testvectrbyte", sizeof("testvectrbyte"));

		//vector<int> 类型的成员赋值,请使用流模式赋值
		int i = 0;
		for(i=0;i<10;i++)
		{
			JceOutputStream_reset(os);
			JceOutputStream_writeInt32(os, 1000+i, 0);
			JArray_pushBack(st->vi,JceOutputStream_getBuffer(os),JceOutputStream_getLength(os));
		}


        st->aa->a= 200;
        st->aa->b->a= 300;
        st->aa->b->f= 0.300;


        WUP_putStruct(attr,"mystruct",st);
        UniAttribute_encode(attr, &buff, &len);

		//回收分配的内存
		JceOutputStream_del(&os);
        Test_TestInfo_del(&st);
        UniAttribute_del(&attr);
    }

    //decode
    {
        Test_TestInfo *st = Test_TestInfo_new();
        UniAttribute * attr_out;
        attr_out = UniAttribute_new();
        UniAttribute_decode(attr_out, buff, len);
        JceFree(buff);
        buff=NULL;

        int ret = WUP_getStruct(attr_out, "mystruct", st);

        printf("\nTest_TestInfo get struct: ret = %d, st->className=%s, st->ibegin=%d, st->ii=%d, st->iend=%d, st->d=%f,st->uii=%d\n",
            ret, st->className,st->ibegin, st->ii, st->iend, st->d,st->uii);

        printf("Test_TestInfo get struct: ret = %d, st->aa->className=%s,st->aa->a=%d\n",
            ret, st->aa->className,st->aa->a);

        printf("Test_TestInfo get struct: ret = %d, st->aa->b->className=%s,st->aa->b->a=%d,st->aa->b->f=%f\n",
            ret,st->aa->b->className,st->aa->b->a,st->aa->b->f);
        printf("\n");


		//vector<int> 类型的读取,请使用流模式赋值
		JceInputStream* is = JceInputStream_new();

		int i =0;
		for(i = 0;i < JArray_size(st->vi);i++)
		{
			JceInputStream_reset(is);
			JceInputStream_setBuffer(is, JArray_getPtr(st->vi, i), JArray_getLength(st->vi, i));
			int n = 0;
			JceInputStream_readInt32(is, &n, 0, true);
			printf("st-vi[%d]: %d\n",i,n);
		}

		JceInputStream_del(&is);
        Test_TestInfo_del(&st);
        UniAttribute_del(&attr_out);
    }
}

void testwuppack()
{
    char *buff=NULL;
    uint32_t len=0;
	printf("-----------------testwuppack------------------------------\n");
    //encode
    {
        Test_TestInfo *st = Test_TestInfo_new();

        st->aa->a= 200;
        st->aa->b->a= 300;
        st->aa->b->f= 0.300;
        st->ibegin = 1111;
        st->ii = 54321;
        st->iend = 9999;
		st->d = 23.0023f;
		st->f = 0.22f;

		//vector<string>
		{

			JString* str = JString_new();
			char *pStr = "i am a JString";
			JString_assign(str,pStr,strlen(pStr));

			JceOutputStream* os = JceOutputStream_new();
			JceOutputStream_reset(os);
			JceOutputStream_writeString(os, str, 0);

			printf("urllist = %s :%d\n",JString_data(str),JString_size(str));
			JArray_pushBack(st->vStr, JceOutputStream_getBuffer(os),JceOutputStream_getLength(os));

			JString_del(&str);
			JceOutputStream_del(&os);
		}

		UniPacket* pack = UniPacket_new();

		pack->iVersion = 3;

		pack->cPacketType = 1;
		pack->iRequestId = 23;

		JString_assign(pack->sServantName,"myservant",sizeof("myservant"));
		JString_assign(pack->sFuncName,"myfun",sizeof("myfun"));

        WUP_putStruct(pack,"mystruct",st);
        UniPacket_encode(pack, &buff, &len);

		//回收分配的内存
        Test_TestInfo_del(&st);
        UniPacket_del(&pack);
    }

    //decode
    {
        UniPacket *unpack = UniPacket_new();

        UniPacket_decode(unpack, buff, len);
        JceFree(buff);
        buff=NULL;

		Test_TestInfo *st = Test_TestInfo_new();
        int ret = WUP_getStruct(unpack, "mystruct", st);

		printf("\nversion:%d",unpack->iVersion);
		printf("\nservantname:%s",JString_data(unpack->sServantName));
		printf("\nfunname:%s",JString_data(unpack->sFuncName));

        printf("\nTest_TestInfo get struct: ret = %d, st->className=%s, st->ibegin=%d, st->ii=%d, st->iend=%d, st->d=%f\n",
            ret, st->className,st->ibegin, st->ii, st->iend, st->d);

        printf("Test_TestInfo get struct: ret = %d, st->aa->className=%s,st->aa->a=%d\n",
            ret, st->aa->className,st->aa->a);

        printf("Test_TestInfo get struct: ret = %d, st->aa->b->className=%s,st->aa->b->a=%d,st->aa->b->f=%f\n",
            ret,st->aa->b->className,st->aa->b->a,st->aa->b->f);
        printf("\n");

	 	int i;
		JceInputStream* is = JceInputStream_new();
		JString* s = 	JString_new();

	 	for(i=0;i<JArray_size(st->vStr);i++)
	 	{
	 		JceInputStream_reset(is);
	 		JceInputStream_setBuffer(is, JArray_getPtr(st->vStr, i), JArray_getLength(st->vStr, i));

	 		JceInputStream_readString(is,s,0,false);
	 		printf("st->vStr[%d]:%s\n",i,JString_data(s));
	 	}

		JString_del(&s);
		JceInputStream_del(&is);
        Test_TestInfo_del(&st);
        UniPacket_del(&unpack);
    }
}


void  testVector()
{
    int i = 0;
    char *buff=NULL;
    uint32_t len=0;
	printf("-----------------testVector------------------------------\n");
    //encode
    {
         UniAttribute * attrv = UniAttribute_new();
         Test_A          *sta = Test_A_new();
         JArray          *arr = JArray_new(sta->className);
         JceOutputStream *vos = JceOutputStream_new();


         for(i=0; i< 10; i++)
         {
            sta->a = i;
            sta->b->a = i;
            sta->b->f = i*0.1;

            JceOutputStream_writeStruct(vos,sta,0);
            JArray_pushBack(arr, JceOutputStream_getBuffer(vos), JceOutputStream_getLength(vos));

            JceOutputStream_reset(vos);
         }

         printf("\n");
		 //结构体的array直接用put进行编码
         WUP_putVector(attrv, "myvector", arr);

         UniAttribute_encode(attrv, &buff, &len);
         UniAttribute_del(&attrv);
         Test_A_del(&sta);
         JceOutputStream_del(&vos);
         JArray_del(&arr);

    }

    //decode
    {
        Test_A *sta = Test_A_new();
        UniAttribute * attrv_out = UniAttribute_new();;
        JArray  *arrRes = JArray_new(sta->className);
        JceInputStream *ais = JceInputStream_new();

        UniAttribute_decode(attrv_out, buff, len);
        JceFree(buff);
        buff=NULL;
        int ret = WUP_getVector(attrv_out, "myvector", arrRes);
        printf("ret:%d, vector size:%d\n", ret, JArray_size(arrRes));

        for(i=0;i<JArray_size(arrRes);i++)
        {
            JceInputStream_setBuffer(ais, JArray_getPtr(arrRes, i), JArray_getLength(arrRes, i));
            JceInputStream_readStruct(ais,sta,0,true);
            printf("vector index:%d value:%d,%d,%f\n", i, sta->a, sta->b->a, sta->b->f);
        }
        printf("\n");

        JArray_del(&arrRes);
        UniAttribute_del(&attrv_out);
        Test_A_del(&sta);
        JceOutputStream_del(&ais);
    }
}


void  testMap()
{
    int i = 0;
    char *buff=NULL;
    uint32_t len=0;
	printf("-----------------testMap------------------------------\n");
    //encode
    {
        char c[10] = {0};
        UniAttribute *      attr = UniAttribute_new();;
        JMapWrapper *          m = JMapWrapper_new("int32", "string");
        JceOutputStream *first   = JceOutputStream_new();
        JceOutputStream *second  = JceOutputStream_new();
        JceInputStream  *ais     = JceInputStream_new();

		//map<int,string>类型的赋值,请使用流模式赋值
        for(i=0; i<10;i++)
        {
            JceOutputStream_reset(first);
            JceOutputStream_writeInt32(first, i, 0);
            sprintf(c,"testmap%d",i);

            JceOutputStream_reset(second);
            JceOutputStream_writeStringBuffer(second, c, strlen(c), 2);
            JMapWrapper_put(m, JceOutputStream_getBuffer(first), JceOutputStream_getLength(first), JceOutputStream_getBuffer(second), JceOutputStream_getLength(second));
        }

		//map类型的wup编码
        int ret = WUP_putMap(attr, "mymap", m);
		printf("ret:%d, map size:%d\n", ret, JMapWrapper_size(m));
		//在内部分配内存，需要由用户自己释放
        UniAttribute_encode(attr, &buff, &len);

        JceInputStream_del(&ais);
        JceOutputStream_del(&first);
        JceOutputStream_del(&second);
        JMapWrapper_del(&m);
        UniAttribute_del(&attr);
    }



    //decode
    {
        JString *          s = JString_new();
        UniAttribute *  attr = UniAttribute_new();;
        JMapWrapper *   mRes = JMapWrapper_new("int32", "string");
        JceInputStream * ais = JceInputStream_new();

        UniAttribute_decode(attr, buff, len);
		//map类型的wup解码
        int ret = WUP_getMap(attr, "mymap", mRes);
        printf("ret:%d, map size:%d\n", ret, JMapWrapper_size(mRes));
		//读map<int, string> 类型的成员
        for(i=0; i<JMapWrapper_size(mRes); ++i)
        {
            Int32 ai;
            JceInputStream_reset(ais);
            JceInputStream_setBuffer(ais, JArray_getPtr(mRes->first, i), JArray_getLength(mRes->first, i));
            JceInputStream_readInt32(ais, &ai, 0, true);

            JceInputStream_setBuffer(ais, JArray_getPtr(mRes->second, i), JArray_getLength(mRes->second, i));
            JceInputStream_readString(ais, s, 2, true);
            printf("map index:%d, first:%d, second:%s\n", i, ai, JString_data(s));
        }
        printf("\n");

        JString_del(&s);
        JceInputStream_del(&ais);
        JMapWrapper_del(&mRes);
        UniAttribute_del(&attr);

        JceFree(buff);
        buff=NULL;
    }

}

void  testMapWithStruct()
{
    int i = 0;
    char *buff=NULL;
    uint32_t len=0;
	printf("-----------------testMapWithStruct------------------------------\n");
    //encode
    {
        Test_A *sta = Test_A_new();
        UniAttribute * attr = UniAttribute_new();;
        JMapWrapper *m = JMapWrapper_new("int32", sta->className);
        JceOutputStream *first = JceOutputStream_new();
        JceOutputStream *second = JceOutputStream_new();

		//请使用流模式赋值
        for(i=0; i<3;i++)
        {
            JceOutputStream_reset(first);
            JceOutputStream_writeInt32(first, i, 0);
            sta->a = i;
            sta->b->a = i;
            sta->b->f = i*0.1;
            JceOutputStream_reset(second);
            JceOutputStream_writeStruct(second,sta,1);
            JMapWrapper_put(m, JceOutputStream_getBuffer(first), JceOutputStream_getLength(first), JceOutputStream_getBuffer(second), JceOutputStream_getLength(second));
        }

        WUP_putMap(attr, "mymap", m);
        UniAttribute_encode(attr, &buff, &len);

        JceOutputStream_del(&first);
        JceOutputStream_del(&second);
        JMapWrapper_del(&m);
        UniAttribute_del(&attr);
        Test_A_del(&sta);
    }
    //decode
    {
        Test_A *sta = Test_A_new();
        UniAttribute * attr = UniAttribute_new();;
        JMapWrapper *mRes = JMapWrapper_new("int32", sta->className);
        JceInputStream  *ais = JceInputStream_new();
        UniAttribute_decode(attr, buff, len);
        int ret = WUP_getMap(attr, "mymap", mRes);
        printf("ret:%d, map size:%d\n", ret, JMapWrapper_size(mRes));
        for(i=0; i<JMapWrapper_size(mRes); ++i)
        {
            Int32 ai;
            JceInputStream_setBuffer(ais, JArray_getPtr(mRes->first, i), JArray_getLength(mRes->first, i));
            JceInputStream_readInt32(ais, &ai, 0, true);

            JceInputStream_setBuffer(ais, JArray_getPtr(mRes->second, i), JArray_getLength(mRes->second, i));
            JceInputStream_readStruct(ais,sta,1,true);
            printf("map index:%d, first:%d, second:%s,%d,%s,%d,%f\n", i, ai,sta->className ,sta->a,sta->b->className,sta->b->a,sta->b->f);
        }
        printf("\n");

        Test_A_del(&sta);
        JceInputStream_del(&ais);
        JMapWrapper_del(&mRes);
        UniAttribute_del(&attr);
        JceFree(buff);
        buff=NULL;
    }

}



int main(int argc, char *argv[])
{
   // while(1)
    {
        usleep(2);
		testJString();
		testJArray();

		testStruct();
		testwuppack();

		testVector();
		testMap();

		testMapWithStruct();
    }

    return 0;
}
