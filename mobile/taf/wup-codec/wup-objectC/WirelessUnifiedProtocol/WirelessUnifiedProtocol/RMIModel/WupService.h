//
//  WupService.h
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

@class WupAgent;

typedef void (^WupServiceBasicBlock)(void);

@interface WupService : NSObject 

@property (readonly) WupAgent *agent;

+ (id)service;
+ (void)cancel:(id)stub;
- (id)initWithAgent:(WupAgent *)agent;

@end
