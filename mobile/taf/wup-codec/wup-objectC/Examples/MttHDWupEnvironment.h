//
//  MttHDWupEnvironment.h
//  MttHD
//
//  Created by 壬俊 易 on 12-3-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>

@interface MttHDWupEnvironment : NSObject

+ (NSString *)imei;
+ (NSString *)qUA;
+ (NSString *)lc;
+ (NSString *)lcId;
+ (NSString *)channel;
+ (NSString *)qAuth;
+ (NSString *)qGuid;

+ (NSString *)qqAccount;
+ (void)setQQAccount:(NSString *)qqAcount;

@end

@interface MttHDWupAgentHelper : NSObject <WupAgentDelegate> {
    NSMutableArray *_serverUrls;
    NSString *_qAuth;
    NSString *_qGuid;
}

+ (MttHDWupAgentHelper *)agentHelper;
+ (MttHDWupAgentHelper *)proxyAgentHelper;
+ (MttHDWupAgentHelper *)relayAgentHelper;

- (NSString *)userAgent;
- (NSString *)qUA;
- (NSString *)qAuth;
- (NSString *)qGuid;
- (NSURL *)serverUrl;
- (void)invalidateServerUrl:(NSURL *)url;


@end
