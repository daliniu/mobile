//
//  MttStatAgent.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-21.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttStatAgent.h"
#import "MttHDCCCryptEx.h"
#import "MttHDWupEnvironment.h"

#define MTT_STAT_KEY "\x62\xe8\x39\xac\x8d\x75\x37\x79"

@implementation MttStatAgent

- (id)init
{
    if (self = [super initWithServant:@"stat"]) {
        self.delegate = [MttHDWupAgentHelper agentHelper];
        self.retryUsingOtherServers = YES;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (bool)stat:(MttSTStat *)request
{
    UniPacket *uniPacket = [UniPacket packet];
    NSString *key = [NSString stringWithCString:MTT_STAT_KEY encoding:NSISOLatin1StringEncoding];
    [uniPacket putDataAttr:@"crypt" value:[[request toData] desWithKey:key]];
    UniPacket *rspPacket = [self invocation:@"stat" parameter:uniPacket];
    if (rspPacket != nil)
        return [NSNumber boolValueWithName:@"" inAttributes:rspPacket.attributes];
    return NO;
}

- (id)stat:(MttSTStat *)request withCompleteHandle:(void (^)(bool))handle
{    
    UniPacket *uniPacket = [UniPacket packet];
    NSString *key = [NSString stringWithCString:MTT_STAT_KEY encoding:NSISOLatin1StringEncoding];
    [uniPacket putDataAttr:@"crypt" value:[[request toData] desWithKey:key]];
    return [self invocation:@"stat" parameter:uniPacket completeHandle:^(UniPacket * response) {
        int value = [NSNumber intValueWithName:@"" inAttributes:response.attributes];
        handle((value == 0 ? YES : NO));
    }];
}

@end
