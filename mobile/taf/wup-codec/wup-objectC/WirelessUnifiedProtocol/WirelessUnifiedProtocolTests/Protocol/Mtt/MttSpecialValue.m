// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.1.4.2 by WSRD Tencent.
// Generated from `/usr/local/resin_system.mqq.com/webapps/communication/taf/upload/renjunyi/MTT.jce'
// **********************************************************************

#import "JceObjectV2.h"
#import "MttSpecialValue.h"

#if JCEV2_ENUM_ETOS_AND_STOE_SUPPORTED

@implementation MttSpecialValueHelper

+ (NSString *)etos:(MttSpecialValue)e
{
    switch(e){
        case MttSpecialValue_ONELINK: return @"MttSpecialValue_ONELINK";
        case MttSpecialValue_MTKWAP: return @"MttSpecialValue_MTKWAP";
        case MttSpecialValue_ANDROIDWM: return @"MttSpecialValue_ANDROIDWM";
        default: return @"";
    }
}

+ (MttSpecialValue)stoe:(NSString *)s
{
    if(isJceEnumStringEqual(s, @"MttSpecialValue_ONELINK")) return MttSpecialValue_ONELINK;
    if(isJceEnumStringEqual(s, @"MttSpecialValue_MTKWAP")) return MttSpecialValue_MTKWAP;
    if(isJceEnumStringEqual(s, @"MttSpecialValue_ANDROIDWM")) return MttSpecialValue_ANDROIDWM;
    return NSIntegerMin;
}

@end

#endif