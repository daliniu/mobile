// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.1.4.2 by WSRD Tencent.
// Generated from `/usr/local/resin_system.mqq.com/webapps/communication/taf/upload/renjunyi/MTT.jce'
// **********************************************************************

#import "MttHotListReq.h"

@implementation MttHotListReq

@synthesize JV2_PROP_NM(o,0,sMd5);
@synthesize JV2_PROP_NM(o,1,iHotType);
@synthesize JV2_PROP_NM(o,2,sQua);

+ (void)initialize
{
    if (self == [MttHotListReq class]) {
        [super initialize];
    }
}

- (id)init
{
    if (self = [super init]) {
        JV2_PROP(sMd5) = DefaultJceString;
        JV2_PROP(sQua) = DefaultJceString;
    }
    return self;
}

- (void)dealloc
{
    JV2_PROP(sMd5) = nil;
    JV2_PROP(sQua) = nil;
    [super dealloc];
}

+ (NSString*)jceType
{
    return @"MTT.HotListReq";
}

@end