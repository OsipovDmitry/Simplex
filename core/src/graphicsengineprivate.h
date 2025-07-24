#ifndef GRAPHICSENGINEPRIVATE_H
#define GRAPHICSENGINEPRIVATE_H

#include <utils/boundingbox.h>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class GraphicsEnginePrivate
{
public:
    GraphicsEnginePrivate(const std::string&);

    std::string& name();

    std::shared_ptr<graphics::RendererBase> &renderer();
    std::shared_ptr<TexturesManager> &texturesManager();
    std::shared_ptr<ProgramsManager> &programsManager();

    graphics::PBuffer &fragmentsBuffer();
    graphics::PBuffer &fragmentsCounter();

    std::shared_ptr<graphics::IVertexArray> &screenQuadVertexArray();

    std::shared_ptr<graphics::IVertexArray> &pointLightAreaVertexArray();
    utils::BoundingBox &pointLightAreaBoundingBox();

    std::shared_ptr<graphics::IVertexArray> &spotLightAreaVertexArray();
    utils::BoundingBox &spotLightAreaBoundingBox();

    std::shared_ptr<graphics::IVertexArray> &directionalLightAreaVertexArray();

    std::shared_ptr<Drawable> &screenQuadDrawable();

    std::shared_ptr<FlatDrawable> &nodeBoundingBoxDrawable();
    std::shared_ptr<FlatDrawable> &visualDrawableNodeLocalBoundingBoxDrawable();
    std::shared_ptr<FlatDrawable> &visualDrawableBoundingBoxDrawable();
    std::shared_ptr<FlatDrawable> &lightNodeAreaBoundingBoxDrawable();

    static const std::string &vertexAttributeNameById(utils::VertexAttribute);
    static const std::string &uniformNameById(UniformId);
    static const std::string &SSBONameById(SSBOId);

private:
    std::string m_name;

    std::shared_ptr<graphics::RendererBase> m_renderer;
    std::shared_ptr<TexturesManager> m_texturesManager;
    std::shared_ptr<ProgramsManager> m_programsManager;

    graphics::PBuffer m_fragmentsBuffer;
    graphics::PBuffer m_fragmentsCounter;

    std::shared_ptr<graphics::IVertexArray> m_screenQuadVertexArray;

    std::shared_ptr<graphics::IVertexArray> m_pointLightAreaVertexArray;
    utils::BoundingBox m_pointLightAreaBoundingBox;

    std::shared_ptr<graphics::IVertexArray> m_spotLightAreaVertexArray;
    utils::BoundingBox m_spotLightAreaBoundingBox;

    std::shared_ptr<graphics::IVertexArray> m_directionalLightAreaVertexArray;

    std::shared_ptr<Drawable> m_screenQuadDrawable;

    std::shared_ptr<FlatDrawable> m_nodeBoundingBoxDrawable;
    std::shared_ptr<FlatDrawable> m_visualDrawableNodeLocalBoundingBoxDrawable;
    std::shared_ptr<FlatDrawable> m_visualDrawableBoundingBoxDrawable;
    std::shared_ptr<FlatDrawable> m_lightNodeAreaBoundingBoxDrawable;

    graphics::PTexture m_defaultSSAOTexture;

    std::shared_ptr<Drawable> m_finalScreenQuadDrawable;
};

}
}

#endif // GRAPHICSENGINEPRIVATE_H
