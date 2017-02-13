//
//  MttBrokerSecurityResponse.h
//  MttHD
//
//  Created by 壬俊 易 on 12-4-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <WirelessUnifiedProtocol/WirelessUnifiedProtocol.h>

@interface MttBrokerSecurityResponse : JceObjectV2

@property (nonatomic, assign, JV2_PROP_GS(iSecurityLevel)) int JV2_PROP_NM(o, 0, iSecurityLevel);               // 安全级别: 0-安全 1-高风险 2-普通风险 3-低风险
@property (nonatomic, retain, JV2_PROP_GS(strSecurityType)) NSString* JV2_PROP_NM(o, 1, strSecurityType);       // 安全信息类型描述, 如“恶意广告类风险网站”
@property (nonatomic, retain, JV2_PROP_GS(vDetailDescription)) NSData* JV2_PROP_NM(o, 2, vDetailDescription);   // 网址检测时为风险信息详细页面，下载资源检测时为软件分析检测信息

@end
