#include <graphics_glfw/glfwwidget.h>

#include <utils/image.h>


int main(int argc, char* argv[])
{
    {
        auto window = simplex::graphics_glfw::GLFWWidget::getOrCreate("Empty window");

        auto window2 = simplex::graphics_glfw::GLFWWidget::getOrCreate("Empty window2", window);
        simplex::graphics_glfw::GLFWWidget::run();
    }

    return 0;
}