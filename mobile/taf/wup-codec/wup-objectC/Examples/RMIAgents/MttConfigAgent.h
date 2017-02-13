//
//  MttConfigAgent.h
//  MttHD
//
//  Created by 壬俊 易 on 12-3-21.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttPageConfigReq.h>
#import <WirelessUnifiedProtocol/MttPageConfigRsp.h>
#import <WirelessUnifiedProtocol/MttSplashReq.h>
#import <WirelessUnifiedProtocol/MttSplashRsp.h>

@interface MttConfigAgent : WupAgent

- (MttPageConfigRsp *)getPageConfig:(MttPageConfigReq *)request;
- (id)getPageConfig:(MttPageConfigReq *)request withCompleteHandle:(void (^)(MttPageConfigRsp *))handle;

- (MttSplashRsp *)getSplash:(MttSplashReq *)request;
- (id)getSplash:(MttSplashReq *)request withCompleteHandle:(void (^)(MttSplashRsp *))handle;

@end
