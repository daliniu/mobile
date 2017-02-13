//
//  MttHDWupManager.h
//  MttHD
//
//  Created by 壬俊 易 on 12-3-20.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MttHDWupEnvironment.h"
#import "MttHDVerifyInfo.h"
#import "MttHDLoginInfo.h"
#import "MttHDPageConfigInfo.h"
#import "MttHDSplashInfo.h"
#import "MttHDStatInfo.h"

/**
 * 起始页更新成功后将发送此通知到Wup通知中心
 */
#define MttHDWupStartPageUpdated @"MttHDWupStartPageUpdated"

/**
 * 收到update响应将发送此通知到Wup通知中心，其userInfo包含下面几个键值：
 * "status" - NSNumber  0-不更新 1-提示更新 2-强制更新 3-检查更新
 * "title"  - NSString
 * "prompt" - NSString
 * "url"    - NSString
 */
#define MttHDWupReceiveUpdateResponse @"MttHDWupReceiveUpdateResponse"

@interface MttHDWupManager : NSObject

+ (MttHDWupManager *)sharedMttHDWupManager;
+ (NSNotificationCenter *)defaultNotificationCenter;

- (void)startStandardProcedure;

- (void)startVerify;
- (void)startLogin;
- (void)startConfig;
- (void)startStat;
- (void)startVideo;

@end
