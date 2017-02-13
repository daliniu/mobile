//
//  MttBrokerSecurityRequest.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttBrokerSecurityRequest.h"

@implementation MttBrokerSecurityRequest

@synthesize JV2_PROP_NM(o, 0, stUserInfo);
@synthesize JV2_PROP_NM(r, 1, cType);
@synthesize JV2_PROP_NM(o, 2, strUrl);
@synthesize JV2_PROP_NM(o, 3, strSha1);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.jce_stUserInfo = nil;
    self.jce_strUrl = nil;
    self.jce_strSha1 = nil;
    [super dealloc];
}

+ (NSString *)jceType
{
    return @"MTT.BrokerSecurityRequest";
}

@end

@implementation MttBrokerSecurityRequest (adapter)

+ (id)mtthdInstance
{
    MttBrokerSecurityRequest *instance = [MttBrokerSecurityRequest object];
    return instance;
}

@end
