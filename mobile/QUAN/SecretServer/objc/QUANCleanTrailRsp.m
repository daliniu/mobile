// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.1.4.3 by WSRD Tencent.
// Generated from `SecretBase.jce'
// **********************************************************************

#import "QUANCleanTrailRsp.h"

@implementation QUANCleanTrailRsp

@synthesize JV2_PROP_NM(o,0,iStatus);

+ (void)initialize
{
    if (self == [QUANCleanTrailRsp class]) {
        [super initialize];
    }
}

- (id)init
{
    if (self = [super init]) {
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

+ (NSString*)jceType
{
    return @"QUAN.CleanTrailRsp";
}

@end
