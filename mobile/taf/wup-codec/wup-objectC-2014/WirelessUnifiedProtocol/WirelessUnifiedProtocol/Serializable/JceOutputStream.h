//
//  JceOutputStream.h
//
//

#import "JceStream.h"

@interface JceOutputStream : JceStream

+ (JceOutputStream *)stream;
+ (JceOutputStream *)streamWithCapability:(int)capability;

- (void)writeTag:(int)tag type:(int)type;
- (void)writeInt1:(char)val;
- (void)writeInt2:(short)val;
- (void)writeInt4:(int)val;
- (void)writeInt8:(long long)val;
- (void)writeInt:(long long)val tag:(int)tag;
- (void)writeFloat:(float)val tag:(int)tag;
- (void)writeDouble:(double)val tag:(int)tag;

- (void)writeDictionary:(NSDictionary *)dictionary tag:(int)tag required:(BOOL)required;
- (void)writeArray:(NSArray *)array tag:(int)tag required:(BOOL)required;
- (void)writeNumber:(NSNumber *)number tag:(int)tag required:(BOOL)required;
- (void)writeObject:(JceObject *)object tag:(int)tag required:(BOOL)required;
- (void)writeString:(NSString *)string tag:(int)tag required:(BOOL)required;
- (void)writeData:(NSData *)data tag:(int)tag required:(BOOL)required;
- (void)writeAnything:(id)anything tag:(int)tag required:(BOOL)required;

@end
