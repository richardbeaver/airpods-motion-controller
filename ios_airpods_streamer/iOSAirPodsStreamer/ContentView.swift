import SwiftUI

struct ContentView: View {
    @StateObject private var sender = AirPodsMotionSender()

    var body: some View {
        VStack(spacing: 40) {
            Text(sender.streaming ? "Streaming AirPods Motion" : "Idle")
                .font(.title)
                .padding()

            Button(sender.streaming ? "Stop" : "Start") {
                if sender.streaming {
                    sender.stop()
                } else {
                    sender.start()
                }
            }
            .font(.title2)
            .padding()
            .background(sender.streaming ? Color.red : Color.green)
            .foregroundColor(.white)
            .cornerRadius(12)
        }
        .padding()
    }
}
