//
//  MttHDPersistentObject.m
//  MttHD
//
//  Created by renjunyi on 12-4-14.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "MttHDPersistentObject.h"

@implementation MttHDPersistentObject

+ (id)theSpecial
{
    id object = nil;
    @synchronized (self) {
        if ((object = [self findFirstByCriteria:@""]) == nil) {
            object = [[[self alloc] init] autorelease];
            [object save];
        }
    }
    return object;
}

+ (NSString *)wupDataDirectory
{
    static NSString *directoryPath = nil;
    if (directoryPath == nil) {
        directoryPath = [[NSHomeDirectory() stringByAppendingPathComponent:@"Library/Wup/"] retain];
        [[NSFileManager defaultManager] createDirectoryAtPath:directoryPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    return directoryPath;
}

@end
