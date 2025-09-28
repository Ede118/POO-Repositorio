#pragma once

#include "Nodo.h"
#include "Types.h"

#include <string>
#include <utility>
#include <vector>

class NodoID : public Nodo {
public:
    NodoID(std::string id, std::string device, std::vector<std::string> cols4);

    const std::string& id() const noexcept;
    const std::string& device() const noexcept;
    const std::vector<std::string>& cols() const noexcept;

private:
    std::string id_;
    std::string device_;
    std::vector<std::string> cols4_;
};

inline NodoID::NodoID(std::string id, std::string device, std::vector<std::string> cols4)
    : id_(std::move(id)), device_(std::move(device)), cols4_(std::move(cols4)) {
    if (cols4_.size() != 4) {
        throw AppError("NodoID requiere exactamente 4 columnas de datos");
    }
}

inline const std::string& NodoID::id() const noexcept { return id_; }
inline const std::string& NodoID::device() const noexcept { return device_; }
inline const std::vector<std::string>& NodoID::cols() const noexcept { return cols4_; }
