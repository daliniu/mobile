//
//  MttLoginReq+Adapter.h
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WirelessUnifiedProtocol/MttLoginReq.h>

@interface MttLoginReq (adapter)

+ (id)mtthdInstance;
+ (NSData *)key;
+ (NSData *)signature;

@end