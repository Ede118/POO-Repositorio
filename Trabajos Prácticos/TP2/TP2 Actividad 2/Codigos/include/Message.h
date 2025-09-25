#ifndef MESSAGE_H
#define MESSAGE_H

#include "Envelope.h"
#include "json.hpp"

#include <stdexcept>
#include <utility>

using Json = nlohmann::json;

class Message {
    Envelope meta_; //metadata
    Json payload_;

    public:
      Message (Envelope meta, Json payload = Json::object());
      
      const Envelope& meta() const noexcept;
      Envelope& meta() noexcept;
      const Json& payload() const noexcept;
      void setPayload(Json p);

    template<typename T>
    void put(const std::string& key, T&& value);

    bool has(const std::string& key) const;
    
    template<typename T>
    T get(const std::string& key) const;
};

#endif // MESSAGE
