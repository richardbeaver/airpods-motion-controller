import SwiftUI
import CoreMotion
import Network

class AirPodsMotionSender: ObservableObject {
    private let motionManager = CMHeadphoneMotionManager()
    private var connection: NWConnection?
    private let host = "" // Replace with Mac’s local IP
    private let port: UInt16 = 9999

    @Published var streaming = false
    
    func start() {
        print("Starting motion updates…")
        print("Motion available:", motionManager.isDeviceMotionAvailable)
        print("Motion active:", motionManager.isDeviceMotionActive)
        
        motionManager.startDeviceMotionUpdates(to: .main) { motion, error in
            if let motion = motion {
                let attitude = motion.attitude
                let pitch = attitude.pitch
                let yaw = attitude.yaw
                let roll = attitude.roll
                print("AirPods motion:", pitch, yaw, roll)
                let message = "\(pitch) \(yaw) \(roll)"
                self.send(message)
            } else if let error = error {
                print("Motion error:", error)
            } else {
                print("No motion data yet")
            }
        }

        // Setup UDP connection
        let nwHost = NWEndpoint.Host(host)
        let nwPort = NWEndpoint.Port(rawValue: port)!
        connection = NWConnection(host: nwHost, port: nwPort, using: .udp)
        connection?.start(queue: .global())

        motionManager.startDeviceMotionUpdates(to: .main) { [weak self] motion, error in
            guard let motion = motion, let self = self else { return }

            let pitch = motion.attitude.pitch
            let yaw   = motion.attitude.yaw
            let roll  = motion.attitude.roll

            let message = String(format: "%.3f %.3f %.3f", pitch, yaw, roll)
            self.send(message)
        }

        streaming = true
    }

    func stop() {
        motionManager.stopDeviceMotionUpdates()
        connection?.cancel()
        connection = nil
        streaming = false
    }

    private func send(_ message: String) {
        let data = message.data(using: .utf8)!
        connection?.send(content: data, completion: .contentProcessed { error in
            if let error = error {
                print("Send error:", error)
            } else {
                print("Sent:", data)
            }
        })
    }
}
