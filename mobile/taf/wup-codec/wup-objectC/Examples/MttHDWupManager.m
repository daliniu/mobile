//
//  MttHDWupManager.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-20.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDWupManager.h"
#import "MttVerifyService.h"
#import "MttLoginService.h"
#import "MttConfigService.h"
#import "MttStatService.h"
#import "MttHDWupEnvironment.h"
#import "MttCoolReadService.h"

@interface MttHDWupManager ()

@property (readonly) NSNotificationCenter *wupNotificationCenter;

@end

@implementation MttHDWupManager

@synthesize wupNotificationCenter = _wupNotificationCenter;

+ (MttHDWupManager *)sharedMttHDWupManager
{
    static MttHDWupManager *sharedMttHDWupManager = nil; 
    @synchronized(self) {
        if (sharedMttHDWupManager == nil)
            sharedMttHDWupManager = [[[self alloc] init] retain];
    }
    return sharedMttHDWupManager;
}

+ (NSNotificationCenter *)defaultNotificationCenter
{
    return [self sharedMttHDWupManager].wupNotificationCenter;
}

- (id)init
{
	if (self = [super init]) {
        _wupNotificationCenter = [[NSNotificationCenter alloc] init];
	}
	return self;
}

- (void)dealloc
{
    [_wupNotificationCenter release];
    [super dealloc];
}

- (void)startStandardProcedure
{
    [self startVerify];
    //加快启动速度，推迟10秒拉取视频
    [self performSelector:@selector(startVideo) withObject:nil afterDelay:10];
}

- (void)startVerify
{
    MttVerifyService *verifyService = [MttVerifyService service];
    
    NSLog(@"===> start verify!");
    [verifyService verifyWithCompleteBlock:^{
        NSLog(@"verify success!");
        [self startLogin];
    } failedBlock: ^{
        NSLog(@"verify failed!");
    }];
}

- (void)startLogin
{
    MttLoginService *loginService = [MttLoginService service];
    
    NSLog(@"===> start login!");
    [loginService loginWithCompleteBlock:^{
        NSLog(@"login success!");
        [self startConfig];
        [self startStat];
    } failedBlock:^{
        NSLog(@"login failed!");
    }];
}

- (void)startConfig
{
    MttConfigService *configService = [MttConfigService service];
    
    NSLog(@"===> start getPageConfig!");
    [configService getPageConfigWithCompleteBlock:^{
        NSLog(@"getPageConfig success!");
    } failedBlock:^{
        NSLog(@"getPageConfig failed!");
    }];
    
    NSLog(@"===> start getSplash!");
    [configService getSplashWithCompleteBlock:^{
        NSLog(@"getSplash success!");
    } failedBlock:^{
        NSLog(@"getSplash failed!");
    }];
}

- (void)startStat
{
    MttStatService *statService = [MttStatService service];
    
    NSLog(@"===> start stat!");
    [statService statWithCompleteBlock:^{
        NSLog(@"stat success!");
    } failedBlock:^{
        NSLog(@"stat failed!");
    }];
}
-(void)startVideo
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(startVideo) object:nil];
    MttVideoService *videoSevice = [MttVideoService service];
    NSLog(@"===> start getVideo!");
    [videoSevice getVideoWithCompleteBlock:^{
        NSLog(@"getVideo success!");
    } failedBlock:^{
        NSLog(@"getVideo failed!");
    }];
    [self performSelector:@selector(startVideo) withObject:nil afterDelay:60*60];
}

@end
