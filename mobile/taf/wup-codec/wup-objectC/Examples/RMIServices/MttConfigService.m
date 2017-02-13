//
//  MttConfigService.m
//  MttHD
//
//  Created by 壬俊 易 on 12-3-19.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttConfigService.h"
#import "MttConfigAgent.h"
#import "MttHDWupManager.h"
#import "MttHDWupEnvironment.h"
#import "MttPageConfigReq+Adapter.h"
#import "MttSplashReq+Adapter.h"
#import "MttHDPageConfigInfo.h"
#import "MttHDSplashInfo.h"

@implementation MttConfigService

- (id)init
{
	if (self = [super initWithAgent:[MttConfigAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)getPageConfigWithCompleteBlock:(WupServiceBasicBlock)completeBlock failedBlock:(WupServiceBasicBlock)failedBlock
{
    MttConfigAgent *agent = (MttConfigAgent *)[self agent];
    id stub = [agent getPageConfig:[MttPageConfigReq mtthdInstance] withCompleteHandle:^(MttPageConfigRsp * rsp) {
        if (rsp != nil) {
            MttHDPageConfigInfo *pageConfigInfo = [MttHDPageConfigInfo theSpecial];
            if (rsp.jce_stBrowserRsp.jce_resultCode == 0 && [rsp.jce_stBrowserRsp.jce_content length] > 0) {
                pageConfigInfo.md5StringOfStartPage = rsp.jce_stBrowserRsp.jce_lastVersionMD5;
                pageConfigInfo.startPageContent = rsp.jce_stBrowserRsp.jce_content;
                [[MttHDWupManager defaultNotificationCenter] postNotificationName:MttHDWupStartPageUpdated object:nil];
            }
            if (rsp.jce_stSearchRsp.jce_resultCode == 0) {
                pageConfigInfo.searchLastUpdateTime = rsp.jce_stSearchRsp.jce_iSearchTime;
            }
            pageConfigInfo.keywordLastUpdateTime = rsp.jce_stKeywordRsp.jce_iKeywordTime;
            pageConfigInfo.domainLastUpdateTime = rsp.jce_stDomainRsp.jce_iDomainTime;
            pageConfigInfo.innerUrls = rsp.jce_stDomainRsp.jce_vDomain;
            
            NSMutableArray *safeUrls = [NSMutableArray array];
            for (MttExtDomain *extDomain in rsp.jce_stDomainRsp.jce_vExtDomain)
                if (extDomain.jce_eType == MttDomainTypeSafeWhiteList4IPAD)
                    [safeUrls addObject:extDomain.jce_sExtDomain];
            pageConfigInfo.safeUrls = safeUrls;
            
            pageConfigInfo.pluginLastUpdateTime = rsp.jce_stPluginRsp.jce_iPluginTime;
            pageConfigInfo.configLastUpdateTime = rsp.jce_stConfRsp.jce_iConfTime;
            
            NSMutableDictionary *configInfoDictionary = [NSMutableDictionary dictionary];
            for (MttConfInfo *configInfo in rsp.jce_stConfRsp.jce_vConfInfo)
                [configInfoDictionary setValue:configInfo.jce_sValue forKey:configInfo.jce_sKey];
            pageConfigInfo.configInfoDictionary = configInfoDictionary;
            
            [pageConfigInfo save];
            if (completeBlock) 
                completeBlock();
        }
        else {
            if (failedBlock)
                failedBlock();
        }
    }];
    return stub;
}

- (id)getSplashWithCompleteBlock:(WupServiceBasicBlock)completeBlock failedBlock:(WupServiceBasicBlock)failedBlock
{
    MttConfigAgent *agent = (MttConfigAgent *)[self agent];
    id stub = [agent getSplash:[MttSplashReq mtthdInstance] withCompleteHandle:^(MttSplashRsp * rsp) {
        if (rsp != nil) {
            @try {
                UIImage *image = [UIImage imageWithData:rsp.jce_vPicData];
                if (image != nil) {
                    MttHDSplashInfo *splashInfo = [MttHDSplashInfo theSpecial];
                    splashInfo.splashImage = image;
                    splashInfo.md5StringOfSplashImage = rsp.jce_sPicMd5;
                    splashInfo.md5StringOfSplashConfig = rsp.jce_sCondMd5;
                    splashInfo.startTime = [NSDate dateWithTimeIntervalSince1970:rsp.jce_iEffectTime];
                    splashInfo.expirationTime = [NSDate dateWithTimeIntervalSince1970:rsp.jce_iInvalidTime];
                    splashInfo.duration = rsp.jce_iShowSecond;
                    [splashInfo save];
                }
                if (completeBlock) 
                    completeBlock();
            }
            @catch (NSException *exception) {
                NSLog(@"%@", exception);
            }
        }
        else {
            if (failedBlock)
                failedBlock();
        }
    }];
    return stub;
}

@end
