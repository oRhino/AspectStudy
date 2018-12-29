//
//  Aspects.h
//  Aspects - A delightful, simple library for aspect oriented programming.
//
//  Copyright (c) 2014 Peter Steinberger. Licensed under the MIT license.
//

/***
 setValue Forkey
 1.查看是否实现访问器方法
 -set<Key>
 如果value为nil,但key是基本类型,调用-setNilValueForKey:
 NSNumber/NSValue与基本类型可以自动转换
 2.+accessInstanceVariablesDirectly是否返回YES
 3.是否有成员变量 _<key>, _is<Key>, <key>, is<Key>
 4.-setValue:forUndefinedKey:
 5.NSUndefinedKeyException异常
 ***/

/***
 value Forkey
 1.查看是否实现访问器方法
 -get<Key>, -<key> ,-is<Key>,
 2.+accessInstanceVariablesDirectly是否返回YES
 3.是否有成员变量 _<key>, _is<Key>, <key>, or is<Key>
 4.-valueForUndefinedKey:
 5.NSUndefinedKeyException异常
 
 ***/

#import <Foundation/Foundation.h>

/// AspectPositionAfter在原方法执行完之后调
/// AspectPositionInstead是替换原方法
/// AspectPositionBefore是在原方法之前调用切片方法
/// AspectOptionAutomaticRemoval是在hook执行完自动移除。
typedef NS_OPTIONS(NSUInteger, AspectOptions) {
    AspectPositionAfter   = 0,            /// Called after the original implementation (default)
    AspectPositionInstead = 1,            /// Will replace the original implementation.
    AspectPositionBefore  = 2,            /// Called before the original implementation.
    
    AspectOptionAutomaticRemoval = 1 << 3 /// Will remove the hook after the first execution.
};

/// Opaque Aspect Token that allows to deregister the hook.
@protocol AspectToken <NSObject>

/// Deregisters an aspect.
/// @return YES if deregistration is successful, otherwise NO.
/// 撤销一个hook。remove方法返回YES代表撤销成功，返回NO就撤销失败。
- (BOOL)remove;

@end

/// The AspectInfo protocol is the first parameter of our block syntax.
@protocol AspectInfo <NSObject>

/// The instance that is currently hooked.
/// instance方法返回当前被hook的实例
- (id)instance;

/// The original invocation of the hooked method.
/// originalInvocation方法返回被hooked方法的原始的invocation
- (NSInvocation *)originalInvocation;

/// arguments方法返回所有方法的参数。它的实现是懒加载
/// All method arguments, boxed. This is lazily evaluated.
- (NSArray *)arguments;

@end

/**
 Aspects uses Objective-C message forwarding to hook into messages. This will create some overhead. Don't add aspects to methods that are called a lot. Aspects is meant for view/controller code that is not called a 1000 times per second.
 Aspects利用的OC的消息转发机制，hook消息。这样会有一些性能开销。不要把Aspects加到经常被使用的方法里面。Aspects是用来设计给view/controller 代码使用的，而不是用来hook每秒调用1000次的方法的。
 
 Adding aspects returns an opaque token which can be used to deregister again. All calls are thread safe.
 添加Aspects之后，会返回一个隐式的token，这个token会被用来注销hook方法的。所有的调用都是线程安全的。
 */
@interface NSObject (Aspects)

/// Adds a block of code before/instead/after the current `selector` for a specific class.
///
/// @param block Aspects replicates the type signature of the method being hooked.
/// The first parameter will be `id<AspectInfo>`, followed by all parameters of the method.
/// These parameters are optional and will be filled to match the block signature.
/// You can even use an empty block, or one that simple gets `id<AspectInfo>`.
///
/// @note Hooking static methods is not supported.
/// @return A token which allows to later deregister the aspect.

//+hook对类的所有对象生效，减号方法是hook只对单个对象生效
//第1个selector是要给它增加切面的原方法。
//第2个参数是AspectOptions类型，是代表这个切片增加在原方法的before / instead / after。
//第4个参数是返回的错误。
//第三个入参block。这个block复制了正在被hook的方法的签名signature ,block遵循AspectInfo协议。我们甚至可以使用一个空的block。AspectInfo协议里面的参数是可选的，主要是用来匹配block签名的。
//返回值是一个token，可以被用来注销这个Aspects。
//不支持hook 静态static方法
+ (id<AspectToken>)aspect_hookSelector:(SEL)selector
                           withOptions:(AspectOptions)options
                            usingBlock:(id)block
                                 error:(NSError **)error;
//hook实例方法
/// Adds a block of code before/instead/after the current `selector` for a specific instance.
- (id<AspectToken>)aspect_hookSelector:(SEL)selector
                           withOptions:(AspectOptions)options
                            usingBlock:(id)block
                                 error:(NSError **)error;

@end


typedef NS_ENUM(NSUInteger, AspectErrorCode) {
    AspectErrorSelectorBlacklisted,                   /// Selectors like release, retain, autorelease are blacklisted.
    AspectErrorDoesNotRespondToSelector,              /// Selector could not be found.
    AspectErrorSelectorDeallocPosition,               /// When hooking dealloc, only AspectPositionBefore is allowed.
    AspectErrorSelectorAlreadyHookedInClassHierarchy, /// Statically hooking the same method in subclasses is not allowed.
    AspectErrorFailedToAllocateClassPair,             /// The runtime failed creating a class pair.
    AspectErrorMissingBlockSignature,                 /// The block misses compile time signature info and can't be called.
    AspectErrorIncompatibleBlockSignature,            /// The block signature does not match the method or is too large.

    AspectErrorRemoveObjectAlreadyDeallocated = 100   /// (for removing) The object hooked is already deallocated.
};

extern NSString *const AspectErrorDomain;
