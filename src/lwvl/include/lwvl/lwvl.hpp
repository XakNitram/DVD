#pragma once

// Figure out how to make this library independent.
#include <glad/glad.h>

#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <variant>


namespace lwvl {
    enum class Attachment {
        Color = GL_COLOR_ATTACHMENT0,
        Depth = GL_DEPTH_ATTACHMENT,
        Stencil = GL_STENCIL_ATTACHMENT
    };

    enum class ByteFormat {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        HalfFloat = GL_HALF_FLOAT,
        Float = GL_FLOAT,
    };

    enum class ChannelLayout {
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RGBA = GL_RGBA,
        BGRA = GL_BGRA,

        R8 = GL_R8,
        R16 = GL_R16,
        RG8 = GL_RG8,
        RG16 = GL_RG16,
        RGB8 = GL_RGB8,
        RGBA2 = GL_RGBA2,
        RGBA4 = GL_RGBA4,
        RGBA8 = GL_RGBA8,
        RGBA16 = GL_RGBA16,

        // Half Float Formats
        R16F = GL_R16F,
        RG16F = GL_RG16F,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,

        // Full Float Formats
        R32F = GL_R32F,
        RG32F = GL_RG32F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,

        // Signed Integer Formats
        R8I = GL_R8I,
        RG8I = GL_RG8I,
        RGB8I = GL_RGB8I,
        RGBA8I = GL_RGBA8I,

        R16I = GL_R16I,
        RG16I = GL_RG16I,
        RGB16I = GL_RGB16I,
        RGBA16I = GL_RGBA16I,

        R32I = GL_R32I,
        RG32I = GL_RG32I,
        RGB32I = GL_RGB32I,
        RGBA32I = GL_RGBA32I,

        // Unsigned Integer Formats
        R8UI = GL_R8UI,
        RG8UI = GL_RG8UI,
        RGB8UI = GL_RGB8UI,
        RGBA8UI = GL_RGBA8UI,

        R16UI = GL_R16UI,
        RG16UI = GL_RG16UI,
        RGB16UI = GL_RGB16UI,
        RGBA16UI = GL_RGBA16UI,

        R32UI = GL_R32UI,
        RG32UI = GL_RG32UI,
        RGB32UI = GL_RGB32UI,
        RGBA32UI = GL_RGBA32UI,
    };

    enum class ChannelOrder {
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RGBA = GL_RGBA,
        BGRA = GL_BGRA,
        StencilIndex = GL_STENCIL_INDEX,
        DepthComponent = GL_DEPTH_COMPONENT,
        DepthStencil = GL_DEPTH_STENCIL
    };

    enum class Filter {
        Linear = GL_LINEAR,
        Nearest = GL_NEAREST
    };

    enum class PrimitiveMode {
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        LineStrip = GL_LINE_STRIP,
        LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
        Triangles = GL_TRIANGLES,
        TriangleFan = GL_TRIANGLE_FAN,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
        TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,
    };

    enum class BufferTarget {
        Array = GL_ARRAY_BUFFER,
        Element = GL_ELEMENT_ARRAY_BUFFER,
        Texture = GL_TEXTURE_BUFFER,
        Uniform = GL_UNIFORM_BUFFER,
        ShaderStorage = GL_SHADER_STORAGE_BUFFER
    };

    enum class ShaderTarget {
        Vertex = GL_VERTEX_SHADER,
        TessCtrl = GL_TESS_CONTROL_SHADER,
        TessEval = GL_TESS_EVALUATION_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute = GL_COMPUTE_SHADER
    };

    class Framebuffer;
    class ShaderProgram;
    class Texture;
    class Uniform;
    class VertexArray;
    class WorldBlock;

    namespace details {
        template<BufferTarget> class Buffer;
    }

    typedef details::Buffer<BufferTarget::Array> ArrayBuffer;
    typedef details::Buffer<BufferTarget::Element> ElementBuffer;
    typedef details::Buffer<BufferTarget::Texture> TextureBuffer;
    typedef details::Buffer<BufferTarget::Uniform> UniformBuffer;
    typedef details::Buffer<BufferTarget::ShaderStorage> ShaderStorageBuffer;

    typedef std::variant<
        ArrayBuffer,
        ElementBuffer,
        TextureBuffer,
        UniformBuffer,
        ShaderStorageBuffer
    > Buffer;

