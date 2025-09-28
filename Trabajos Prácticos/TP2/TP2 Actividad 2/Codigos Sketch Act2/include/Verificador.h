#pragma once

#include <initializer_list>
#include <string>
#include <unordered_map>
#include <unordered_set>

class Verificador {
public:
    void allow(const std::string& id, const std::string& device, std::initializer_list<char> perms);
    void allow(const std::string& id, const std::string& device, const std::string& perms);

    bool canRead(const std::string& id, const std::string& device) const;
    bool canWrite(const std::string& id, const std::string& device) const;
    bool canAppend(const std::string& id, const std::string& device) const;
    bool hasDevice(const std::string& id, const std::string& device) const;

private:
    std::unordered_map<std::string, std::unordered_set<std::string>> id2dev_;
    std::unordered_map<std::string, std::unordered_set<char>> id2perm_;

    bool can(const std::string& id, const std::string& device, char perm) const;
};
