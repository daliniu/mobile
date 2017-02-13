//
//  MttHDLoginInfo.h
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDPersistentObject.h"

@interface MttHDLoginInfo : MttHDPersistentObject

@property (retain) NSData         *guid;
@property (retain) NSData         *auth;
@property (retain) NSMutableArray *proxyList;
@property (retain) NSMutableArray *relayList;
@property (assign) NSInteger      loginTime;
@property (assign) NSInteger      statValue;

+ (MttHDLoginInfo *)theSpecial;
- (void)updateRelayList:(NSArray *)list;
- (void)updateProxyList:(NSArray *)list;

@end
