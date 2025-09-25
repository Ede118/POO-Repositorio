#ifndef ENVELOPE_H
#define ENVELOPE_H

#include <string>
#include <optional>
#include <chrono>

using namespace std;

enum class OpCode : int { Unkown = 0, Ok = 1, Error = 2};

class Envelope {
    
    OpCode opCode_{OpCode::Unkown};
    optional<string> userId_;
    optional<string> deviceId_;
    std::chrono::system_clock::time_point timestamp_{std::chrono::system_clock::now()};
    string correlationId_{};

    public:
        // Constructor
        Envelope(OpCode op,
                optional<string> userId = nullopt,
                optional<string> deviceId = nullopt,
                string correlationId = "");

        // Observadores
        OpCode opCode() const noexcept;
        const optional<string>& userId() const noexcept;
        const optional<string>& deviceId() const noexcept;
        std::chrono::system_clock::time_point timestamp() const noexcept;
        const string& correlationId() const noexcept;

        bool isValid(bool requireIds) const;

        void setOpCode(OpCode op);
        void setUserAndDevice(string uid, string did);

        
};

#endif
