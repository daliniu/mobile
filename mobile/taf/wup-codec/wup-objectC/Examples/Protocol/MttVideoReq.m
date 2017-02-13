//
//  MttVideoReq.m
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//
#import "MttVideoReq.h"
#import "MttHDWupManager.h"
#import "MttHDWupAdapters.h"
#import "MttHDVideoInfo.h"

@implementation MttVideoRequest

@synthesize JV2_PROP_NM(o, 0, stUB);
@synthesize JV2_PROP_NM(o, 1, sMd5);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.jce_stUB = nil;
    self.jce_sMd5 = nil;
    [super dealloc];
}

+ (NSString *)jceType
{
    return @"MTT.VideoReq";
}

@end

@implementation MttVideoRequest (adapter)

+ (id)mtthdInstance
{
    MttVideoRequest *instance = [MttVideoRequest object];
    instance.jce_stUB = [MttUserBase mtthdInstance];
    instance.jce_sMd5 = [MttHDVideoInfo theSpecial].sMd5;
    return instance;
}

@end
