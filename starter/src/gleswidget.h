#ifndef GLESWIDGET_H
#define GLESWIDGET_H

#include <memory>

#include <QWidget>

namespace renderer {

class Context;
using ContextPtr = std::shared_ptr<Context>;

class ContextGroup;
using ContextGroupPtr = std::shared_ptr<ContextGroup>;

class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;

class Program;
using ProgramPtr = std::shared_ptr<Program>;

class VertexArray;
using VertexArrayPtr = std::shared_ptr<VertexArray>;

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;

}

class GLESWidget : public QWidget
{
	Q_OBJECT
public:
    explicit GLESWidget(QWidget *parent = nullptr, renderer::ContextGroupPtr shareGroup = nullptr);

    renderer::ContextPtr context() const;
    void setContext(renderer::ContextPtr context);
    void setProgram(renderer::ProgramPtr program);

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
};

#endif // GLESWIDGET_H
