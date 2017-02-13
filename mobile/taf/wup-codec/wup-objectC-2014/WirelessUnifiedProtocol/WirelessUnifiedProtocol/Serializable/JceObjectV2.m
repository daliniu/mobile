//
//  JceObjectV2.m
//
//  Created by 壬俊 易 on 12-3-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "JceObjectV2.h"
#import "JceStream.h"

#pragma mark - JcePair (V2)

@interface JcePair (V2)

+ (id)analyzeExtStr:(NSString *)str;
+ (JcePair *)pairFromExtStr:(NSString *)str;

@end

@implementation JcePair (V2)

+ (id)analyzeExtStr:(NSString *)str
{
    unichar flag = [str characterAtIndex:0];
    switch (flag) {
        id key = nil, value = nil;
        unichar l = 0;
        case 'V':
            value = [JcePair analyzeExtStr:[str substringFromIndex:1]];
            return [JcePair pairWithValue:value forKey:nil];
        case 'M':
            l = [str substringWithRange:NSMakeRange(1, 2)].intValue;
            key = [JcePair analyzeExtStr:[str substringWithRange:NSMakeRange(3, l)]];
            value = [JcePair analyzeExtStr:[str substringFromIndex:(3 + l)]];
            return [JcePair pairWithValue:value forKey:key];
        case 'O':
            return NSClassFromString([str substringFromIndex:1]);
        default:
            assert(0);
            return nil;
    }
}

+ (JcePair *)pairFromExtStr:(NSString *)str
{
    assert([str length] < 128);
    id pair = [self analyzeExtStr:str];
    assert([pair isKindOfClass:[JcePair class]]);
    return pair;
}

@end

#pragma mark - JcePropertyInfo

@interface JcePropertyInfo : NSObject

@property (nonatomic, assign) NSInteger tag;    // index
@property (nonatomic, assign) BOOL flag;        // required
@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSString *type;
@property (nonatomic, retain) JcePair *ext;     // for vector & map

+ (id)propertyInfo;
- (NSComparisonResult)compareWithTag:(JcePropertyInfo *)obj;

@end

@implementation JcePropertyInfo

@synthesize tag;
@synthesize flag;
@synthesize name;
@synthesize type;
@synthesize ext;

+ (id)propertyInfo
{
    return [[[self alloc] init] autorelease];
}

- (id)init
{
    if (self = [super init]) {
    }
    return self;
}

- (void)dealloc 
{
    self.name = nil;
    self.type = nil;
    self.ext = nil;
    [super dealloc];
}

- (NSComparisonResult)compareWithTag:(JcePropertyInfo *)obj
{
    if (self.tag < obj.tag)
        return NSOrderedAscending;
    else if (self.tag == obj.tag)
        return NSOrderedSame;
    return NSOrderedDescending;
}

- (NSString *)description
{
    return [NSString stringWithFormat:
            @"{\n"
            @"    tag      : %d\n"
            @"    required : %d\n"
            @"    name     : %@\n"
            @"    type     : %@\n"
            @"    ext      : %@\n"
            @"}\n", self.tag, self.flag, self.name, self.type, self.ext];
}

@end

#pragma mark - JceObjectV2

static NSMutableDictionary *jcev2_class_props_descriptions;

@interface JceObjectV2 ()

- (NSDictionary *)jcePropsDescription;

@end

@implementation JceObjectV2

+ (NSString *)jceType
{
    return nil;
}

- (NSString *)jceType
{
    return [[self class] jceType];
}

