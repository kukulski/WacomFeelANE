//
//  EventTap.c
//  WacomANE
//
//  Created by Timothy Kukulski on 12/15/12.
//  Copyright (c) 2012 Nick Kwiatkowski. All rights reserved.
//

#include <stdio.h>


// Start watching events to figure out when to close the window
NSAssert(_eventMonitor == nil, @"_eventMonitor should not be created yet");
_eventMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:
                 (NSLeftMouseDownMask | NSRightMouseDownMask | NSOtherMouseDownMask | NSKeyDownMask)
                                                      handler:^(NSEvent *incomingEvent) {
                 NSEvent *result = incomingEvent;
                 NSWindow *targetWindowForEvent = [incomingEvent window];
                 if (targetWindowForEvent != _window) {
                 [self _closeAndSendAction:NO];
                 } else if ([incomingEvent type] == NSKeyDown) {
                 if ([incomingEvent keyCode] == 53) {
                 // Escape
                 [self _closeAndSendAction:NO];
                 result = nil; // Don't process the event
                 } else if ([incomingEvent keyCode] == 36) {
                 // Enter
                 [self _closeAndSendAction:YES];
                 result = nil;
                 }
                 }
                 return result;
                 }];