//
//  MttConfigAgent.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-21.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttConfigAgent.h"
#import "MttHDWupEnvironment.h"

@implementation MttConfigAgent

- (id)init
{
    if (self = [super initWithServant:@"config"]) {
        self.delegate = [MttHDWupAgentHelper agentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttPageConfigRsp *)getPageConfig:(MttPageConfigReq *)request
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"req" value:request];
    UniPacket *rspPacket = [self invocation:@"getPageConfig" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"rsp" forClass:[MttPageConfigRsp class]];
    return nil;
}

- (id)getPageConfig:(MttPageConfigReq *)request withCompleteHandle:(void (^)(MttPageConfigRsp *))handle
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"req" value:request];
    return [self invocation:@"getPageConfig" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttPageConfigRsp *rsp = [response getObjectAttr:@"rsp" forClass:[MttPageConfigRsp class]];
        handle(rsp);
    }];
}

- (MttSplashRsp *)getSplash:(MttSplashReq *)request
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"req" value:request];
    UniPacket *rspPacket = [self invocation:@"getSplash" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"rsp" forClass:[MttSplashRsp class]];
    return nil;
}

- (id)getSplash:(MttSplashReq *)request withCompleteHandle:(void (^)(MttSplashRsp *))handle
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"req" value:request];
    return [self invocation:@"getSplash" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttSplashRsp *rsp = [response getObjectAttr:@"rsp" forClass:[MttSplashRsp class]];
        handle(rsp);
    }];
}

@end
