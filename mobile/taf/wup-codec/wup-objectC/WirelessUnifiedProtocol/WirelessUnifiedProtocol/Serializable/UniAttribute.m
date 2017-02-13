//
//  UniAttribute.m
//  WirelessUnifiedProtocol
//
//  Created by renjunyi on 12-4-17.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "UniAttribute.h"

@implementation UniAttribute

@synthesize JV2_PROP_EX(r, 0, uniAttributes, M09ONSStringM09ONSStringONSData); 

- (id)init
{
	if (self = [super init]) {
        self.jce_uniAttributes = [NSMutableDictionary dictionary];
	}
	return self;
}

- (void)dealloc
{
    self.jce_uniAttributes = nil;
    [super dealloc];
}

+ (UniAttribute *)fromAttributeData:(NSData *)data
{
    UniAttribute *attribute = [[[UniAttribute alloc] init] autorelease];
    JceInputStream *attrStream = [JceInputStream streamWithData:data];
    JcePair *description = [JcePair pairWithValue:[JcePair pairWithValue:[NSData class] forKey:[NSString class]] forKey:[NSString class]];
    [attribute.jce_uniAttributes setDictionary:[attrStream readDictionary:0 required:YES description:description]];
    return attribute;
}

- (NSData *)attributeData
{
    JceOutputStream *stream = [JceOutputStream stream];
    [stream writeDictionary:self.jce_uniAttributes tag:0 required:YES];
    return [stream data];
}

- (NSData *)attrValueWithName:(NSString *)name andType:(NSString *)type
{
    NSDictionary *attribute = [self.jce_uniAttributes objectForKey:name];
    if (attribute) {
        NSAssert([attribute count] == 1, @"");
        NSAssert((type == nil || ([attribute objectForKey:type] == [[attribute allValues] lastObject])), @"type mismatch!");
        return [[attribute allValues] lastObject];
    }
    return nil;
}

- (void)setAttrValue:(NSData *)value withName:(NSString *)name andType:(NSString *)type
{
    NSAssert(name != nil && type != nil, @"");
    NSDictionary *attribute = [NSDictionary dictionaryWithObject:value forKey:type];
    [self.jce_uniAttributes setValue:attribute forKey:name];
}

@end

#pragma mark - categories

@implementation JceObject (uniAttribute)

+ (JceObject *)objectWithName:(NSString *)name andType:(NSString *)type inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:type];
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readObject:0 required:YES description:self];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name andType:(NSString *)type
{
    JceOutputStream *stream = [JceOutputStream stream];
    [stream writeObject:self tag:0 required:YES];
    [attrs setAttrValue:[stream data] withName:name andType:type];
}

// for debug
+ (JceObject *)objectWithAttributeData:(NSData *)data
{
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readObject:0 required:YES description:self];
}

@end

@implementation JceObjectV2 (uniAttribute)

+ (JceObjectV2 *)objectWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    return (JceObjectV2 *)[self objectWithName:name andType:[self jceType] inAttributes:attrs];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    [self setInAttributes:attrs withName:name andType:[self jceType]];
}

@end

@implementation NSData (uniAttribute)

+ (NSData *)dataWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"list<char>"];
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readData:0 required:YES];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:self.length];
    [stream writeData:self tag:0 required:YES];
    [attrs setAttrValue:[stream data] withName:name andType:@"list<char>"];
}

@end

@implementation NSString (uniAttribute)

+ (NSString *)stringWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"string"];
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readString:0 required:YES];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:[self lengthOfBytesUsingEncoding:NSUTF8StringEncoding]];
    [stream writeString:self tag:0 required:YES];
    [attrs setAttrValue:[stream data] withName:name andType:@"string"];
}

@end

@implementation NSNumber (uniAttribute)

+ (BOOL)boolValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"bool"];
    if (data == nil) return NO; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readInt:0];
}

+ (void)setBool:(BOOL)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"bool"];
}

+ (char)charValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"char"];
    if (data == nil) return 0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readInt:0];
}

+ (void)setChar:(char)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"char"];
}

+ (char)shortValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"short"];
    if (data == nil) return 0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readInt:0];
}

+ (void)setShort:(short)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"short"];
}

+ (char)intValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"int32"];
    if (data == nil) return 0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readInt:0];    
}

+ (void)setInt:(int)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"int32"];
}

+ (long long)longlongValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"int64"];
    if (data == nil) return 0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readInt:0]; 
}

+ (void)setLonglong:(long long)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:16];
    [stream writeInt:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"int64"];    
}

+ (float)floatValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"float"];
    if (data == nil) return 0.0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readFloat:0];
}

+ (void)setFloat:(float)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:16];
    [stream writeFloat:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"float"];
}

+ (double)doubleValueWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:@"double"];
    if (data == nil) return 0.0; // ASSERT_TRHOW_WS_EXCEPTION(data != nil);
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readDouble:0]; 
}

+ (void)setDouble:(double)value InAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:16];
    [stream writeDouble:value tag:0];
    [attrs setAttrValue:[stream data] withName:name andType:@"double"];
}

+ (NSNumber *)numberWithName:(NSString *)name inAttributes:(UniAttribute *)attrs
{
    NSData *data = [attrs attrValueWithName:name andType:nil];
    JceInputStream *stream = [JceInputStream streamWithData:data];
    return [stream readNumber:0 required:YES];
}

- (void)setInAttributes:(UniAttribute *)attrs withName:(NSString *)name
{
    JceOutputStream *stream = [JceOutputStream streamWithCapability:16];
    [stream writeNumber:self tag:0 required:YES];
    [attrs setAttrValue:[stream data] withName:name andType:[NSString stringWithUTF8String:[self objCType]]];
}

@end