    namespace details {
        using AnonymousBuffer = lwvl::Buffer;
        template<ShaderTarget> class Shader;
    }

    typedef details::Shader<ShaderTarget::Vertex> VertexShader;
    typedef details::Shader<ShaderTarget::TessCtrl> TesselationControlShader;
    typedef details::Shader<ShaderTarget::TessEval> TesselationEvaluationShader;
    typedef details::Shader<ShaderTarget::Geometry> GeometryShader;
    typedef details::Shader<ShaderTarget::Fragment> FragmentShader;
    typedef details::Shader<ShaderTarget::Compute> ComputeShader;
    typedef std::variant<
        VertexShader,
        TesselationControlShader,
        TesselationEvaluationShader,
        GeometryShader,
        FragmentShader,
        ComputeShader
    > Shader;

    namespace details {
        template<BufferTarget targetType>
        class Buffer {
        public:
            enum class Usage {
                Static = GL_STATIC_DRAW,
                Dynamic = GL_DYNAMIC_DRAW,
                Stream = GL_STREAM_DRAW
            };
        private:
            class ID {
                static unsigned int reserve() {
                    unsigned int tempID;
                    glGenBuffers(1, &tempID);
                    return tempID;
                }

            public:
                ~ID() {
                    glDeleteBuffers(1, &bufferID);
                }

                explicit operator unsigned int() const {
                    return bufferID;
                }

                const unsigned int bufferID = reserve();
            };

            constexpr static BufferTarget target{targetType};

            // Offsite Data - to avoid copying buffers on the GPU for simple copies of this class.
            std::shared_ptr<Buffer::ID> m_offsite_id = std::make_shared<Buffer::ID>();

            // Local Data
            GLuint m_id;
            Usage m_usage;

        public:
            unsigned int id() {
                return m_id;
            }

            Buffer() :
                m_offsite_id(std::make_shared<Buffer::ID>()),
                m_id(static_cast<GLuint>(*m_offsite_id)),
                m_usage(Usage::Dynamic)
            {}

            explicit Buffer(Usage usage) :
                m_offsite_id(std::make_shared<Buffer::ID>()),
                m_id(static_cast<GLuint>(*m_offsite_id)),
                m_usage(usage)
            {}

            Buffer(const Buffer &other) :
                m_offsite_id(other.m_offsite_id),
                m_id(static_cast<GLuint>(*m_offsite_id)),
                m_usage(other.m_usage)
            {}

            Buffer &operator=(const Buffer &other) {
                m_offsite_id = other.m_offsite_id;
                m_id = static_cast<GLuint>(*m_offsite_id);
                m_usage = other.m_usage;
            }

            Buffer(Buffer &&other) noexcept :
                m_offsite_id(other.m_offsite_id),
                m_id(static_cast<GLuint>(*m_offsite_id)),
                m_usage(other.m_usage)
            {}

            Buffer &operator=(Buffer &&other) noexcept {
                m_offsite_id(other.m_offsite_id),
                m_id = static_cast<GLuint>(*m_offsite_id);
                m_usage = other.m_usage;
            };

            explicit Buffer(const AnonymousBuffer &buffer) : m_offsite_id(nullptr) {
                lwvl::ArrayBuffer& other{std::get<Buffer<target>>(buffer)};
                m_offsite_id = other.m_offsite_id;
                m_id = static_cast<GLuint>(*m_offsite_id);
                m_usage = other.m_usage;
            }

            Buffer &operator=(const AnonymousBuffer &buffer) {
                lwvl::ArrayBuffer& other{std::get<Buffer<target>>(buffer)};
                m_offsite_id = other.m_offsite_id;
                m_id = static_cast<GLuint>(*m_offsite_id);
                m_usage = other.m_usage;
            }

            explicit Buffer(const AnonymousBuffer &&buffer) : m_offsite_id(nullptr) {
                lwvl::ArrayBuffer& other{std::get<Buffer<target>>(buffer)};
                m_offsite_id = other.m_offsite_id;
                m_id = static_cast<GLuint>(*m_offsite_id);
                m_usage = other.m_usage;
            }

            Buffer &operator=(const AnonymousBuffer &&buffer) {
                lwvl::ArrayBuffer& other{std::get<Buffer<target>>(buffer)};
                m_offsite_id = other.m_offsite_id;
                m_id = static_cast<GLuint>(*m_offsite_id);
                m_usage = other.m_usage;
            }

