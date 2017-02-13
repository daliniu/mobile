//
//  MttSecurityAgent.h
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import "MttBrokerSecurityRequest.h"
#import "MttBrokerSecurityResponse.h"

@interface MttSecurityAgent : WupAgent

- (MttBrokerSecurityResponse *)security:(MttBrokerSecurityRequest *)request;
- (id)security:(MttBrokerSecurityRequest *)request withCompleteHandle:(void (^)(MttBrokerSecurityResponse *))handle;

@end