- (NSDictionary *)jcePropsDescription
{
    NSMutableDictionary *propsDescription = nil;
    @synchronized(self) {
        if (jcev2_class_props_descriptions == nil)
            jcev2_class_props_descriptions = [[[NSMutableDictionary alloc] init] retain];
        assert(jcev2_class_props_descriptions != nil);
        
        NSString* className = NSStringFromClass([self class]);
        if ((propsDescription = [jcev2_class_props_descriptions objectForKey:className]) == nil) 
        {
            NSDictionary *props = [[self class] jcePropertiesWithEncodedTypes];
            propsDescription = [NSMutableDictionary dictionaryWithCapacity:[props count]];
            [jcev2_class_props_descriptions setObject:propsDescription forKey:className];
            for (NSString *propName in props) 
            {
                if ([propName hasPrefix:JV2_PROP_LFX_STR]) {
                    NSArray *strs = [propName componentsSeparatedByString:@"_"];
                    assert([strs count] >= 5 && [strs count] <= 8);
                    JcePropertyInfo *propInfo = [JcePropertyInfo propertyInfo];
                    propInfo.tag = [[strs objectAtIndex:2] integerValue];
                    propInfo.flag = [[strs objectAtIndex:3] isEqualToString:@"r"];
                    propInfo.name = [NSString stringWithFormat:@"%@%@", JV2_PROP_NFX_STR, [strs objectAtIndex:4]];
                    propInfo.type = [props objectForKey:propName];
                    propInfo.ext = ([strs count] == 8 ? [JcePair pairFromExtStr:[strs objectAtIndex:7]] : nil);
                    [propsDescription setObject:propInfo forKey:[NSNumber numberWithInt:propInfo.tag]];
                }
            }
        }
        return propsDescription;
    }
}

- (void)__pack:(JceOutputStream *)stream
{
	AUTO_COLLECT
    
    NSDictionary *description = [self jcePropsDescription];
    assert(description != nil);
    
    NSArray *keys = [description keysSortedByValueUsingSelector:@selector(compareWithTag:)];
    for (NSNumber* key in keys) 
    {
        JcePropertyInfo *propInfo = [description objectForKey:key];
        
        // 关于类型编码，请参看《Object-C Runtime Programming Guide》的“Type Encodings”部分
        id theProperty = [self valueForKey:propInfo.name];
        switch ([propInfo.type characterAtIndex:0]) 
        {
            case 'B':   // bool
            case 'c':   // char
            case 'C':   // unsigned char
            case 's':   // short
            case 'S':   // unsigned short
            case 'i':   // int
            case 'I':   // unsigned int
            case 'l':   // long
            case 'L':   // unsigned long
            case 'q':   // long long
                if (propInfo.flag != NO || theProperty != 0)
                    [stream writeInt:[theProperty longLongValue] tag:propInfo.tag];
                break;
            case 'f':   // float
                if (propInfo.flag != NO || theProperty != 0)
                    [stream writeFloat:[theProperty floatValue] tag:propInfo.tag];
                break;
            case 'd':   // double
                if (propInfo.flag != NO || theProperty != 0)
                    [stream writeDouble:[theProperty doubleValue] tag:propInfo.tag];
                break;
            case '@':   // objects
                if (propInfo.flag != NO || theProperty != nil)
                    [stream writeAnything:theProperty tag:propInfo.tag required:propInfo.flag];
                break;
            default:
                assert(0);
                break;
        }
    }
    
	AUTO_RELEASE
}

- (void)__unpack:(JceInputStream *)stream
{
	AUTO_COLLECT
    
    NSDictionary *description = [self jcePropsDescription];
    assert(description != nil);
    
    NSArray *keys = [description keysSortedByValueUsingSelector:@selector(compareWithTag:)];
    for (NSNumber* key in keys) 
    {
        JcePropertyInfo *propInfo = [description objectForKey:key];
        
        // 关于类型编码，请参看《Object-C Runtime Programming Guide》的“Type Encodings”部分
        switch ([propInfo.type characterAtIndex:0]) 
        {
            case 'B':   // bool
            case 'c':   // char
            case 'C':   // unsigned char
            case 's':   // short
            case 'S':   // unsigned short
            case 'i':   // int
            case 'I':   // unsigned int
            case 'l':   // long
            case 'L':   // unsigned long
            case 'q':   // long long
            case 'f':   // float
            case 'd':   // double
            {
                NSNumber *value = [stream readNumber:propInfo.tag required:propInfo.flag];
                if (value != nil)
                    [self setValue:value forKey:propInfo.name];
                break;
            }
            case '@':   // objects
            {
                Class cls = NSClassFromString([propInfo.type substringWithRange:NSMakeRange(2, [propInfo.type length] - 3)]);
                id value = [stream readAnything:propInfo.tag required:propInfo.flag description:(propInfo.ext != nil ? propInfo.ext : cls)];
                if (value != nil) {
                    assert([[value class] isSubclassOfClass:cls]);
                    [self setValue:value forKey:propInfo.name];
                }
                break;
            }
            default:
                assert(0);
                break;
        }
    }
    
	AUTO_RELEASE
}


@end
