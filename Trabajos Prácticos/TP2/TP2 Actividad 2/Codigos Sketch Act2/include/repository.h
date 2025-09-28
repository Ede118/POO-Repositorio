#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "serializer.h"
#include <string>

class IRepository {
public:
    virtual ~IRepository() = default;
    virtual void save(const Message& m, const std::string& path) = 0;
    virtual Message load(const std::string& path) = 0;
};

class FileRepository final : public IRepository {
public:
    explicit FileRepository(ISerializer& s): ser_(s) {}
    void save(const Message& m, const std::string& path) override; // TODO
    Message load(const std::string& path) override;                // TODO
private:
    ISerializer& ser_;
};

#endif 