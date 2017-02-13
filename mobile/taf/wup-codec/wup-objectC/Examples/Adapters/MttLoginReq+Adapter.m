//
//  MttLoginReq+Adapter.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttLoginReq+Adapter.h"
#import "MttHDWupManager.h"
#import "MttHDCCCryptEx.h"
#import "MttHDVerifyRes.h"
#import "MttHDWupAdapters.h"

@implementation MttLoginReq (adapter)

+ (id)mtthdInstance 
{
    MttLoginReq *instance = [MttLoginReq object];    
    instance.stUB = [MttUserBase mtthdInstance];
    instance.sOrigGUID = [MttHDLoginInfo theSpecial].guid;
    instance.vCrypt = [self key];
    instance.iWidth = [NSNumber numberWithInt:[UIScreen mainScreen].bounds.size.width];
    instance.iHeight = [NSNumber numberWithInt:[UIScreen mainScreen].bounds.size.height];
    instance.sChannel = MttHDWupEnvironment.channel;
    instance.eRelayProtoVer = [NSNumber numberWithInt:MttRelayProtoVer_1];
    instance.sMark = [self signature];
    instance.iVerifyId = [NSNumber numberWithInt:[MttHDVerifyInfo theSpecial].verifyId];
    return instance;
}

+ (NSData *)key
{
    MttHDLoginInfo *loginInfo = [MttHDLoginInfo theSpecial];
    
    unsigned char buff[32] = { 0 };
    [loginInfo.guid getBytes:buff length:(loginInfo.guid.length < 16 ? loginInfo.guid.length : 16)];
    for (int i = 31; i >= 0; i = i - 2) {
        buff[i] = rand();
        buff[i - 1] = buff[i / 2];
    }
    NSData *data = [NSData dataWithBytes:buff length:32];
    char* key = "\x7f\xca\xad\x73\x40\xe2\xba\x6b\x83\x58\xda\x0f\x0e\xfc\x1d\xa9\x32\xf1\x73\x34\x26\x3c\x9f\xf5";
    NSString *dKey = [NSString stringWithCString:key encoding:NSISOLatin1StringEncoding];
    return [data tripleDesWithKey:dKey];
}

+ (NSData *)signature
{
    // TODO:renjunyi 更改服务器verify资源
    MttHDVerifyInfo *verifyInfo = [MttHDVerifyInfo theSpecial];
    
    NSArray *info = [verifyInfo.parameter componentsSeparatedByString:@","];
    @try {
        NSData *resData = [MttHDVerifyRes verifyResWithName:[info objectAtIndex:0]];
        NSInteger startPos = [[info objectAtIndex:1] intValue];
        NSInteger endPos = [[info objectAtIndex:2] intValue];
        NSData *data = [resData subdataWithRange:NSMakeRange(startPos, endPos - startPos + 1)];
        return [data md5];
    }
    @catch (NSException *exception) {
        NSLog(@"create signature error! e=%@", exception);
    }
    return nil;
}

@end