            template<typename T>
            void construct(const T *data, GLsizei count) {
                glBufferData(
                    static_cast<GLenum>(target), sizeof(T) * count,
                    data, static_cast<GLenum>(m_usage)
                );
            }

            template<class Iterator>
            void construct(Iterator first, Iterator last) {
                glBufferData(
                    static_cast<GLenum>(target), sizeof(*first) * (last - first),
                    &(*first), static_cast<GLenum>(m_usage)
                );
            }

            template<typename T>
            void update(const T *data, GLsizei count, GLsizei offset = 0) {
                glBufferSubData(static_cast<GLenum>(target), offset, count * sizeof(T), data);
            }

            template<class Iterator>
            void update(Iterator first, Iterator last, GLsizei offset = 0) {
                glBufferSubData(
                    static_cast<GLenum>(target), offset, sizeof(*first) * (last - first), &(*first));
            }

            void usage(Usage usage) { m_usage = usage; }
            Usage usage() { return m_usage; }

            [[nodiscard]] GLuint id() const {
                return m_id;
            }

            // These operations should not be const because they modify GL state.
            void bind() {
                glBindBuffer(
                    static_cast<GLenum>(target),
                    m_id
                );
            }

            static void clear() {
                glBindBuffer(static_cast<GLenum>(target), 0);
            }
        };

        template<ShaderTarget target>
        class Shader {
            unsigned int m_id{reserve()};

            friend class lwvl::ShaderProgram;

            static unsigned int reserve() {
                unsigned int id = glCreateShader(static_cast<GLenum>(target));
                return id;
            }

        public:
            explicit Shader(const std::string &source) {
                const char *src = source.c_str();
                glShaderSource(m_id, 1, &src, nullptr);
                glCompileShader(m_id);

                int result;
                glGetShaderiv(m_id, GL_COMPILE_STATUS, &result);
                if (result == GL_FALSE) {
                    int length;
                    glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);
                    char *message = static_cast<char *>(_malloca(length * sizeof(char)));
                    glGetShaderInfoLog(m_id, length, &length, message);
                    std::stringstream error;
                    error << "Failed to compile ";

                    switch (target) {
                        case ShaderTarget::Vertex: error << "vertex"; break;
                        case ShaderTarget::Fragment: error << "fragment"; break;
                        case ShaderTarget::Geometry: error << "geometry"; break;
                        case ShaderTarget::TessCtrl: error << "tesselation control"; break;
                        case ShaderTarget::TessEval: error << "tesselation evaluation"; break;
                        case ShaderTarget::Compute: error << "compute"; break;
                        default: error << "unknown type of ";
                            break;
                    }

                    error << " shader" << std::endl << message << std::endl;
                    throw std::invalid_argument(error.str().c_str());
                }
            }

            Shader(const Shader &other) = delete;

            Shader(Shader &&other) noexcept: m_id(other.m_id) {
                other.m_id = 0;
            }

            Shader &operator=(const Shader &other) = delete;

            Shader &operator=(Shader &&other) noexcept {
                m_id = other.m_id;
                other.m_id = 0;
                return *this;
            }

            static std::string readFile(const std::string &filepath) {
                // need to figure out how to handle errors on this.
                std::ifstream file(filepath);
                std::stringstream output_stream;

                std::string line;
                while (getline(file, line)) {
                    output_stream << line << '\n';
                }

                return output_stream.str();
            }

            ~Shader() {
                // An id of 0 will be silently ignored.
                glDeleteShader(m_id);
            }

