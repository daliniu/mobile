//
//  MttHDVerifyInfo.h
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDPersistentObject.h"

@interface MttHDVerifyInfo : MttHDPersistentObject

@property (assign) NSInteger verifyId;
@property (retain) NSString  *parameter;

+ (MttHDVerifyInfo *)theSpecial;

@end
