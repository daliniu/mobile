//
//  MttVerifyAgent.h
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttVerifyReq.h>
#import <WirelessUnifiedProtocol/MttVerifyRsp.h>

@interface MttVerifyAgent : WupAgent

- (MttVerifyRsp *)verify:(MttVerifyReq *)request;
- (id)verify:(MttVerifyReq *)request withCompleteHandle:(void (^)(MttVerifyRsp *))handle;

@end
