//
//  MttSTStat+Adapter.h
//  MttHD
//
//  Created by 壬俊 易 on 12-4-10.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/MttSTStat.h>

@interface MttSTTime (Adapter)

+ (id)mtthdInstance;

@end

@interface MttSTStat (Adapter)

+ (id)mtthdInstance;
+ (void)mtthdInstanceReset;

@end
