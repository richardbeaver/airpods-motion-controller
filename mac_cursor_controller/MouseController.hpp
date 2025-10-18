#pragma once
#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>

class MouseController {
  int motion_scale = 100;

public:
  void moveCursor(float dx, float dy) {
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
};
