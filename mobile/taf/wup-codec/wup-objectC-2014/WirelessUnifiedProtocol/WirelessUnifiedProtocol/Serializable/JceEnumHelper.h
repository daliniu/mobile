//
//  JceEnumHelper.h
//  WirelessUnifiedProtocol
//
//

#import <Foundation/Foundation.h>

BOOL isJceEnumStringEqual(NSString *s1, NSString *s2);

#define JceEnum             NSInteger
#define JceEnumUndefined    NSIntegerMin

@interface JceEnumHelper : NSObject

+ (NSString *)etos:(JceEnum)e;
+ (JceEnum)stoe:(NSString *)s;

/**
 * 以下接口用于转换不带枚举类型名前缀的枚举值字符串
 * 假设枚举值定义为JceEnum_Value1，此接口对应处理的字符串是"Value1"
 */
+ (NSString *)eto_s:(JceEnum)e;
+ (JceEnum)_stoe:(NSString *)s;

@end
