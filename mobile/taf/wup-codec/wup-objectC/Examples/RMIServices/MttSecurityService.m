//
//  MttSecurityService.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttSecurityService.h"
#import "MttSecurityAgent.h"

@implementation MttSecurityService

- (id)init
{
	if (self = [super initWithAgent:[MttSecurityAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)security:(NSString *)url checkType:(int)type WithCompleteBlock:(void (^)(MttBrokerSecurityResponse *))completeBlock failedBlock:(WupServiceBasicBlock)failedBlock
{
    MttSecurityAgent *agent = (MttSecurityAgent *)[self agent];
    MttBrokerSecurityRequest *request = [MttBrokerSecurityRequest object];
    request.jce_cType  = type;
    request.jce_strUrl = url;
    id stub = [agent security:request withCompleteHandle:^(MttBrokerSecurityResponse *rsp) {
        if (rsp != NO) {
            if (rsp) {
                
            }
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
