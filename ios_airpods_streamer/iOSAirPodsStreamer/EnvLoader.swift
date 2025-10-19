import Foundation

enum Env {
    static func load() {
        guard let path = Bundle.main.path(forResource: ".env", ofType: nil),
              let contents = try? String(contentsOfFile: path, encoding: .utf8) else {
            print("[Env] .env file not found in bundle.")
            return
        }

        for line in contents.split(whereSeparator: \.isNewline) {
            let trimmed = line.trimmingCharacters(in: .whitespaces)
            guard !trimmed.isEmpty, !trimmed.hasPrefix("#") else { continue }

            let parts = trimmed.split(separator: "=", maxSplits: 1).map(String.init)
            guard parts.count == 2 else { continue }

            let key = parts[0].trimmingCharacters(in: .whitespaces)
            let value = parts[1].trimmingCharacters(in: .whitespaces)
            setenv(key, value, 1)
            print("[Env] Loaded \(key)=\(value)")
        }
    }

    static func value(for key: String) -> String? {
        guard let raw = getenv(key) else { return nil }
        return String(cString: raw)
    }
}
