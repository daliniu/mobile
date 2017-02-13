//
//  MttHDPersistentObject.h
//  MttHD
//
//  Created by renjunyi on 12-4-14.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "SQLitePersistentObject.h"

@interface MttHDPersistentObject : SQLitePersistentObject

+ (id)theSpecial;
+ (NSString *)wupDataDirectory;

@end
