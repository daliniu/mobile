//
//  WupHttpRequest.m
//  WirelessUnifiedProtocol
//
//  Created by 壬俊 易 on 12-4-11.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#import "WupHttpRequest.h"
#import "WupAgentDelegate.h"

@interface WupHttpRequest ()

@property (retain) WupHttpRequest *retryRequest;
@property (assign) WupHttpRequest *retryRequestRef;
@property (retain) WupHttpRequest *parentRequest;

@end

@implementation WupHttpRequest

@synthesize retryRequest = _retryRequest;
@synthesize retryRequestRef = _retryRequestRef;
@synthesize parentRequest = _parentRequest;
@synthesize retryUsingOtherServers = _retryUsingOtherServers;
@synthesize agentDelegate = _agentDelegate;

- (id)initWithURL:(NSURL *)newURL
{
    if (self = [super initWithURL:newURL]) {
        self.retryUsingOtherServers = NO;
        self.agentDelegate = nil;
	}
	return self;
}

- (void)dealloc
{
    self.retryRequest = nil;
    self.parentRequest = nil;
    self.agentDelegate = nil;
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{    
    WupHttpRequest* clone = [super copyWithZone:zone]; 
    clone.retryUsingOtherServers = self.retryUsingOtherServers;
    clone.agentDelegate = self.agentDelegate;
    if (clone->failureBlock == nil && self->failureBlock != nil)
        clone->failureBlock = [self->failureBlock copy];
    if (clone->completionBlock == nil && self->completionBlock != nil)
        clone->completionBlock = [self->completionBlock copy];
    return clone;
}

- (void)start
{
    if (self.retryUsingOtherServers != NO && self.retryRequest == nil) {
        self.retryRequest = [[self copy] autorelease];
        self.retryRequestRef = self.retryRequest;
    }
    [super start];
}

- (void)startSynchronous
{
    if (self.retryUsingOtherServers != NO && self.retryRequest == nil) {
        self.retryRequest = [[self copy] autorelease];
        self.retryRequestRef = self.retryRequest;
    }
    [super startSynchronous];
}

- (void)startAsynchronous
{
    if (self.retryUsingOtherServers != NO && self.retryRequest == nil) {
        self.retryRequest = [[self copy] autorelease];
        self.retryRequestRef = self.retryRequest;
    }
    [super startAsynchronous];
}

- (void)cancel
{
    [self.retryRequestRef cancel];
    [super cancel];
}

- (void)clearDelegatesAndCancel
{
    [self.retryRequestRef clearDelegatesAndCancel];
    [super clearDelegatesAndCancel];
}

- (NSData *)responseData
{
    if ([self.retryRequestRef retryRequestRef] != nil) {
        [self.retryRequestRef responseData];
    }
    return [super responseData];
}

- (void)failWithError:(NSError *)theError
{
    if (self.retryUsingOtherServers == YES && theError.code != ASIRequestCancelledErrorType) 
    {
        assert(self.retryRequest != nil);
        assert(self.agentDelegate != nil);
        
        [self.agentDelegate invalidateServerUrl:self.originalURL];
        if ([self.agentDelegate serverUrl] != nil) {
            NSLog(@"%@: rmi retry with url(%@)!", [self.userInfo objectForKey:@"funcName"], [self.agentDelegate serverUrl]);
            [self.retryRequest setURL:[self.agentDelegate serverUrl]];
            [self.retryRequest startAsynchronous];
            [self setFailedBlock:nil];
            [self setCompletionBlock:nil];
            self.retryRequest.parentRequest = self;
            self.retryRequest = nil;
        }
    }
    
    [super failWithError:theError];
}

@end
