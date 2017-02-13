//
//  WupService.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "WupService.h"
#import "WupAgent.h"

@implementation WupService

@synthesize agent = _agent;

+ (id)service
{
    return [[[self alloc] init] autorelease];
}

+ (void)cancel:(id)stub
{
    [WupAgent cancel:stub];
}

- (id)init
{
    assert(0);
}

- (id)initWithAgent:(WupAgent *)agent
{
	if (self = [super init]) {
        _agent = [agent retain];
	}
	return self;
}

- (void)dealloc
{
    [_agent release];
    [super dealloc];
}

@end
