//
//  MttVerifyReq+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttVerifyReq+adapter.h"
#import "MttUserBase+adapter.h"

@implementation MttVerifyReq (adapter)

+ (id)mtthdInstance
{
    MttVerifyReq *instance = [MttVerifyReq object];
    instance.stUB = [MttUserBase mtthdInstance];
    return instance;
}

@end
