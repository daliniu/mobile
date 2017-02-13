//
//  MttLoginAgent.m
//
//  Created by 壬俊 易 on 12-3-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttLoginAgent.h"
#import "MttHDWupEnvironment.h"

@implementation MttLoginAgent

- (id)init
{
    if (self = [super initWithServant:@"login"]) {
        self.delegate = [MttHDWupAgentHelper agentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttLoginRsp *)login:(MttLoginReq *)request 
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"stLReq" type:@"MTT.LoginReq" value:request];
    UniPacket *rspPacket = [self invocation:@"login" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"stLRsp" forClass:[MttLoginRsp class]];
    return nil;
}

- (id)login:(MttLoginReq *)request withCompleteHandle:(void (^)(MttLoginRsp *))handle
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"stLReq" type:@"MTT.LoginReq" value:request];
    return [self invocation:@"login" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttLoginRsp *rsp = [response getObjectAttr:@"stLRsp" forClass:[MttLoginRsp class]];
        handle(rsp);
    }];
}

- (MttUpdateRsp *)update:(MttUserBase *)request 
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"ub" type:@"MTT.UserBase" value:request];
    UniPacket *rspPacket = [self invocation:@"update" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"uRsp" forClass:[MttUpdateRsp class]];
    return nil;
}

- (id)update:(MttUserBase *)request withCompleteHandle:(void (^)(MttUpdateRsp *))handle
{
    UniPacket *uniPacket = [[[UniPacket alloc] init] autorelease];
    [uniPacket putObjectAttr:@"ub" type:@"MTT.UserBase" value:request];
    return [self invocation:@"update" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttUpdateRsp *rsp = [response getObjectAttr:@"uRsp" forClass:[MttUpdateRsp class]];
        handle(rsp);
    }];
}

@end
