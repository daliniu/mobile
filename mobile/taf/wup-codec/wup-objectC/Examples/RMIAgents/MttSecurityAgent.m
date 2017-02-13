//
//  MttSecurityAgent.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttSecurityAgent.h"
#import "MttHDWupEnvironment.h"

@implementation MttSecurityAgent

- (id)init
{
    if (self = [super initWithServant:@"Security"]) {
        self.delegate = [MttHDWupAgentHelper relayAgentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttBrokerSecurityResponse *)security:(MttBrokerSecurityRequest *)request
{
    UniPacket *uniPacket = [UniPacket packet];
    [uniPacket putObjectAttr:@"Security" value:request];
    UniPacket *rspPacket = [self invocation:@"Security" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"BrokerSecurityResponse" forClass:[MttBrokerSecurityResponse class]];
    return NO;
}

- (id)security:(MttBrokerSecurityRequest *)request withCompleteHandle:(void (^)(MttBrokerSecurityResponse *))handle
{    
    UniPacket *uniPacket = [UniPacket packet];
    [uniPacket putObjectAttr:@"Security" value:request];
    return [self invocation:@"Security" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttBrokerSecurityResponse *rsp = [response getObjectAttr:@"BrokerSecurityResponse" forClass:[MttBrokerSecurityResponse class]];
        handle(rsp);
    }];
}

@end
