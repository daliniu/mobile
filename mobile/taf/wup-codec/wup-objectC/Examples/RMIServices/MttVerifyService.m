//
//  MttVerifyService.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttVerifyService.h"
#import "MttVerifyAgent.h"
#import "MttHDWupAdapters.h"
#import "MttHDWupManager.h"
#import "MttHDWupEnvironment.h"

@implementation MttVerifyService 

- (id)init
{
	if (self = [super initWithAgent:[MttVerifyAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)verifyWithCompleteBlock:(WupServiceBasicBlock)completeBlock failedBlock:(WupServiceBasicBlock)failedBlock
{
    id stub = nil;
    
    MttHDLoginInfo *loginInfo = [MttHDLoginInfo theSpecial];
    if (loginInfo.auth.length > 0) {
        if (completeBlock)
            completeBlock();
    }
    else {
        MttVerifyAgent *agent = (MttVerifyAgent *)[self agent];
        stub = [agent verify:[MttVerifyReq mtthdInstance] withCompleteHandle:^(MttVerifyRsp * rsp) {
            if (rsp != nil) {
                MttHDVerifyInfo* verifyInfo = [MttHDVerifyInfo theSpecial];
                verifyInfo.verifyId = rsp.iVerifyId.intValue;
                verifyInfo.parameter = rsp.sParameter;
                [verifyInfo save];
                if (completeBlock)
                    completeBlock();
            }
            else {
                if (failedBlock)
                    failedBlock();
            }
        }];
    }
    return stub;
}

@end
