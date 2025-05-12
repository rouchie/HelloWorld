#include <iostream>

#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"
#include "utils/UtilsJson.h"

class A
{
public:
    A() = default;

    virtual ~A() = default;

protected:
    virtual void Hello()
    {
    }
};

class B final : public A
{
protected:
    void Hello() override;
};

int main(int ac, char **av)
{
    std::string name = "我就是要写点中文进去";

    nlohmann::json j;
    j["name"] = name;
    j["age"] = 18;

    SPDLOG_INFO("{}", j.dump());

    {
        std::string hello = get(j, "name", "默认值");
        std::string world = get(j, "music", "默认值.mp3");
        SPDLOG_INFO("{}:{}", name, world);
    }

    return 0;
}
