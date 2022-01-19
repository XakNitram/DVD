#include "pch.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "Core/Event.hpp"
#include "Core/Window.hpp"


using namespace std::chrono;
using namespace lwvl::debug;
using Vector = glm::vec2;

template<class clock>
static inline double delta(time_point<clock> start) {
    return 0.000001 * static_cast<double>(duration_cast<microseconds>(
        high_resolution_clock::now() - start
    ).count());
}


std::ostream & operator << (std::ostream &out, Vector const &c) {
    out << "(" << c.x << ", " << c.y << ")";
    return out;
}


class Logo {
public:
    static constexpr float speed = 1000.0f;
    static constexpr float tolerance = 50.0f;

    Vector position;
    Vector velocity;
    Vector bounds;
    float width, height;

    Logo(float x, float y, float angle, int width, int height, Vector bounds):
        position(x, y), velocity(speed * std::cosf(angle), speed * std::sinf(angle)),
        bounds(bounds), width(static_cast<float>(width)), height(static_cast<float>(height))
    {
        std::cout << bounds << std::endl;
    }

    void update(float dt) {
        Vector newPosition = position + (velocity * dt);
        float left = newPosition.x + bounds.x;
        float right = (newPosition.x + width) - bounds.x;
        float bottom = newPosition.y + bounds.y;
        float top = (newPosition.y + height) - bounds.y;

        if (right >= 0.0f) {
            newPosition.x = bounds.x - width;
            velocity *= Vector{-1.0f, 1.0f};
        } else if (left <= 0.0f) {
            newPosition.x = -bounds.x;
            velocity *= Vector{-1.0f, 1.0f};
        }

        if (top >= 0.0f) {
            newPosition.y = bounds.y - height;
            velocity *= Vector{1.0f, -1.0f};
        } else if (bottom <= 0.0f) {
            newPosition.y = -bounds.y;
            velocity *= Vector{1.0f, -1.0f};
        }

        if ((left <= 0.0f && bottom <= 0.0f) && glm::distance(left, bottom) <= tolerance) {
            //std::cout << "Hit bottom left corner" << std::endl;
            velocity = Vector{0.0f, 0.0f};
        } else if ((right >= 0.0f && bottom <= 0.0f) && glm::distance(right, bottom) <= tolerance) {
            //std::cout << "Hit bottom right corner" << std::endl;
            velocity = Vector{0.0f, 0.0f};
        } else if ((right >= 0.0f && top >= 0.0f) && glm::distance(right, top) <= tolerance) {
            //std::cout << "Hit top right corner" << std::endl;
            velocity = Vector{0.0f, 0.0f};
        } else if ((left <= 0.0f && top >= 0.0f) && glm::distance(left, top) <= tolerance) {
            //std::cout << "Hit top left corner" << std::endl;
            velocity = Vector{0.0f, 0.0f};
        }

        position = newPosition;
    }
};


void move(Logo& logo, float dt) {
    logo.update(dt);
}


int run(int width, int height) {
    Window window(width, height, "DVD Standby");


#ifndef NDEBUG
    GLEventListener listener(
        [](
            Source source, Type type,
            Severity severity, unsigned int id, int length,
            const char *message, const void *userState
        ) {
            std::cout << "[OpenGL] " << message << std::endl;
        }
    );
#endif

    const auto startTime = high_resolution_clock::now();
    const auto width_f = static_cast<float>(width);
    const auto height_f = static_cast<float>(height);
    const float aspect = width_f / height_f;

    lwvl::VertexArray array;
    lwvl::ArrayBuffer vertexBuffer;
    lwvl::Texture dvd{lwvl::Texture::Target::Texture2D};
    lwvl::ShaderProgram control;
    control.link(
        lwvl::VertexShader::readFile("Data/Shaders/quad.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/quad.frag")
    );

    control.bind();
    control.uniform("projection").ortho2D(
        height_f * 0.5f, -height_f * 0.5f,
        width_f * 0.5f, -width_f * 0.5f
    );

    float vertices[] {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0
    };

    array.bind();
    vertexBuffer.bind();
    vertexBuffer.construct(vertices, 8);
    array.attribute(2, GL_FLOAT, 2 * sizeof(float), 0, 0);


    stbi_set_flip_vertically_on_load(1);
    int logoWidth, logoHeight, logoBPP;
    unsigned char* localBuffer = stbi_load(
        "Data/Textures/dvd-logo.png",
        &logoWidth, &logoHeight, &logoBPP, 4
    );

    //for (int i = 0; i < (logoWidth * logoHeight) / 4; i++) {
    //    std::cout << "[ ";
    //    for (int j = 0; j < 4; j++) {
    //        std::cout << static_cast<int>(localBuffer[(i * 4) + j]) << ", ";
    //    }
    //    std::cout << "], ";
    //}
    //std::cout << std::endl;

    dvd.bind();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    dvd.filter(lwvl::Filter::Linear);
    dvd.construct(
        logoWidth, logoHeight, lwvl::ChannelLayout::RGBA32F,
        lwvl::ChannelOrder::RGBA,
        lwvl::ByteFormat::UnsignedByte,
        localBuffer
    );
    stbi_image_free(localBuffer);

    control.bind();
    control.uniform("tex").set(static_cast<int>(dvd.slot()));

    const auto time_random = static_cast<float>(
        duration_cast<microseconds>(high_resolution_clock::now() - startTime).count()
    );

    Logo logo(0.0f, 0.0f, time_random, logoWidth, logoHeight, Vector{width_f * 0.5f, height_f * 0.5f});

    control.bind();
    control.uniform("scale").set(logo.width, logo.height);

    auto frameStart = high_resolution_clock::now();
    while (!window.shouldClose()) {
        const auto dt = static_cast<float>(delta(frameStart));
        frameStart = high_resolution_clock::now();

        // Fill event buffer
        Window::update();

        while (std::optional<Event> possible = window.pollEvent()) {
            Event& concrete = possible.value();

            if (concrete.type == Event::Type::KeyRelease
                && std::get<KeyboardEvent>(concrete.event).key == GLFW_KEY_ESCAPE
            ) {
                window.shouldClose(true);
            }
        }

        move(logo, dt);

        // Rendering
        lwvl::clear();
        dvd.bind();
        control.bind();
        control.uniform("offset").set(logo.position.x, logo.position.y);
        array.bind();
        array.drawArrays(lwvl::PrimitiveMode::TriangleFan, 4);

        window.swapBuffers();
    }

    return 0;
}


int main() {
    try {
        return run(800, 600);
    } catch(std::exception& e) {
        std::cout << "Errored." << std::endl;
        std::cerr << e.what() << std::endl;
    }
}
