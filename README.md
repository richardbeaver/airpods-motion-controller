# AirPods Motion Controller

Uses AirPods’ head motion to control Mac’s mouse cursor in real time.
This project demonstrates low-latency motion streaming, real-time signal
smoothing, and system-level mouse control between iOS and macOS devices.

---

## Overview

AirPods Motion Controller is a two-part system:

1. iOS App (Swift) - `ios_airpods_streamer`
   - Captures live head-tracking data from the AirPods’ motion sensors
     using `CMHeadphoneMotionManager`.
   - Streams pitch and yaw data over UDP to a server running on Mac.

2. macOS App (C++) - `mac_cursor_controller`
   - Runs a lightweight UDP server that receives motion packets from the iOS device.
   - Manages multiple threads to handle data parsing, state updates, and user
     input.
   - Uses atomics, condition variables, and mutexes for efficient thread
     synchronization.
     - No polling or sleeps - system reacts as data becomes available.
   - Smooths incoming data using an exponential moving average and adaptive
     responsiveness based on head speed (`MotionSmoother`).
   - Translates the filtered orientation data into real mouse movements using
     the macOS `CGEvent` API.
   - Enables user re-calibration for a natural cursor experience.

---

## How It Works

### 1. iOS Motion Capture

The iOS app listens to head rotation updates from the AirPods.
Each update includes `pitch`, `yaw`, and `roll` angles in radians.

- [AirPodsMotionSender](./ios_airpods_streamer/iOSAirPodsStreamer/AirPodsMotionSender.swift)

### 2. UDP Data Streaming

A `NWConnection` sends the formatted motion data to Mac’s IP address and port
(defined in .env - see .env.example for used environment variables):

- [AirPodsMotionSender](./ios_airpods_streamer/iOSAirPodsStreamer/AirPodsMotionSender.swift)

### 3. macOS Motion Processing

The C++ server listens for packets and extracts the latest pitch and yaw values.
It applies smoothing with an exponential filter to reduce jitter:

- [MotionControlApp](./mac_cursor_controller/App.hpp)
- [Server](./mac_cursor_controller/Server.hpp)
- [MotionSmoother](./mac_cursor_controller/MotionSmoother.hpp)
- [MotionProcessor](./mac_cursor_controller/MotionProcessor.hpp)

### 4. Cursor Control

The `MouseController` uses `CoreGraphics` events to move the macOS cursor.
In addition, pressing Enter in the Mac app recalibrates the neutral position
so the cursor re-centers.

- [MouseController](./mac_cursor_controller/MouseController.hpp)

---

## Running

### 1. Setup

- Clone the repository.
- Create a .env file in the project root:

```sh
MOTION_SERVER_HOST=192.168.x.x # your Mac’s IP on the same Wi-Fi
MOTION_SERVER_PORT=9999
```

### 2. iOS App

- Open the Xcode project: `./ios_airpods_streamer/iOSAirPodsStreamer.xcodeproj/`
- Build and Run in Xcode on a real iPhone with AirPods connected.

### 3. macOS App

- Open the Xcode project: `./mac_cursor_controller/mac_cursor_controller.xcodeproj/`
- Build the target in Xcode.
- Run the built executable in a terminal window from the project root so
  that `.env` loads correctly:
  - You can find the compiled binary under your Xcode build directory
    (e.g. `DerivedData/.../Build/Products/Debug/`), or set the project to build
    directly inside the repository.
- If prompted, grant accessibility permissions

- Once running, press **Start** on the iOS app and move your head to control the
  Mac cursor.

## Calibrating

- Keep your head facing forward at your desired neutral position and press Enter.
  - This resets the neutral orientation so that the cursor centers correctly.

## Latency Measurements

- Launch the macOS server with the `--timing` flag to enable latency output
- When enabled, the server prints two latency metrics:
  - Server latency — time from the iOS device’s motion measurement to when the
    packet is parsed by the server running on the Mac.
  - End-to-end latency — time from the iOS motion measurement to the completion
    of cursor movement on macOS.
