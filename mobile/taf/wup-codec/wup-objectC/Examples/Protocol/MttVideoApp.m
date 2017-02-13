//
//  MttVideoApp.m
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//


#import "MttVideoApp.h"

@implementation MttEpisodeNode

@synthesize  JV2_PROP_NM(o, 0, sEpisodeUrl);
@synthesize  JV2_PROP_NM(o, 1, sEpisodeTitle);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.jce_sEpisodeUrl = nil;
    self.jce_sEpisodeTitle = nil;
    [super dealloc];
}

+ (NSString *)jceType
{
    return @"MTT.CoolReadEpisodeStruct";
}

@end

@implementation MttEpisodeNode (adapter)

+ (id)mtthdInstance
{
    MttEpisodeNode *instance = [MttEpisodeNode object];
    return instance;
}

@end

@implementation MttIpadViewData

@synthesize  JV2_PROP_NM(o, 0, sWebLink);
@synthesize  JV2_PROP_NM(o, 1, sWebLinkTitle);
@synthesize  JV2_PROP_NM(o, 2, sWebSubTitle);
@synthesize  JV2_PROP_NM(o, 3, sWebCoverUrl);
@synthesize  JV2_PROP_NM(o, 4, sWebEpsInfo);
@synthesize  JV2_PROP_EX(o, 5, vEpisodeList, VOMttEpisodeNode);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.jce_sWebLink = nil;
    self.jce_sWebLinkTitle = nil;
    self.jce_sWebSubTitle = nil;
    self.jce_sWebCoverUrl = nil;
    self.jce_sWebEpsInfo = nil;
    self.jce_vEpisodeList = nil;
    [super dealloc];
}

+ (NSString *)jceType
{
    return @"MTT.IpadViewData";
}

@end

@implementation MttIpadViewData (adapter)

+ (id)mtthdInstance
{
    MttIpadViewData *instance = [MttIpadViewData object];
    return instance;
}

@end

@implementation MttChannel


@synthesize  JV2_PROP_NM(o, 0, iAppId);
@synthesize  JV2_PROP_NM(o, 1, sCid);
@synthesize  JV2_PROP_NM(o, 2, sName);
@synthesize  JV2_PROP_NM(o, 3, iOrder);
@synthesize  JV2_PROP_NM(o, 4, sIconUrl);
@synthesize  JV2_PROP_NM(o, 5, sDescritpion);
@synthesize  JV2_PROP_NM(o, 6, iDataFrom);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.jce_sCid = nil;
    self.jce_sName = nil;
    self.jce_sIconUrl = nil;
    self.jce_sDescritpion = nil;
    [super dealloc];
}

+ (NSString *)jceType
{
    return @"MTT.Channel";
}

@end

@implementation MttChannel (adapter)

+ (id)mtthdInstance
{
    MttChannel *instance = [MttChannel object];
    return instance;
}

@end

@implementation MttVideoChannel

@synthesize JV2_PROP_NM(o, 0, stChannel);
@synthesize JV2_PROP_EX(o, 1, vVideo, VOMttIpadViewData);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.jce_stChannel = nil;
    self.jce_vVideo = nil;
    [super dealloc];
}

+ (NSString *)jceType
{
    return @"MTT.VideoChannel";
}

@end

@implementation MttVideoChannel (adapter)

+ (id)mtthdInstance
{
    MttVideoChannel *instance = [MttVideoChannel object];
    return instance;
}

@end

@implementation MttVideoResponse

@synthesize  JV2_PROP_NM(o, 0, sAppTitle);
@synthesize  JV2_PROP_NM(o, 1, sSearchUrl);
@synthesize  JV2_PROP_NM(o, 2, iVideoNum);
@synthesize  JV2_PROP_EX(o, 3, vData, VOMttVideoChannel);
@synthesize  JV2_PROP_NM(o, 4, sMd5);

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    self.jce_sAppTitle = nil;
    self.jce_sSearchUrl = nil;
    self.jce_vData = nil;
    self.jce_sMd5 = nil;
    [super dealloc];
}

+ (NSString *)jceType
{
    return @"MTT.VideoApp";
}

@end

@implementation MttVideoResponse (adapter)

+ (id)mtthdInstance
{
    MttVideoResponse *instance = [MttVideoResponse object];
    return instance;
}

@end


