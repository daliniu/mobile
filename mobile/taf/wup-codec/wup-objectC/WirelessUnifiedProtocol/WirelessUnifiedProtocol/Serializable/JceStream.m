//
//  JceStream.m
//
//  Created by 壬俊 易 on 12-1-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "JceStream.h"

@implementation JceStream

@class JceInputStream;
@class JceOutputStream;

@synthesize streamBuffer = _streamBuffer;
@synthesize streamSize = _streamSize;
@synthesize cursor = _cursor;

- (id)init
{
	if (self = [super init])
    {
		_streamBuffer = nil;
		_streamSize = 0;
		_cursor = 0;
	}
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (NSData *)data
{
    return nil;
}

- (NSString *)description
{
    NSData *originData = [self data];
    assert(_cursor <= originData.length);
    
    UInt8 *buff = (UInt8 *)malloc(sizeof(UInt8) * originData.length);
    [originData getBytes:buff length:originData.length];
    NSString *description = [NSString stringWithFormat:@"%@ ^cursor %@",
                             [NSData dataWithBytes:buff length:_cursor],
                             [NSData dataWithBytes:(buff + _cursor) length:(originData.length - _cursor)]];
    free(buff);
    return description;
}

@end

#pragma mark -

@implementation JcePair

@synthesize key = _key;
@synthesize value = _value;

+ (JcePair *)pairWithValue:(id)value forKey:(id)key
{
    JcePair *pair = [[JcePair alloc] init];
    pair.value = value;
    pair.key = key;
    return [pair autorelease];
}

- (id)init
{
    if (self = [super init]) 
    {
        _key = nil;
        _value = nil;
    }
    return self;
}

- (void)dealloc
{
    [_key release];
    [_value release];
    [super dealloc];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@, %@>", NSStringFromClass([self.key class]), NSStringFromClass([self.value class])];
}

@end
