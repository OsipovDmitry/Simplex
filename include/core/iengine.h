#ifndef IENGINE_H
#define IENGINE_H

namespace simplex
{
namespace core
{

class IEngine
{
public:
    virtual ~IEngine() = default;

    virtual void update() = 0;
};

}
}

#endif // IENGINE_H
