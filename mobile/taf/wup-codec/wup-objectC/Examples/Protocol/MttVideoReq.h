//
//  MttVideoReq.h
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>
#import <WirelessUnifiedProtocol/MttUserBase.h>

@interface MttVideoRequest : JceObjectV2

@property (nonatomic, retain, JV2_PROP_GS(stUB)) MttUserBase * JV2_PROP_NM(o, 0, stUB);
@property (nonatomic, retain, JV2_PROP_GS(sMd5)) NSString* JV2_PROP_NM(o, 1, sMd5);

@end

@interface MttVideoRequest (adapter)

+ (id)mtthdInstance;

@end