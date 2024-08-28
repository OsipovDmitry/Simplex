#ifndef CORE_VISUALDRAWABLE_H
#define CORE_VISUALDRAWABLE_H

#include <core/drawable.h>

namespace simplex
{
namespace core
{

class VisualDrawablePrivate;
class CORE_SHARED_EXPORT VisualDrawable : public Drawable
{
    PRIVATE_IMPL(VisualDrawable)

public:
    ~VisualDrawable() override;

    const utils::BoundingBox &boundingBox() const;

    bool isTransparent() const;

    float alphaCutoff() const;
    void setAlphaCutoff(float);

    bool isLighted() const;
    bool isShadowed() const;
    bool isShadowCasted() const;

    static const DrawableComponentSet &componentSet();

protected:
    VisualDrawable(std::unique_ptr<VisualDrawablePrivate>);

    void setLightedFlag(bool);
    void setShadowedFlag(bool);
    void setShadowCastedFlag(bool);

};

}
}


#endif // CORE_VISUALDRAWABLE_H
