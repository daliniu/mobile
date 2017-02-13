//
//  MttHDSplashInfo.m
//  MttHD
//
//  Created by renjunyi on 12-4-14.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDSplashInfo.h"

@implementation MttHDSplashInfo

@dynamic splashImage;
@synthesize md5StringOfSplashImage = _md5StringOfSplashImage;
@synthesize md5StringOfSplashConfig = _md5StringOfSplashConfig;
@synthesize startTime = _startTime;
@synthesize expirationTime = _expirationTime;
@synthesize duration = _duration;

+ (MttHDSplashInfo *)theSpecial
{
    static MttHDSplashInfo *special = nil;
    if (special == nil) {
        special = [[super theSpecial] retain];
    }
    return special;
}

+ (NSArray *)transients
{
    return [NSArray arrayWithObjects:@"splashImage", nil];
}

- (id)init
{
	if (self = [super init]) {
        _md5StringOfSplashImage = @"";
        _md5StringOfSplashConfig = @"";
        _startTime = [[NSDate alloc] initWithTimeIntervalSince1970:0];
        _expirationTime = [[NSDate alloc] initWithTimeIntervalSince1970:0];
        _duration = 0;
	}
	return self;
}

- (void)dealloc
{
    [_md5StringOfSplashImage release];
    [_md5StringOfSplashConfig release];
    [_startTime release];
    [_expirationTime release];
    [super dealloc];
}

- (UIImage *)defaultSplashImage
{
    return [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"DefaultSplash" ofType:@"png"]];
}

- (UIImage *)splashImage
{
    NSString *path = [[[self class] wupDataDirectory] stringByAppendingPathComponent:@"Splash.png"];
    return [UIImage imageWithContentsOfFile:path];
}

- (void)setSplashImage:(UIImage *)splashImage
{    
    NSString *path = [[[self class] wupDataDirectory] stringByAppendingPathComponent:@"Splash.png"];
    [UIImagePNGRepresentation(splashImage) writeToFile:path atomically:NO];
}

- (NSInteger)duration
{
    @synchronized (self) {
        return _duration;
    }
}

- (void)setDuration:(NSInteger)duration
{
    @synchronized (self) {
        if (duration < 1) duration = 1;
        if (duration > 3) duration = 3;
        _duration = duration;
    }
}

@end
