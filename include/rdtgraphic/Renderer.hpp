#pragma once

#include <iostream>
#include <print>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Triangle.hpp"

constexpr const char* VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec2 position;
uniform vec2 vertexScale;

void main() {
    gl_Position = vec4(aPos * vertexScale + position, 0.0, 1.0);
}
)";

constexpr const char* FRAGMENT_SHADER = R"(
#version 330 core
uniform vec4 color;
out vec4 FragColor;

void main() {
    FragColor = color;
}
)";

namespace RDTGraphic {

using std::vector;
using std::print;

enum DrawMode {
    NDC,
    Pixel
};

class Renderer {
    GLFWwindow* window = nullptr;
    vector<Tria> triangles = {};

    GLuint program = 0; 
    GLint color_loc = -1;
    GLint position_loc = -1;
    GLint scale_loc = -1;

    int windowHeight = 600;
    int windowWidth  = 800;

    void checkInitialized() {
        if (!window) {
            std::cerr << "Renderer Error: Maybe you called functions before calling init()!\n";
            std::exit(1);
        }
    }

public:
    // DO NOT COPY WE HAVE COPYRIGHTS
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer() {
        // just sleep...
    }

    GLuint compile_shader(const char* source, GLenum type) {
        GLuint shader = glCreateShader(type);

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success != GL_TRUE) {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            std::vector<char> log(length > 0 ? length : 1);
            glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), nullptr, log.data());

            std::cerr << "Shader compilation failed:\n" << log.data() << '\n';

            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    bool init(int width = 800, int height = 600) {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(
            width, height, "Drawing Texts", nullptr, nullptr
        );

        if (!window) {
            std::cerr << "Failed to create window\n";
            return false;
        }

        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int, int action, int) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        });

        glewExperimental = GL_TRUE;

        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW\n";
            return false;
        }

        GLuint vertex = compile_shader(VERTEX_SHADER, GL_VERTEX_SHADER);
        GLuint fragment = compile_shader(FRAGMENT_SHADER, GL_FRAGMENT_SHADER);

        if (!vertex || !fragment) {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return false;
        }

        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);

        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        if (success != GL_TRUE) {
            GLint length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

            if (length <= 0) {
                length = 1;
            }

            std::vector<char> log(length);
            glGetProgramInfoLog(program, static_cast<GLsizei>(log.size()), nullptr, log.data());

            std::cerr << "Shader linking failed:\n" << log.data() << '\n';
            return false;
        }

        color_loc = glGetUniformLocation(program, "color");
        position_loc = glGetUniformLocation(program, "position");
        scale_loc = glGetUniformLocation(program, "vertexScale");

        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        
        return true;
    }

    void registerTriangles(vector<Tria>&& v_triangles) {
        checkInitialized();
 
        triangles = std::move(v_triangles);

        for (auto& tria : triangles) {
            glGenVertexArrays(1, &tria.vao_id);
            glGenBuffers(1, &tria.vbo_id);

            glBindVertexArray(tria.vao_id);
            glBindBuffer(GL_ARRAY_BUFFER, tria.vbo_id);

            glBufferData(
                GL_ARRAY_BUFFER,
                sizeof(tria.vertices),
                tria.vertices.data(),
                GL_STATIC_DRAW
            );

            glVertexAttribPointer(
                0,
                2,
                GL_FLOAT,
                GL_FALSE,
                2 * sizeof(float),
                nullptr
            );

            glEnableVertexAttribArray(0);
        }
        
        glBindVertexArray(0);
    }

    bool draw(DrawMode mode = DrawMode::NDC) {
        static bool warnOnce = false;
        static bool zeroSizeWarn = false;
        checkInitialized();

        glfwPollEvents();

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        if ((width == 0 || height == 0) && !zeroSizeWarn) {
            print("Zero size is not allowed for window!\n");
            zeroSizeWarn = true;
            return false;
        }

        glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        switch (mode) {
            case NDC: {
                for (const auto& tria : triangles) {
                    auto color = tria.color.to_array();

                    glUniform4fv(color_loc, 1, color.data());
                    glUniform2f(
                        position_loc,
                        tria.position.x,
                        tria.position.y
                    );

                    glUniform2f(scale_loc, 1.0f, 1.0f);

                    glBindVertexArray(tria.vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, 3);
                }
                
                break;
            }

            case Pixel: {
                for (const auto& tria : triangles) {
                    auto color = tria.color.to_array();

                    glUniform4fv(color_loc, 1, color.data());
                    float pixelX = (float)windowWidth / 2.0f + tria.position.x;
                    float pixelY = (float)height - (float)windowHeight / 2.0f + tria.position.y;

                    glUniform2f(
                        position_loc,
                        // NDC is -1 ~ 1, so I doubled it
                        2.0f * pixelX / (float)width - 1.0f,
                        2.0f * pixelY / (float)height - 1.0f
                    );
                    glUniform2f(
                        scale_loc,
                        (float)windowHeight / (float)width,
                        (float)windowHeight / (float)height
                    );

                    glBindVertexArray(tria.vao_id);
                    glDrawArrays(GL_TRIANGLES, 0, 3);
                }

                break;
            }

            default: {
                if (!warnOnce) {
                    print("Warning: Unknown draw mode recieved!\n");
                    warnOnce = true;
                }
                break;
            }
        }

        glfwSwapBuffers(window);
        return glfwWindowShouldClose(window);
    }

    ~Renderer() {
        for (const auto& tria : triangles) {
            glDeleteBuffers(1, &tria.vbo_id);
            glDeleteVertexArrays(1, &tria.vao_id);
        }

        if (program != 0) {
            glDeleteProgram(program); 
        }

        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();

        print("Renderer has destructed!\n");
    }
};

}