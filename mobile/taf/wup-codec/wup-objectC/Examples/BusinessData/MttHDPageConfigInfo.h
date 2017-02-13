//
//  MttHDPageConfigInfo.h
//  MttHD
//
//  Created by renjunyi on 12-4-14.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDPersistentObject.h"

@interface MttHDPageConfigInfo : MttHDPersistentObject

+ (MttHDPageConfigInfo *)theSpecial;

@property (retain)            NSString     *md5StringOfStartPage;
@property (nonatomic, assign) NSInteger    searchLastUpdateTime;
@property (nonatomic, assign) NSInteger    keywordLastUpdateTime;

@property (nonatomic, assign) NSInteger    domainLastUpdateTime;
@property (retain)            NSArray      *innerUrls;
@property (retain)            NSArray      *safeUrls;

@property (nonatomic, assign) NSInteger    pluginLastUpdateTime;
@property (nonatomic, assign) NSInteger    configLastUpdateTime;
@property (retain)            NSDictionary *configInfoDictionary;

+ (MttHDPageConfigInfo *)theSpecial;

- (NSString *)startPageContentFilePath;
- (void)setStartPageContent:(NSString *)startPageContent;

- (NSInteger)userNeedRateAfterDays;
- (NSInteger)userNeedRateAfterLeaveTimes;

@end