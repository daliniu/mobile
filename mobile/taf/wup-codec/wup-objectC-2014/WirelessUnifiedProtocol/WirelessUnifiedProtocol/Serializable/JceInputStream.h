//
//  JceInputStream.h
//
//

#import "JceStream.h"

@interface JceInputStream : JceStream
{
	int	_headType;
	int	_headTag;
}

@property (nonatomic, assign) int headType;
@property (nonatomic, readonly) int	headTag;

+ (JceInputStream *)streamWithBuffer:(void *)buffer length:(int)length;
+ (JceInputStream *)streamWithData:(NSData *)data;

- (BOOL)readHead;
- (BOOL)peakHead;

- (int)readInt1;
- (int)readInt2;
- (int)readInt4;
- (long long)readInt8;
- (float)readFloat;
- (double)readDouble;

- (long long)readInt:(int)tag;
- (float)readFloat:(int)tag;
- (double)readDouble:(int)tag;

- (NSNumber *)readNumber:(int)tag required:(BOOL)required;
- (NSString *)readString:(int)tag required:(BOOL)required;
- (NSData *)readData:(int)tag required:(BOOL)required;
- (id)readObject:(int)tag required:(BOOL)required description:(Class)theClass;
- (NSArray *)readArray:(int)tag required:(BOOL)required description:(id)description;
- (NSDictionary *)readDictionary:(int)tag required:(BOOL)required description:(JcePair *)description;
- (id)readAnything:(int)tag required:(BOOL)required description:(id)description;

@end
