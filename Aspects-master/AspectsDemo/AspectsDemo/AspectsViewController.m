//
//  AspectsViewController.m
//  AspectsDemo
//
//  Created by Peter Steinberger on 05/05/14.
//  Copyright (c) 2014 PSPDFKit GmbH. All rights reserved.
//

#import "AspectsViewController.h"
#import "Aspects.h"
#import "Person.h"
#import <objc/runtime.h>
#import "ViewController.h"


@implementation AspectsViewController


- (void)viewDidLoad{
    [super viewDidLoad];
    //1.class和object_getClass的区别
    //class如果是实例对象,返回的是类对象,如果是类对象,返回自身
    //object_getClass返回的是isa的指向
    Person *p = [[Person alloc]init];
    NSLog(@"%p",p.class);
    NSLog(@"%p",object_getClass(p));
    
    NSLog(@"%p",[Person class]);
    NSLog(@"%p",object_getClass([Person class]));
    id  a = object_getClass([Person class]);
    id  b = [Person class];
    if (class_isMetaClass(a)) {
        NSLog(@"元类!");
    }else{
        NSLog(@"-----");
    }
    if (class_isMetaClass(b)) {
        NSLog(@"元类!");
    }else{
        NSLog(@"-----");
    }
    
}

- (IBAction)buttonPressed:(id)sender {
    
    UIViewController *testController = [[ViewController alloc] init];
    [self.navigationController pushViewController:testController animated:YES];

    // We are interested in being notified when the controller is being dismissed.
    [testController aspect_hookSelector:@selector(viewWillDisappear:) withOptions:2 usingBlock:^(id<AspectInfo> info, BOOL animated) {
        UIViewController *controller = [info instance];
        if (controller.isBeingDismissed || controller.isMovingFromParentViewController) {
            [[[UIAlertView alloc] initWithTitle:@"Popped" message:@"Hello from Aspects" delegate:nil cancelButtonTitle:nil otherButtonTitles:@"Ok", nil] show];
        }
    } error:NULL];

    // Hooking dealloc is delicate, only AspectPositionBefore will work here.
//    [testController aspect_hookSelector:NSSelectorFromString(@"dealloc") withOptions:AspectPositionBefore usingBlock:^(id<AspectInfo> info) {
//        NSLog(@"Controller is about to be deallocated: %@", [info instance]);
//    } error:NULL];
}

@end
