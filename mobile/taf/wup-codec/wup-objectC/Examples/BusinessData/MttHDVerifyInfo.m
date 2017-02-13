//
//  MttHDVerifyInfo.m
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDVerifyInfo.h"

@implementation MttHDVerifyInfo

@synthesize verifyId = _verifyId;
@synthesize parameter = _parameter;

+ (MttHDVerifyInfo *)theSpecial
{
    static MttHDVerifyInfo *special = nil;
    if (special == nil) {
        special = [[super theSpecial] retain];
    }
    return special;
}

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    [_parameter release];
    [super dealloc];
}

@end
