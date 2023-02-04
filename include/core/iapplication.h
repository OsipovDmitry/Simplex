#ifndef IAPPLICATION_H
#define IAPPLICATION_H

class IApplication
{
public:
    virtual ~IApplication() = default;

    virtual void update() = 0;
};

#endif // IAPPLICATION_H
