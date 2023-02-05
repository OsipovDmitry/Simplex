#ifndef IAPPLICATION_H
#define IAPPLICATION_H

namespace simplex
{
namespace core
{

class IApplication
{
public:
    virtual ~IApplication() = default;

    virtual void update() = 0;
};

}
}

#endif // IAPPLICATION_H
