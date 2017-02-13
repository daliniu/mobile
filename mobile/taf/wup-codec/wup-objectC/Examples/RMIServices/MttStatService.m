//
//  MttStatService.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttStatService.h"
#import "MttStatAgent.h"
#import "MttHDWupAdapters.h"
#import "MttSTStat+Adapter.h"
#import "MttHDWupManager.h"

@implementation MttStatService

- (id)init
{
	if (self = [super initWithAgent:[MttStatAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)statWithCompleteBlock:(WupServiceBasicBlock)completeBlock failedBlock:(WupServiceBasicBlock)failedBlock
{
    MttStatAgent *agent = (MttStatAgent *)[self agent];
    id stub = [agent stat:[MttSTStat mtthdInstance] withCompleteHandle:^(bool ret) {
        if (ret != NO) {
            [MttSTStat mtthdInstanceReset];
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

@end
