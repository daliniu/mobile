//
//  JceObject.m
//
//  Created by godwin.guo on 11-9-29. Modified by renjunyi on 11-12-1.
//  Copyright (c) 2011年 Tencent. All rights reserved.
//

#import "JceObject.h"
#import "objc/runtime.h"
#import "JceInputStream.h"
#import "JceOutputStream.h"

@implementation JceObject

+ (NSDictionary *)jcePropertiesWithEncodedTypes
{
	NSMutableDictionary *theProps = [NSMutableDictionary dictionary];
	objc_property_t *propList = class_copyPropertyList([self class], nil);
	for (int i = 0; propList[i] != nil; i++) {
		objc_property_t oneProp = propList[i];
		NSString *propName = [NSString stringWithUTF8String:property_getName(oneProp)];
		NSString *attrs = [NSString stringWithUTF8String: property_getAttributes(oneProp)];
		if ([attrs rangeOfString:@",R,"].location == NSNotFound) {
			NSArray *attrParts = [attrs componentsSeparatedByString:@","];
			if (attrParts != nil && [attrParts count] > 0) {
                NSString *propType = [[attrParts objectAtIndex:0] substringFromIndex:1];
                [theProps setObject:propType forKey:propName];
			}
		}
	}
	free(propList);
	return theProps;
}

+ (id)object
{
    return [[[self alloc] init] autorelease];
}

+ (id)fromData:(NSData *)data
{
    if ([data length] != 0) 
        return [[[[self alloc] init] fromData:data] autorelease];
    return nil;
}

- (id)fromData:(NSData *)data
{
    @try {
        JceInputStream *stream = [JceInputStream streamWithData:data];
        [self __unpack:stream];
        return self;
    }
    @catch (NSException *exception) {
        NSLog(@"%@", exception);
    }
    return nil;
}

- (NSData *)toData
{
    NSData* data = nil;
    JceOutputStream *stream = [[JceOutputStream alloc] init];
    @try {
        [self __pack:stream];
        data = [stream data];
    }
    @catch (NSException *exception) {
        NSLog(@"%@", exception);
    }
    @finally {
        [stream release];
    }
    return data;
}

+ (NSString *)jceType
{
    return nil;
}

- (NSString *)jceType
{
    return [[self class] jceType];
}

- (void)__pack:(JceOutputStream *)stream
{
    NSAssert(NO, @"需要由子类重写此方法！");
}

- (void)__unpack:(JceInputStream *)stream
{
    NSAssert(NO, @"需要由子类重写此方法！");
}

- (NSString *)description
{
    NSMutableString *description = [[NSMutableString alloc] init];
    [description appendString:@"{\n"];
    
    NSDictionary *props = [[self class] jcePropertiesWithEncodedTypes];
    for (NSString *propName in props) 
    {
        NSString *propType = [props objectForKey:propName];
        if ([propType isEqualToString:@"i"] ||  // int
            [propType isEqualToString:@"I"] ||  // unsigned int
            [propType isEqualToString:@"l"] ||  // long
            [propType isEqualToString:@"L"] ||  // usigned long
            [propType isEqualToString:@"q"] ||  // long long
            [propType isEqualToString:@"Q"] ||  // unsigned long long
            [propType isEqualToString:@"s"] ||  // short
            [propType isEqualToString:@"S"] ||  // unsigned short
            [propType isEqualToString:@"B"] ||  // bool or _Bool
            [propType isEqualToString:@"f"] ||  // float
            [propType isEqualToString:@"d"] ||  // double
            [propType isEqualToString:@"c"] ||  // char
            [propType isEqualToString:@"C"] )   // unsigned char
            [description appendFormat:@"\t%@: %@\n", propName, [[self valueForKey:propName] stringValue]];
        else if ([propType hasPrefix:@"@"])     // Object
            [description appendFormat:@"\t%@: %@\n", propName, [self valueForKey:propName]];
        else                                    // error
            [description appendFormat:@"\t%@: ???\n", propName];
    }
    
    [description appendString:@"}\n"];
    return [description autorelease];
}

#pragma mark - NSCoding

- (void)encodeWithCoder:(NSCoder *)aCoder
{    
    [aCoder encodeDataObject:[self toData]];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    if (self = [super init]) {
        [self fromData:[aDecoder decodeDataObject]];
    }
    return self;
}

@end