            [[nodiscard]] GLuint id() const {
                return m_id;
            }
        };
    }

    namespace debug {
        enum class Source {
            API = GL_DEBUG_SOURCE_API,
            APPLICATION = GL_DEBUG_SOURCE_APPLICATION,
            SHADER_COMPILER = GL_DEBUG_SOURCE_SHADER_COMPILER,
            THIRD_PARTY = GL_DEBUG_SOURCE_THIRD_PARTY,
            WINDOW_SYSTEM = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
            OTHER = GL_DEBUG_SOURCE_OTHER,
            DONT_CARE = GL_DONT_CARE
        };

        enum class Type {
            DEPRECATED_BEHAVIOR = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
            ERROR = GL_DEBUG_TYPE_ERROR,
            MARKER = GL_DEBUG_TYPE_MARKER,
            PERFORMANCE = GL_DEBUG_TYPE_PERFORMANCE,
            PORTABILITY = GL_DEBUG_TYPE_PORTABILITY,
            POP_GROUP = GL_DEBUG_TYPE_POP_GROUP,
            PUSH_GROUP = GL_DEBUG_TYPE_PUSH_GROUP,
            UNDEFINED_BEHAVIOR = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
            OTHER = GL_DEBUG_TYPE_OTHER,
            DONT_CARE = GL_DONT_CARE
        };

        enum class Severity {
            LOW = GL_DEBUG_SEVERITY_LOW,
            MEDIUM = GL_DEBUG_SEVERITY_MEDIUM,
            HIGH = GL_DEBUG_SEVERITY_HIGH,
            NOTIFICATION = GL_DEBUG_SEVERITY_NOTIFICATION,
            DONT_CARE = GL_DONT_CARE
        };

#ifdef _WIN32
        using LWVLDebugProc = void (__stdcall *)(
#else
            using LWVLDebugProc = void(*)(
#endif
            Source source, Type type, Severity severity,
            unsigned int id, int length, const char *message,
            const void *userState
        );

        class GLEventListener {
            GLuint m_unusedIDs = 0;
            LWVLDebugProc m_callback;
            void *m_userPtr;

            void assign();

        public:
            explicit GLEventListener(void *userPtr, LWVLDebugProc callback, bool enabled = true);

            explicit GLEventListener(LWVLDebugProc callback, bool enabled = true);

            ~GLEventListener();

            void control(Source sourceFilter, Type typeFilter, Severity severityFilter, GLsizei count, bool enabled);

            void control(bool enabled);

            void invoke(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message);
        };

        static void __stdcall glDebugCallback(
            GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *state
        );

        void simpleDebugCallback(GLDEBUGPROC callback, void *userPtr);

        void clearErrors();
    }

    void clear();

    struct Viewport {
        int x, y, width, height;
    };

    void viewport(Viewport);
    Viewport viewport();

    class Framebuffer {
        uint32_t m_id = 0;

    public:
        Framebuffer();

        ~Framebuffer();

        void attach(Attachment, Texture &, int level = 0);
        void attach1D(Attachment, Texture &, int level = 0);
        void attach2D(Attachment, Texture &, int level = 0);
        void attach3D(Attachment, Texture &, int layer, int level = 0);

        void bind();

        static void clear();

        [[nodiscard]] GLuint id() const;
    };

    /* ****** Shader Program ******
    * A simple abstraction over the code required to create a shader in OpenGL.
    * Not feature complete.
    *
    * Usage:
    *   ShaderProgram myShader;
    *   myShader.attach(VertexShader);
    *   myShader.attach(GeometryShader);
    *   myShader.attach(FragmentShader);
    *   myShader.link();
    *
    * Quick Linking (Vertex and Fragment Shaders only):
    *   ShaderProgram myShader;
    *   myShader.link(VertexShader, FragmentShader);
    */
    class ShaderProgram {
        class ID {
            static unsigned int reserve() {
                return glCreateProgram();
            }

        public:
            ~ID() {
                glDeleteProgram(programID);
            }

            constexpr explicit operator uint32_t() const {
                return programID;
            }

            const uint32_t programID = reserve();
        };

        // Offsite Data -
        std::shared_ptr<ShaderProgram::ID> m_offsite_id = std::make_shared<ShaderProgram::ID>();

        // Local Data
        uint32_t m_id = static_cast<uint32_t>(*m_offsite_id);

        [[nodiscard]] int uniformLocation(const std::string &name) const;

    public:
        ShaderProgram() = default;

        ShaderProgram(const ShaderProgram &other) = default;

        ShaderProgram &operator=(const ShaderProgram &other) = default;

        ShaderProgram(ShaderProgram &&other) noexcept = default;

        ShaderProgram &operator=(ShaderProgram &&other) noexcept = default;

        [[nodiscard]] unsigned int id() const;

        Uniform uniform(const std::string &name);

        template<ShaderTarget target>
        void attach(const details::Shader<target> &shader) { glAttachShader(m_id, shader.m_id); }

        template<ShaderTarget target>
        void detach(const details::Shader<target> &shader) { glDetachShader(m_id, shader.m_id); }

        void link();

        void link(const VertexShader &vs, const FragmentShader &fs);

        void link(const std::string &vertexSource, const std::string &fragmentSource);

        void bind() const;

        static void clear();
    };

    class Texture {
        friend Framebuffer;
    public:
        class ID {
        protected:
            static GLuint reserve();
        public:
            ~ID();
            explicit operator GLuint() const;
            const GLuint id = reserve();
        };

        enum class Target {
            Texture1D = GL_TEXTURE_1D,
            Texture1DArray = GL_TEXTURE_1D_ARRAY,
            Texture2D = GL_TEXTURE_2D,
            Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
            Texture2DArray = GL_TEXTURE_2D_ARRAY,
            Texture2DArrayMultisample = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            TextureRectangle = GL_TEXTURE_RECTANGLE,
            TextureCubeMap = GL_TEXTURE_CUBE_MAP,
            TextureCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
            Texture3D = GL_TEXTURE_3D,
            TextureBuffer = GL_TEXTURE_BUFFER
        };

        Texture();
        explicit Texture(Target t);

        [[nodiscard]] GLuint slot() const;
        void slot(GLuint value);
        void filter(Filter value);
        void bind();

        void construct(
            int width,
            ChannelLayout layout, ChannelOrder order, ByteFormat type,
            const void *pixels, int level = 0
        );

        void construct(
            int width, int height,
            ChannelLayout layout, ChannelOrder order, ByteFormat type,
            const void *pixels, int level = 0
        );

        void construct(int width, int height, int samples, ChannelLayout layout, bool fixedSampleLocations = false);
        void construct(int width, int height, int depth, int samples, ChannelLayout layout, bool fixedSampleLocations = false);

        void construct(
            int width, int height, int depth,
            ChannelLayout layout, ChannelOrder order, ByteFormat type,
            const void *pixels, int level = 0
        );

        void construct(TextureBuffer &buffer, ChannelLayout layout);

        [[nodiscard]] GLuint id() const;

    private:
        [[nodiscard]] inline GLenum target() const;

        // Offsite data for easy copies.
        std::shared_ptr<ID> m_info = std::make_shared<ID>();

        // Local data to avoid lookups.
        GLuint m_id = m_info->id;
        Target m_target;
        GLuint m_slot = 0;
    };

    class Uniform {
        int m_location = -1;
    public:
        Uniform() = default;
        explicit Uniform(int location);
        Uniform(const Uniform&) = default;
        Uniform(Uniform&&) = default;
        ~Uniform() = default;
        Uniform &operator=(const Uniform&) = default;
        Uniform &operator=(Uniform&&) = default;

        void set(int);
        void set(float);
        void set(unsigned int);
        void set(int, int);
        void set(float, float);
        void set(unsigned int, unsigned int);
        void set(int, int, int);
        void set(float, float, float);
        void set(unsigned int, unsigned int, unsigned int);
        void set(int, int, int, int);
        void set(float, float, float, float);
        void set(unsigned int, unsigned int, unsigned int, unsigned int);
        void matrix4(const float *data);
        void ortho(float top, float bottom, float right, float left, float far, float near);
        void ortho2D(float top, float bottom, float right, float left);

        [[nodiscard]] int location() const;
    };

    class VertexArray {
        // There are only < 256 attribute bind sites, so this could be a uint16_t
        //   if another 16 or 2x8 bytes can be used for something else.
        unsigned int m_attributes = 0;
        unsigned int m_instances = 1;
        unsigned int m_id = 0;
    public:
        VertexArray();

        ~VertexArray();

        void bind();

        static void clear();

        [[nodiscard]] GLuint id() const;

        [[nodiscard]] uint32_t instances() const;
        void instances(uint32_t count);

        void attribute(uint8_t dimensions, GLenum type, int64_t stride, int64_t offset, uint32_t divisor = 0);

        void drawArrays(PrimitiveMode mode, int count) const;

        void drawElements(PrimitiveMode mode, int count, ByteFormat type) const;

        void multiDrawArrays(PrimitiveMode mode, const GLint *firsts, const GLsizei *counts, GLsizei drawCount);

        void multiDrawElements(
            PrimitiveMode mode, const GLsizei *counts, ByteFormat type, const void *const *indices, GLsizei drawCount
        );
    };

    class WorldBlock {
    public:
        WorldBlock() = default;
        void attach(const ShaderProgram& program);
    private:
        lwvl::UniformBuffer blockBuffer;
    };
}
