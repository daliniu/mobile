//
//  WupAgent.h
//
//

#import "WupAgentDelegate.h"

@class UniPacket;

/**
 * 远程方法调用代理类WupAgent，提供对基于WUP协议封装的远程方法调用的支持
 * 对于远程方法调用，需要指定servantName和funcName，调用参数和返回值通过uniPacket传递
 */
@interface WupAgent : NSObject

@property (nonatomic, readonly) NSString *servantName;
@property (nonatomic, retain) NSObject<WupAgentDelegate> *delegate;
@property (nonatomic, assign) BOOL retryUsingOtherServers;

+ (id)agent;
- (id)initWithServant:(NSString *)name;

// 同步调用远程方法
- (UniPacket *)invocation:(NSString *)funcName parameter:(UniPacket *)uniPacket;

// 异步调用远程方法，handle将在一个"a well-known global concurrent queue"中被调用
- (id)invocation:(NSString *)funcName parameter:(UniPacket *)uniPacket completeHandle:(void (^)(UniPacket *))handle;

// 取消远程方法调用
+ (void)cancel:(id)stub;

@end
