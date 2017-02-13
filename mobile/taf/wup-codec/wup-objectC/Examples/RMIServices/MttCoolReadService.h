//
//  MttCoolReadService.h
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WirelessUnifiedProtocol/WupService.h>
#import "MttVideoReq.h"
#import "MttVideoApp.h"

@interface MttVideoService : WupService

- (id)getVideoWithCompleteBlock:(WupServiceBasicBlock)completeBlock failedBlock:(WupServiceBasicBlock)failedBlock;

@end
