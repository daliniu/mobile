//
//  MttSecurityService.h
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WirelessUnifiedProtocol/WupService.h>
#import "MttBrokerSecurityRequest.h"
#import "MttBrokerSecurityResponse.h"

@interface MttSecurityService : WupService

- (id)security:(NSString *)url checkType:(int)type WithCompleteBlock:(void (^)(MttBrokerSecurityResponse *))completeBlock failedBlock:(WupServiceBasicBlock)failedBlock;

@end
