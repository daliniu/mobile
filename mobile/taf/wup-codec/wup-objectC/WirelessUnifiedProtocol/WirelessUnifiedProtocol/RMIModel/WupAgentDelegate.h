//
//  WupAgentDelegate.h
//  WirelessUnifiedProtocol
//
//  Created by 壬俊 易 on 12-4-11.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol WupAgentDelegate <NSObject>

// 获取服务器URL
- (NSURL*)serverUrl;

// 当使用指定的URL进行的WUP请求失败时，可以通过此接口废弃该URL，下次使用serverUrl接口将获得一
// 个服务器url列表中的新url，当没有新的url时，返回nil
- (void)invalidateServerUrl:(NSURL*)url;

- (NSString*)userAgent;
- (NSString*)qAuth;
- (NSString*)qGuid;
- (NSString*)qUA;

@end
