//
//  MttVerifyService.h
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WirelessUnifiedProtocol/WupService.h>

@interface MttVerifyService : WupService

- (id)verifyWithCompleteBlock:(WupServiceBasicBlock)completeBlock failedBlock:(WupServiceBasicBlock)failedBlock;

@end
