#pragma once
#include<vector>
#include<string>
#include <stdexcept>
//���л�������
class Serializable {
public:
    // ����ԭ���ļ��ӿ�
    virtual void saveToFile(const std::string& filename) const = 0;
    virtual void loadFromFile(const std::string& filename) = 0;

    // ������ʽ�ӿڣ��Ǵ��飬�ṩĬ�Ͽ�ʵ�֣�
    virtual bool loadFromStream(std::istream& /*file*/) {
        throw std::runtime_error("loadFromStream not implemented");
    }

    virtual void saveToStream(std::ostream& /*file*/) const {
        throw std::runtime_error("saveToStream not implemented");
    }

    virtual ~Serializable() = default;
};
