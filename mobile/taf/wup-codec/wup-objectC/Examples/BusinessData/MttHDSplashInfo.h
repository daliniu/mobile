//
//  MttHDSplashInfo.h
//  MttHD
//
//  Created by renjunyi on 12-4-14.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDPersistentObject.h"

@interface MttHDSplashInfo : MttHDPersistentObject

@property (retain) UIImage   *splashImage;
@property (retain) NSString  *md5StringOfSplashImage;
@property (retain) NSString  *md5StringOfSplashConfig;
@property (retain) NSDate    *startTime;
@property (retain) NSDate    *expirationTime;
@property (assign) NSInteger duration;

- (UIImage *)defaultSplashImage;

@end
