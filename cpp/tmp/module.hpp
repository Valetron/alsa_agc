#ifndef MODULE_HPP
#define MODULE_HPP

#include <vector>

class ModuleBase
{
public:
    ~ModuleBase() = default;

    template <typename T>
    void processFrame(std::vector<T>& buffer);
};

#endif // MODULE_HPP
