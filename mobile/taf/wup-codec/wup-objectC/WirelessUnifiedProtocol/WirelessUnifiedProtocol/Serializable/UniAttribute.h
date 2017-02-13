//
//  UniAttribute.h
//  WirelessUnifiedProtocol
//
//  Created by renjunyi on 12-4-17.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "JceObjectV2.h"

@interface UniAttribute : JceObjectV2

@property (nonatomic, retain, JV2_PROP_GS(uniAttributes)) NSMutableDictionary* JV2_PROP_EX(r, 0, uniAttributes, M09ONSStringM09ONSStringONSData);

+ (UniAttribute *)fromAttributeData:(NSData *)data;
- (NSData *)attributeData;

- (NSData *)attrValueWithName:(NSString *)name andType:(NSString *)type;
- (void)setAttrValue:(NSData *)data withName:(NSString *)name andType:(NSString *)type;

@end

#pragma mark - categories

@interface JceObject (uniAttribute)

+ (JceObject *)objectWithName:(NSString *)name andType:(NSString *)type inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name andType:(NSString *)type;

// for debug
+ (JceObject *)objectWithAttributeData:(NSData *)data;

@end

@interface JceObjectV2 (uniAttribute)

+ (JceObjectV2 *)objectWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name;

@end

@interface NSData (uniAttribute)

+ (NSData *)dataWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name;

@end

@interface NSString (uniAttribute)

+ (NSString *)stringWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name;

@end

@interface NSNumber (uniAttribute)

+ (BOOL)boolValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setBool:(BOOL)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (char)charValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setChar:(char)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (char)shortValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setShort:(short)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (char)intValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setInt:(int)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (long long)longlongValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setLonglong:(long long)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (float)floatValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setFloat:(float)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (double)doubleValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
+ (void)setDouble:(double)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name;

+ (NSNumber *)numberWithName:(NSString *)name inAttributes:(UniAttribute *)attrs;
- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name;

@end
