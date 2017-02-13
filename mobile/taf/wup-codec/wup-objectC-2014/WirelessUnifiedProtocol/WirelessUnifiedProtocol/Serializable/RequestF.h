//
//  RequestF.h
//
//

#import "JceObject.h"

#pragma mark -
#pragma mark STRUCT

#pragma mark -

@interface RequestPacket : JceObject

@property (nonatomic, retain) NSNumber     *iVersion;           // short
@property (nonatomic, retain) NSNumber     *cPacketType;        // char
@property (nonatomic, retain) NSNumber     *iMessageType;       // int
@property (nonatomic, retain) NSNumber     *iRequestId;         // int
@property (nonatomic, retain) NSString     *sServantName;
@property (nonatomic, retain) NSString     *sFuncName;
@property (nonatomic, retain) NSData       *sBuffer;
@property (nonatomic, retain) NSNumber     *iTimeout;           // int
@property (nonatomic, retain) NSDictionary *context;
@property (nonatomic, retain) NSDictionary *status;

@end
