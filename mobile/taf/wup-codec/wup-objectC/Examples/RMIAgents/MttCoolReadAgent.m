//
//  MttCoolReadAgent.m
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//
#import "MttCoolReadAgent.h"
#import "MttHDWupEnvironment.h"

@implementation MttVideoAgent

- (id)init
{
    if (self = [super initWithServant:@"coolRead"]) {
        self.delegate = [MttHDWupAgentHelper proxyAgentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (MttVideoResponse *)getVideo:(MttVideoRequest *)request
{
    UniPacket *uniPacket = [UniPacket packet];
    [uniPacket putObjectAttr:@"req" value:request];
    UniPacket *rspPacket = [self invocation:@"getVideo" parameter:uniPacket];
    if (rspPacket != nil)
        return [rspPacket getObjectAttr:@"VideoApp" forClass:[MttVideoResponse class]];
    return NO;
}
- (id)getVideo:(MttVideoRequest *)request withCompleteHandle:(void (^)(MttVideoResponse *))handle
{
    UniPacket *uniPacket = [UniPacket packet];
    [uniPacket putObjectAttr:@"req" value:request];
    return [self invocation:@"getVideo" parameter:uniPacket completeHandle:^(UniPacket * response) {
        MttVideoResponse *rsp = [response getObjectAttr:@"rsp" forClass:[MttVideoResponse class]];
        handle(rsp);
    }];
}

@end
