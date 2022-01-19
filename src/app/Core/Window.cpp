#include "pch.hpp"
#include "Window.hpp"


Window *Window::getState(GLFWwindow *window) {
    return static_cast<Window *>(glfwGetWindowUserPointer(window));
}


void Window::terminate() {
    if (m_window != nullptr) {
        glfwSetWindowUserPointer(m_window, nullptr);
        glfwDestroyWindow(m_window);
        glfwPollEvents();
    }

    glfwTerminate();
}


Window::Window(int width, int height, const char *title, GLFWmonitor *monitor) : config({width, height}) {
    m_events.reserve(eventStackCapacity);

    /* Initialize GLFW. */
    if (!glfwInit()) {
        throw std::exception("Failed to initialize GLFW.");
    }

    // Set GLFW window hints.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // !NDEBUG

    glfwWindowHint(GLFW_SAMPLES, config.samples);

    glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    //glfwWindowHint(GLFW_SRGB_CAPABLE, true);

    /* Create a GLFW window and its OpenGL context. */
    m_window = glfwCreateWindow(width, height, title, monitor, nullptr);
    glfwMakeContextCurrent(m_window);
    if (!m_window) {
        terminate();
        throw std::exception("Failed to create GLFW window.");
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        terminate();
        throw std::exception("Failed to initialize Glad.");
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSwapInterval(1);

    glfwSetKeyCallback(
        m_window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            Window *state = Window::getState(window);
            switch (action) {
                case GLFW_PRESS:
                    state->m_events.emplace_back(
                        Event::Type::KeyPress, KeyboardEvent{key, scancode, mods}
                    );
                    return;
                case GLFW_RELEASE:
                    state->m_events.emplace_back(
                        Event::Type::KeyRelease, KeyboardEvent{key, scancode, mods}
                    );
                    return;
                case GLFW_REPEAT:
                    state->m_events.emplace_back(
                        Event::Type::KeyRepeat, KeyboardEvent{key, scancode, mods}
                    );
                    return;
                default:return;
            }
        }
    );

    glfwSetCursorPosCallback(
        m_window, [](GLFWwindow *window, double xpos, double ypos) {
            Window *state = Window::getState(window);
            state->m_events.emplace_back(Event::Type::MouseMotion, MouseMotionEvent{xpos, ypos});
        }
    );

    glfwSetMouseButtonCallback(
        m_window, [](GLFWwindow *window, int button, int action, int mods) {
            Window *state = Window::getState(window);
            switch (action) {
                case GLFW_PRESS:state->m_events.emplace_back(Event::Type::MouseDown, MouseButtonEvent{button, mods});
                    return;
                case GLFW_RELEASE:state->m_events.emplace_back(Event::Type::MouseUp, MouseButtonEvent{button, mods});
                default:return;
            }
        }
    );

    glfwSetCharCallback(
        m_window, [](GLFWwindow *window, unsigned int codepoint) {
            Window *state = Window::getState(window);
            state->m_events.emplace_back(Event::Type::TextInput, TextEvent{codepoint});
        }
    );

    /* Output the current OpenGL version. */
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
}


bool Window::shouldClose() {
    return glfwWindowShouldClose(m_window);
}


void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}


void Window::update() {
    glfwPollEvents();
}


void Window::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}


std::optional<Event> Window::pollEvent() {
    if (m_events.empty()) {
        return std::nullopt;
    } else {
        Event event = m_events.back();
        m_events.pop_back();
        return event;
    }
}

void Window::pushEvent(Event event) {
    m_events.push_back(event);
}

void Window::shouldClose(bool value) {
    glfwSetWindowShouldClose(m_window, value);
}

void Window::setIcon(const Icon &icon) {
    GLFWimage internal{icon.width, icon.height, icon.pixels};
    glfwSetWindowIcon(m_window, 1, &internal);
}
