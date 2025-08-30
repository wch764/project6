#pragma once
#include<vector>
#include<string>
#include <stdexcept>
//序列化抽象类
class Serializable {
public:
    // 保留原有文件接口
    virtual void saveToFile(const std::string& filename) const = 0;
    virtual void loadFromFile(const std::string& filename) = 0;

    // 新增流式接口（非纯虚，提供默认空实现）
    virtual bool loadFromStream(std::istream& /*file*/) {
        throw std::runtime_error("loadFromStream not implemented");
    }

    virtual void saveToStream(std::ostream& /*file*/) const {
        throw std::runtime_error("saveToStream not implemented");
    }

    virtual ~Serializable() = default;
};
