//
//  MttBrokerSecurityResponse.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttBrokerSecurityResponse.h"

@implementation MttBrokerSecurityResponse

@synthesize JV2_PROP_NM(o, 0, iSecurityLevel);
@synthesize JV2_PROP_NM(o, 1, strSecurityType);
@synthesize JV2_PROP_NM(o, 2, vDetailDescription);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.jce_strSecurityType = nil;
    self.jce_vDetailDescription = nil;
    [super dealloc];
}

+ (NSString *)jceType
{
    return @"Mtt.BrokerSecurityResponse";
}

@end
