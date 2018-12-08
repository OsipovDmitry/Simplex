#ifndef GLESWIDGET_H
#define GLESWIDGET_H

#include <memory>
#include <glm/mat4x4.hpp>
#include <renderer/forwarddecl.h>

#include <QWidget>

class GLESWidget : public QWidget
{
	Q_OBJECT
public:
    explicit GLESWidget(QWidget *parent = nullptr, renderer::ContextGroupPtr shareGroup = nullptr);

    renderer::ContextPtr context() const;
    void setContext(renderer::ContextPtr context);
    void setProgram(renderer::ProgramPtr program);
    void setVAO(renderer::VertexArrayPtr vao, uint32_t numIndices);
    void setTransform(const glm::mat4x4 mat);

protected:
	void paintEvent(QPaintEvent*);

private:
	renderer::ContextPtr m_context;
	renderer::ProgramPtr m_program;
	renderer::VertexArrayPtr m_vertexArray;
	renderer::BufferPtr m_uniformBuffer;
	renderer::TexturePtr m_texture;

	renderer::TexturePtr m_framebufferTexture;
	renderer::FramebufferPtr m_framebuffer;

    glm::mat4x4 m_transform;
    uint32_t m_numIndices;
};

#endif // GLESWIDGET_H
