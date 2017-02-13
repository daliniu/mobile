//
//  UniPacket.h
//
//  Created by 壬俊 易 on 11-12-1.
//  Copyright (c) 2011年 Tencent. All rights reserved.
//

#import "RequestF.h"
#import "JceObjectV2.h"
#import "UniAttribute.h"

@interface UniPacket : RequestPacket

@property (nonatomic, readonly) UniAttribute* attributes;

+ (UniPacket *)packet;

#pragma mark - deprecated

// 下面的这些接口已经废弃不用了，要设置/获取参数，建议使用UniAttribute.h中的方法
- (id)getObjectAttr:(NSString *)attrName forClass:(Class)theClass;
- (void)putDataAttr:(NSString *)attrName value:(NSData *)attrValue;
- (void)putObjectAttr:(NSString *)attrName value:(JceObjectV2 *)attrValue;
- (void)putObjectAttr:(NSString *)attrName type:(NSString *)attrType value:(JceObject *)attrValue;

@end
