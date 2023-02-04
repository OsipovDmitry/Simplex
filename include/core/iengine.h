#ifndef IENGINE_H
#define IENGINE_H

class IEngine
{
public:
    virtual ~IEngine() = default;

    virtual void update() = 0;
};

#endif // IENGINE_H
