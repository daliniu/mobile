//
//  MttHDWupEnvironment.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-15.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDWupEnvironment.h"
#import <WirelessUnifiedProtocol/MttLoginRsp.h>
#import "MttHDWupManager.h"
#import "MttReleaseInfo+Adapter.h"
#import "UIDevice+Identifier.h"
#import "MttHDCCCryptEx.h"
#import "ASIHTTPRequest.h"

static NSString *_qqAccount = @"";

@implementation MttHDWupEnvironment

+ (NSString *)imei
{
    return [[UIDevice currentDevice] uniqueIdentifier];
}

+ (NSString *)qUA
{
    return [MttReleaseInfo qua];
}

+ (NSString *)lc
{
    return [MttReleaseInfo lc];
}

+ (NSString *)lcId
{
    return [MttReleaseInfo lcId];
}

+ (NSString *)channel
{
    return [MttReleaseInfo channel];
}

+ (NSString *)qAuth
{
    return [[[NSString alloc] initWithData:[MttHDLoginInfo theSpecial].auth encoding:NSUTF8StringEncoding] autorelease];
}

+ (NSString *)qGuid
{
    return [[MttHDLoginInfo theSpecial].guid hexString];
}

+ (NSString *)qqAccount
{
    return _qqAccount;
}

+ (void)setQQAccount:(NSString *)qqAccount
{
    if (_qqAccount != qqAccount) {
        [_qqAccount release];
        _qqAccount = [qqAccount retain];
    }
}

- (id)init
{
	if (self = [super init]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

@end

@implementation MttHDWupAgentHelper

+ (MttHDWupAgentHelper *)agentHelper
{
    return [self proxyAgentHelper];
}

+ (MttHDWupAgentHelper *)proxyAgentHelper
{
    return [[[self alloc] initAsProxyAH] autorelease];
}

+ (MttHDWupAgentHelper *)relayAgentHelper
{
    return [[[self alloc] initAsRelayAH] autorelease];
}

- (id)initAsProxyAH
{
	if (self = [super init]) {
        MttHDLoginInfo *loginInfo = [MttHDLoginInfo theSpecial];
        @synchronized(loginInfo) {
            _serverUrls = [[NSMutableArray alloc] initWithCapacity:[loginInfo.proxyList count]];
            for (NSString* proxyUrl in loginInfo.proxyList)
                [_serverUrls addObject:[NSURL URLWithString:proxyUrl]];
            _qAuth = [[MttHDWupEnvironment qAuth] retain];
            _qGuid = [[MttHDWupEnvironment qGuid] retain];
        }
	}
	return self;
}

- (id)initAsRelayAH
{
	if (self = [super init]) {
        MttHDLoginInfo *loginInfo = [MttHDLoginInfo theSpecial];
        @synchronized(loginInfo) {
            _serverUrls = [[NSMutableArray alloc] initWithCapacity:[loginInfo.relayList count]];
            for (NSString* relayUrl in loginInfo.relayList) {
                if (![relayUrl hasSuffix:@"wup"]) {
                    relayUrl = [NSString stringWithFormat:@"%@/wup", relayUrl];
                }
                [_serverUrls addObject:[NSURL URLWithString:relayUrl]];            
            }
//            [_serverUrls addObject:[NSURL URLWithString:@"http://117.135.129.46:8080/wup"]]; 
            _qAuth = [[MttHDWupEnvironment qAuth] retain];
            _qGuid = [[MttHDWupEnvironment qGuid] retain];
        }
	}
	return self;
}

- (void)dealloc
{
    [_serverUrls release];
    [_qAuth release];
    [_qGuid release];
    [super dealloc];
}

- (NSString *)userAgent
{
    return [ASIHTTPRequest defaultUserAgentString];
}

- (NSString *)qUA
{
    return [MttHDWupEnvironment qUA];
}

- (NSString *)qAuth
{
    return _qAuth;
}

- (NSString *)qGuid
{
    return _qGuid;
}

- (NSURL *)serverUrl
{
    @synchronized(self) {
        if ([_serverUrls count] > 0)
            return [_serverUrls objectAtIndex:0];
    }
    return nil;
}

- (void)invalidateServerUrl:(NSURL*)url
{
    @synchronized(self) {
        [_serverUrls removeObject:url];
    }
}

@end
