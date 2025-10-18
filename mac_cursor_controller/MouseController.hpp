#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>

class MouseController {
  int motion_scale = 100;

public:
  // Move the cursor relative to its current position
  void moveRelative(double dx, double dy) {
    // Get current cursor position
    CGEventRef event = CGEventCreate(nullptr);
    CGPoint cursor = CGEventGetLocation(event);
    CFRelease(event);

    // Apply movement
    cursor.x += dx * motion_scale;
    cursor.y -= dy * motion_scale; // invert y for screen coordinates

    // Move the cursor
    CGEventRef move = CGEventCreateMouseEvent(nullptr, kCGEventMouseMoved,
                                              cursor, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, move);
    CFRelease(move);
  }

  // Move the cursor to the center of the screen
  void moveToCenter() {
    auto mainDisplay = CGMainDisplayID();
    CGRect bounds = CGDisplayBounds(mainDisplay);
    CGPoint center = CGPointMake(CGRectGetMidX(bounds), CGRectGetMidY(bounds));

    // Create and post a mouse-move event to the center
    CGEventRef move = CGEventCreateMouseEvent(nullptr, kCGEventMouseMoved,
                                              center, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, move);
    CFRelease(move);
  }
};
