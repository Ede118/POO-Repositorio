#include "Verificador.h"

#include "Types.h"

#include <stdexcept>

namespace {
void validatePerms(const std::string& perms) {
    for (char perm : perms) {
        if (perm != 'r' && perm != 'w' && perm != 'a') {
            throw AppError("Permiso inválido (use r, w o a)");
        }
    }
}
}

void Verificador::allow(const std::string& id, const std::string& device, std::initializer_list<char> perms) {
    id2dev_[id].insert(device);
    auto& permSet = id2perm_[id];
    for (char perm : perms) {
        if (perm != 'r' && perm != 'w' && perm != 'a') {
            throw AppError("Permiso inválido (use r, w o a)");
        }
        permSet.insert(perm);
    }
}

void Verificador::allow(const std::string& id, const std::string& device, const std::string& perms) {
    validatePerms(perms);
    id2dev_[id].insert(device);
    auto& permSet = id2perm_[id];
    for (char perm : perms) {
        permSet.insert(perm);
    }
}

bool Verificador::canRead(const std::string& id, const std::string& device) const {
    return can(id, device, 'r');
}

bool Verificador::canWrite(const std::string& id, const std::string& device) const {
    return can(id, device, 'w');
}

bool Verificador::canAppend(const std::string& id, const std::string& device) const {
    return can(id, device, 'a');
}

bool Verificador::hasDevice(const std::string& id, const std::string& device) const {
    auto it = id2dev_.find(id);
    if (it == id2dev_.end()) {
        return false;
    }
    return it->second.count(device) > 0;
}

bool Verificador::can(const std::string& id, const std::string& device, char perm) const {
    auto permIt = id2perm_.find(id);
    if (permIt == id2perm_.end()) {
        return false;
    }
    if (permIt->second.count(perm) == 0) {
        return false;
    }
    return hasDevice(id, device);
}
