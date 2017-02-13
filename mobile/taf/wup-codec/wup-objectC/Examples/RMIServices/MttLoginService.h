//
//  MttLoginService.h
//
//  Created by 壬俊 易 on 12-3-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WirelessUnifiedProtocol/WupService.h>
#import <WirelessUnifiedProtocol/MttUpdateRsp.h>

@interface MttLoginService : WupService

- (id)loginWithCompleteBlock:(WupServiceBasicBlock)completeBlock failedBlock:(WupServiceBasicBlock)failedBlock;
- (id)updateWithCompleteBlock:(void (^)(MttUpdateRsp *))completeBlock failedBlock:(WupServiceBasicBlock)failedBlock;

@end
