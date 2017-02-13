//
//  WupService.h
//
//

@class WupAgent;

typedef void (^WupServiceBasicBlock)(void);

@interface WupService : NSObject 

@property (readonly) WupAgent *agent;

+ (id)service;
+ (void)cancel:(id)stub;
- (id)initWithAgent:(WupAgent *)agent;

@end
