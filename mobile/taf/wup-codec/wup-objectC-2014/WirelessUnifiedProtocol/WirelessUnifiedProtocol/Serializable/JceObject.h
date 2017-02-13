//
//  JceObject.h
//
//

#import "JceInputStream.h"
#import "JceOutputStream.h"
#import "JceEnumHelper.h"

#pragma mark -

typedef BOOL                    JceBool;
typedef char                    JceInt8;
typedef unsigned char           JceUInt8;
typedef short                   JceInt16;
typedef unsigned short          JceUInt16;
typedef int                     JceInt32;
typedef unsigned int            JceUInt32;
typedef long long               JceInt64;
typedef unsigned long long      JceUInt64;
typedef float                   JceFloat;
typedef double                  JceDouble;

#define AUTO_COLLECT            NSAutoreleasePool *autoReleasePool = [[NSAutoreleasePool alloc] init];
#define AUTO_RELEASE            [autoReleasePool release];

#define DefaultJceString        @""
#define DefaultJceData          [NSData data]
#define DefaultJceArray         [NSArray array]
#define DefaultJceDictionary    [NSDictionary dictionary]

#pragma mark - 

@interface JceObject : NSObject <NSCoding>

+ (id)object;
+ (NSDictionary *)jcePropertiesWithEncodedTypes;

+ (id)fromData:(NSData *)data;
- (id)fromData:(NSData *)data;
- (NSData *)toData;

+ (NSString *)jceType;
- (NSString *)jceType;

- (void)__pack:(JceOutputStream *)stream;	// !!! INTERNAL USE ONLY
- (void)__unpack:(JceInputStream *)stream;	// !!! INTERNAL USE ONLY

@end
