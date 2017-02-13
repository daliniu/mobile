//
//  MttUserBase+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttUserBase+adapter.h"
#import "MttHDWupManager.h"

@implementation MttUserBase (adapter)

+ (id)mtthdInstance 
{
    MttUserBase *instance = [MttUserBase object];
    instance.jce_sIMEI = [MttHDWupEnvironment imei];
    instance.jce_sGUID = [MttHDLoginInfo theSpecial].guid;
    instance.jce_sQUA = [MttHDWupEnvironment qUA];
    instance.jce_sLC = [MttHDWupEnvironment lc];
    instance.jce_sCellphone = @"";
    instance.jce_sUin = [MttHDWupEnvironment qqAccount];
    instance.jce_sChannel = [MttHDWupEnvironment channel];
    instance.jce_iServerVer = MttServerVer_IPAD;
    return instance;
}

@end
