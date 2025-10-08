#pragma once

#include "Types.h"

#include <filesystem>
#include <string>
#include <vector>

class File {
public:
    explicit File(std::filesystem::path path);

    const std::filesystem::path& path() const noexcept;

    void writeHeaderV2();
    void writeEventRow(const std::string& id, Event ev, const std::vector<std::string>& cols4);

    static std::string nowStr();

private:
    static constexpr const char* kHeader = "#h, ID, Date, Event, c1, c2, c3, c4";

    std::filesystem::path path_;
    bool headerWritten_ = false;

    void ensureParentDir() const;
    bool hasHeaderOnDisk() const;
    static std::string buildRow(const std::string& id, Event ev, const std::vector<std::string>& cols4);
};
