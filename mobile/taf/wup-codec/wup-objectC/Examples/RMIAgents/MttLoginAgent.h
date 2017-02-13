//
//  MttLoginAgent.h
//
//  Created by 壬俊 易 on 12-3-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttLoginReq.h>
#import <WirelessUnifiedProtocol/MttLoginRsp.h>

@interface MttLoginAgent : WupAgent

- (MttLoginRsp *)login:(MttLoginReq *)request;
- (id)login:(MttLoginReq *)request withCompleteHandle:(void (^)(MttLoginRsp *))handle;

- (MttUpdateRsp *)update:(MttUserBase *)request;
- (id)update:(MttUserBase *)request withCompleteHandle:(void (^)(MttUpdateRsp *))handle;

@end
