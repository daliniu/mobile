//
//  MttCoolReadService.m
//  MttHD
//
//  Created by bilsonzhou on 12-4-20.
//  Copyright (c) 2012年 tencent. All rights reserved.
//


#import "MttCoolReadService.h"
#import "MttHDWupManager.h"
#import "MttHDWupEnvironment.h"
#import "MttCoolReadAgent.h"
#import "MttHDVideoInfo.h"
#import "SQLiteInstanceManager.h"

@implementation MttVideoService

- (id)init
{
	if (self = [super initWithAgent:[MttVideoAgent agent]]) {
	}
	return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (id)getVideoWithCompleteBlock:(WupServiceBasicBlock)completeBlock failedBlock:(WupServiceBasicBlock)failedBlock
{
    MttVideoAgent *agent = (MttVideoAgent *)[self agent];
    id stub = [agent getVideo:[MttVideoRequest mtthdInstance] withCompleteHandle:^(MttVideoResponse * rsp) {
        if (rsp != nil) {
            @try {
               @synchronized([SQLiteInstanceManager sharedManager])
                {
                    MttHDVideoInfo* videoInfo = [MttHDVideoInfo theSpecial];
                    //videoInfo.sAppTitle = rsp.jce_sAppTitle;
                    videoInfo.sSearchUrl = rsp.jce_sSearchUrl;
                    videoInfo.sMd5 = rsp.jce_sMd5;
                    videoInfo.iVideoNum = rsp.jce_iVideoNum;
                    [videoInfo save];
                }

                //保存频道数据信息
                NSArray * vChannel = rsp.jce_vData;
                NSMutableArray *vNavigationList = [[NSMutableArray alloc] init];
                for (int i = 0; i < vChannel.count; ++i)
                {
                    VideoNavigationData *channel = [[VideoNavigationData alloc] init];
                    MttVideoChannel *videoChannel = [vChannel objectAtIndex:i];
                    channel.typeTitle = videoChannel.jce_stChannel.jce_sName;
                    channel.typeFileName = [videoChannel.jce_stChannel.jce_sCid stringByAppendingString:@".plist"];
                    [vNavigationList addObject:channel];
                    [channel release];
                
                    //保存对应频道的视频信息
                    NSMutableArray *videoArray = [[NSMutableArray alloc] init];
                    NSArray *vVideoArray = videoChannel.jce_vVideo;
                    for (int j = 0; j < vVideoArray.count; ++j)
                    {
                        MttIpadViewData *ipadVideoData = [vVideoArray objectAtIndex:j];
                         VideoSection *section = [[VideoSection alloc] init];
                        section.rowId = j;
                        section.imageUrl = ipadVideoData.jce_sWebCoverUrl;
                        section.abstract = ipadVideoData.jce_sWebSubTitle;
                        section.episodeInfro = ipadVideoData.jce_sWebEpsInfo;
                        section.videoTitle = ipadVideoData.jce_sWebLinkTitle;
                        section.url = ipadVideoData.jce_sWebLink;
                        section.episodeNodes = nil;
                        NSMutableArray *tmpEpisodeNodes = [[NSMutableArray alloc] init];
                        section.episodeNodes = tmpEpisodeNodes;
                        [tmpEpisodeNodes release];
                        for (int k = 0; k < ipadVideoData.jce_vEpisodeList.count; ++k)
                        {
                            MttEpisodeNode *mttNode = [ipadVideoData.jce_vEpisodeList objectAtIndex:k];
                            EpisodeNode *node = [[EpisodeNode alloc] init];
                            node.episodeNum = k;
                            node.episodeUrl = mttNode.jce_sEpisodeUrl;
                            [section.episodeNodes addObject:node];
                            [node release];
                        }
                        [videoArray addObject:section];
                        [section release];
                    }
                    [MttHDVideoInfo saveVideoData:videoArray toFile:[videoChannel.jce_stChannel.jce_sCid stringByAppendingString:@".plist"] withUpdatePrefix:YES];
                    [videoArray release];
                }
                [MttHDVideoInfo saveNavigationChannelData:vNavigationList];
                [vNavigationList release];

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

