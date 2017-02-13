//
//  MttSplashReq+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-4.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDWupAdapters.h"
#import "MttHDSplashInfo.h"

@implementation MttSplashReq (Adapter)

+ (id)mtthdInstance
{
    MttSplashReq *instance = [MttSplashReq object];
    MttHDSplashInfo *splashInfo = [MttHDSplashInfo theSpecial];
    instance.jce_stUB = [MttUserBase mtthdInstance];
    instance.jce_iWidth = [[UIScreen mainScreen] applicationFrame].size.width;
    instance.jce_iHeight = [[UIScreen mainScreen] applicationFrame].size.height;
    instance.jce_sPicMd5 = splashInfo.md5StringOfSplashImage;
    instance.jce_sCondMd5 = splashInfo.md5StringOfSplashConfig;
    return instance;
}

@end
