//
//  MttLoginService.m
//
//  Created by 壬俊 易 on 12-3-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttLoginService.h"
#import "MttLoginAgent.h"
#import "MttHDWupAdapters.h"
#import "MttHDWupManager.h"
#import "MttHDWupEnvironment.h"

@implementation MttLoginService

- (id)init
{
	if (self = [super initWithAgent:[MttLoginAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)loginWithCompleteBlock:(WupServiceBasicBlock)completeBlock failedBlock:(WupServiceBasicBlock)failedBlock;
{
    MttLoginAgent *agent = (MttLoginAgent *)[self agent];
    id stub = [agent login:[MttLoginReq mtthdInstance] withCompleteHandle:^(MttLoginRsp * rsp) {
        if (rsp != nil) {
            MttHDLoginInfo *loginInfo = [MttHDLoginInfo theSpecial];
            loginInfo.guid = rsp.sGUID;
            loginInfo.auth = rsp.vAuth;
            loginInfo.loginTime = rsp.iTime.intValue;
            loginInfo.statValue = rsp.iStatValue.intValue;
            [loginInfo updateProxyList:rsp.vProxyList];
            [loginInfo updateRelayList:rsp.vRelayList];
            [loginInfo save];
            if (completeBlock) 
                completeBlock();
        }
        else {
            if (failedBlock)
                failedBlock();
        }
    }];
    return stub;
}

- (id)updateWithCompleteBlock:(void (^)(MttUpdateRsp *))completeBlock failedBlock:(WupServiceBasicBlock)failedBlock
{
    MttLoginAgent *agent = (MttLoginAgent *)[self agent];
    id stub = [agent update:[MttUserBase mtthdInstance] withCompleteHandle:^(MttUpdateRsp * rsp) {
        if (rsp != nil) {
            NSMutableDictionary *userInfo = [NSMutableDictionary dictionary];
            [userInfo setObject:rsp.iStatus forKey:@"status"];
            [userInfo setObject:rsp.sMark forKey:@"title"];
            [userInfo setObject:rsp.sText forKey:@"prompt"];
            [userInfo setObject:rsp.sURL forKey:@"url"];
            // TODO:renjunyi
            if (completeBlock) 
                completeBlock(rsp);
        }
        else {
            if (failedBlock)
                failedBlock();
        }
    }];
    return stub;
}

@end
