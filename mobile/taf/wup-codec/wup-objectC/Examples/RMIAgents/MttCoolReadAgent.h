//
//  MttCoolReadAgent.h
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import "MttVideoReq.h"
#import "MttVideoApp.h"

@interface MttVideoAgent : WupAgent

- (MttVideoResponse *)getVideo:(MttVideoRequest *)request;
- (id)getVideo:(MttVideoRequest *)request withCompleteHandle:(void (^)(MttVideoResponse *))handle;

@end
