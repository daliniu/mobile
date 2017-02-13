//
//  MttReleaseInfo+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-21.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttReleaseInfo+Adapter.h"
#import "NSPathEx.h"

/**
 * 浏览器的QUA相关宏定义(构建平台自动替换)，iPad版本QUA定义格式如下:
 * "IPQB23_GA/230004&IMTT_3/230004&IP&644820&AppleiPad&50060&6312&V3"
 */
#define QUA3_PART1      "IPQB25_GA/25"          // 版本号，如“IPQB23_GA/23”
#define QUA3_BUILD      "VersionName"           // build号
#define QUA3_PART2      "&IMTT_3/25"            // 版本号，如“&IMTT_3/23“
#define QUA3_PART3      "IP&644820&AppleiPad&"  // 编译号
#define QUA3_POSTFIX    "&V3"

 
/**
 * 浏览器的LC与LCID定义
 */

/**********************************************************
 *iPad 2.4
 **********************************************************
#define USER_LICENSE    "D63204D5BED24B7"       // iPad 2.4
#define USER_LICENSE_ID "6312"                  // iPad 2.4
*/

/**********************************************************
 *iPad 2.5
 **********************************************************/
#define USER_LICENSE    "069F9AA7C78F4AE"       // iPad 2.5
#define USER_LICENSE_ID "6355"                  // iPad 2.5
 
 
/**
 * 渠道号
 */
#define CHANNEL         "50060"

@implementation MttReleaseInfo

+ (NSString *)qua 
{
    static NSString *qUA = nil;
    if (qUA == nil) {
        NSString *format = @"" QUA3_PART1 QUA3_BUILD QUA3_PART2 QUA3_BUILD QUA3_PART3 "%@&" USER_LICENSE_ID QUA3_POSTFIX;
        qUA = [[NSString alloc] initWithFormat:format, [self channel]];
    }
    return qUA;
}

+ (NSString *)lc
{
    return @"" USER_LICENSE;
}

+ (NSString *)lcId
{
    return @"" USER_LICENSE_ID;
}

+ (NSString *)channel
{
    static NSString *channelID = nil;
    if (channelID == nil) {
        NSDictionary *channelInfo = [[[NSDictionary alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"MttHD-Channel" ofType:@"plist"]] autorelease];
        channelID = [[channelInfo objectForKey:@"ChannelId"] retain];
        if (channelID == nil)
            channelID = [(@"" CHANNEL) retain];
    }
    return channelID;
}


/*************************************
 *2.4版本之前的LC
 *************************************/
/*
 #ifdef __LC_1_0__
 //for ipad 1.0 preview
 const QQCHAR m_sLC[] = "7ED59E5738D1468";  //felixpeng 20101229
 const QQCHAR* pQUATail =  "&3601&V3";
 #endif
 
 #ifdef __LC_1_1__
 //for ipad 1.1
 const QQCHAR m_sLC[] = "0AED8B5C515E443";  // jhyehou 20110417
 const QQCHAR* pQUATail =  "&4048&V3";
 #endif
 
 #ifdef __LC_1_0__
 //for ipad 1.0 preview
 const QQCHAR m_sLC[] = "7ED59E5738D1468"; //felixpeng 20101229
 const QQCHAR* pQUATail =  "&3601&V3";
 #endif
 
 #ifdef __LC_1_1__
 //for ipad 1.1
 const QQCHAR m_sLC[] = "0AED8B5C515E443"; // jhyehou 20110417
 const QQCHAR* pQUATail =  "&4048&V3";
 #endif
 
 #endif
 
 #ifdef __LC_1_2__
 //for ipad 1.2
 const QQCHAR m_sLC[] = "5EF7B8BE55CA4C7"; // jhyehou 20110601
 const QQCHAR* pQUATail =  "&4429&V3";
 #endif
 
 #ifdef __LC_1_3__
 //for ipad 1.3
 const QQCHAR m_sLC[] = "7E6C04AA0AE6431"; // jhyehou 20110704
 const QQCHAR* pQUATail =  "&4886&V3";
 #endif
 
 #ifdef __LC_1_4__
 //for ipad 1.4
 const QQCHAR m_sLC[] = "98485343AB21481"; // paulineli 20110725
 const QQCHAR* pQUATail =  "&5722&V3";
 #endif
 
 #ifdef __LC_1_5__
 //for ipad 1.5
 const QQCHAR m_sLC[] = "0D4E771A2DD74A0"; // paulineli 20110802
 const QQCHAR* pQUATail =  "&5783&V3";
 #endif
 
 #ifdef __LC_1_6__
 //for ipad 1.6
 const QQCHAR m_sLC[] = "C266099EC3A0468"; // paulineli 20110818
 const QQCHAR* pQUATail =  "&5898&V3";
 #endif
 
 #ifdef __LC_1_7__
 //for ipad 1.7
 const QQCHAR m_sLC[] = "20C7144310ED4AA"; // paulineli 20110907
 const QQCHAR* pQUATail =  "&6018&V3";
 #endif
 
 #ifdef __LC_1_8__
 //for ipad 1.8
 const QQCHAR m_sLC[] = "C1419EE29B234CB"; // kmwen 20111008
 const QQCHAR* pQUATail =  "&6090&V3";
 #endif
 
 #ifdef __LC_1_9__
 //for ipad 1.9
 const QQCHAR m_sLC[] = "234FC93AD4E149E"; // paulineli 20111025
 const QQCHAR* pQUATail =  "&6116&V3";
 #endif
 
 #ifdef __LC_1_9_1__
 //for ipad 1.9.1
 const QQCHAR m_sLC[] = "20BF5732133B4BC"; // paulineli 20111104
 const QQCHAR* pQUATail =  "&6136&V3";
 #endif
 
 #ifdef __LC_2_0__
 //for ipad 1.9.1
 const QQCHAR m_sLC[] = "783B8BB9B9E3402"; // paulineli 20111121
 const QQCHAR* pQUATail =  "&6154&V3";
 //const QQCHAR m_sLC[] = "A180A7AA10C34ED"; // jhyehou 20111124 for test
 //const QQCHAR* pQUATail =  "&6076&V3";
 #endif
 
 #ifdef __LC_2_1__
 //for ipad 2.1
 const QQCHAR m_sLC[] = "3AACB1C879EB4C7"; // paulineli 20111208
 const QQCHAR* pQUATail =  "&6211&V3";
 #endif
 
 #ifdef __LC_2_2__
 //for ipad 2.2
 const QQCHAR m_sLC[] = "6EF698F5F391406"; // paulineli 20111227
 const QQCHAR* pQUATail =  "&6230&V3";
 #endif
 
 #ifdef __LC_2_3__
 //for ipad 2.3
 const QQCHAR m_sLC[] = "53324AB70EC0413"; // maxxiang 20120215
 const QQCHAR* pQUATail =  "&6280&V3";
 #endif
 ***/
 
@end
