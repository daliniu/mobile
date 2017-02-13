//
//  WupHttpRequest.h
//  WirelessUnifiedProtocol
//
//  Created by 壬俊 易 on 12-4-11.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ASIHTTPRequest.h"
#import "WupAgentDelegate.h"

@interface WupHttpRequest : ASIHTTPRequest

// 当WUP请求失败时，是否更换其它WUP服务器进行重试，默认为NO
@property (assign) BOOL retryUsingOtherServers;

// 当retryUsingOtherServers为YES时，必须指定agentDelegate
@property (retain) NSObject<WupAgentDelegate> *agentDelegate;

@end
