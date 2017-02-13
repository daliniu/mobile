//
//  MttBrokerSecurityRequest.h
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttUserBase.h>

enum {
    MttSecurityCheckType_WEBURL,    //网址检测
    MttSecurityCheckType_SOFT       //资源检测 
};
#define MttSecurityCheckType NSInteger

@interface MttBrokerSecurityRequest : JceObjectV2

@property (nonatomic, retain, JV2_PROP_GS(stUserInfo)) MttUserBase* JV2_PROP_NM(o, 0, stUserInfo);
@property (nonatomic, assign, JV2_PROP_GS(cType)) MttSecurityCheckType JV2_PROP_NM(r, 1, cType);
@property (nonatomic, retain, JV2_PROP_GS(strUrl)) NSString* JV2_PROP_NM(o, 2, strUrl);
@property (nonatomic, retain, JV2_PROP_GS(strSha1)) NSString* JV2_PROP_NM(o, 3, strSha1);

@end

@interface MttBrokerSecurityRequest (adapter)

+ (id)mtthdInstance;

@end
