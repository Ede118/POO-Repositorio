#pragma once

#include "Types.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

struct AllowRule {
    std::string id;
    std::string device;
    std::string perms; // combinación de r|w|a
};

class CLI {
public:
    void parse(int argc, char** argv);

    Mode mode() const noexcept { return mode_; }
    bool append() const noexcept { return append_; }
    const std::optional<std::filesystem::path>& inputPath() const noexcept { return inputPath_; }
    const std::filesystem::path& outputPath() const noexcept { return outputPath_; }
    const std::vector<AllowRule>& allowRules() const noexcept { return rules_; }

private:
    Mode mode_ = Mode::Write;
    bool append_ = false;
    std::optional<std::filesystem::path> inputPath_{};
    std::filesystem::path outputPath_{"logs/telemetria.csv"};
    std::vector<AllowRule> rules_;

    static void printHelp();
    void parseAllow(const std::string& spec);
};
