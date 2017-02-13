//
//  WirelessUnifiedProtocol-Debug.h
//  WirelessUnifiedProtocol
//
//  Created by 壬俊 易 on 12-6-13.
//  Copyright (c) 2012年 Tencent. All rights reserved.
//

#ifndef WirelessUnifiedProtocol_WirelessUnifiedProtocol_Debug_h
#define WirelessUnifiedProtocol_WirelessUnifiedProtocol_Debug_h

#define ASSERT_FAIL_TRHOW_JCE_SERIALIZABLE_EXCEPTION(x)                         \
do {                                                                            \
    if (!(x)) {                                                                 \
        NSString *reason = [NSString stringWithFormat:@"%s(%d) %s: assert(%@) fail!", __FILE__, __func__, __LINE__, @#x] userInfo:nil]; \
        NSException *exception = [NSException exceptionWithName:@"JceSerializableException" reason:reason]; \
        @throw exception;                                                       \
    }                                                                           \
} while(0)

#endif
