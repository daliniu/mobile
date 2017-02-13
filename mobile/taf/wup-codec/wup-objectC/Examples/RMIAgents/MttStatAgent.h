//
//  MttStatAgent.h
//  MttHD
//
//  Created by 壬俊 易 on 12-3-21.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttSTStat.h>

@interface MttStatAgent : WupAgent

- (bool)stat:(MttSTStat *)request;
- (id)stat:(MttSTStat *)request withCompleteHandle:(void (^)(bool))handle;

@end
