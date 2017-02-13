//
//  MttVerifyAgent.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttVerifyAgent.h"
#import "MttHDCCCryptEx.h"
#import "MttHDWupEnvironment.h"

#define MTT_VERIFY_KEY "\x25\x92\x3c\x7f\x2a\xe5\xef\x92"

@implementation MttVerifyAgent

- (id)init
{
    if (self = [super initWithServant:@"v"]) {
        self.delegate = [MttHDWupAgentHelper agentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttVerifyRsp *)verify:(MttVerifyReq *)request
{
    UniPacket *uniPacket = [UniPacket packet];
    NSString *key = [NSString stringWithCString:MTT_VERIFY_KEY encoding:NSISOLatin1StringEncoding];
    [uniPacket putDataAttr:@"v" value:[[request toData] desWithKey:key]];
    UniPacket *rspPacket = [self invocation:@"verify" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"rsp" forClass:[MttVerifyRsp class]];
    return nil;
}

- (id)verify:(MttVerifyReq *)request withCompleteHandle:(void (^)(MttVerifyRsp *))handle
{
    UniPacket *uniPacket = [UniPacket packet];
    NSString *key = [NSString stringWithCString:MTT_VERIFY_KEY encoding:NSISOLatin1StringEncoding];
    [uniPacket putDataAttr:@"v" value:[[request toData] desWithKey:key]];
    return [self invocation:@"verify" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttVerifyRsp *rsp = [response getObjectAttr:@"rsp" forClass:[MttVerifyRsp class]];
        handle(rsp);
    }];
}

@end
