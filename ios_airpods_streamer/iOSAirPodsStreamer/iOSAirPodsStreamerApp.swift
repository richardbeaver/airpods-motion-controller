//
//  iOSAirPodsStreamerApp.swift
//  iOSAirPodsStreamer
//
//  Created by Richard Beaver on 10/16/25.
//

import SwiftUI

@main
struct iOSAirPodsStreamerApp: App {
    init() {
        Env.load()
    }

    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}
