#define DEBUG 1

#include <time.h>
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../generated/spinning_cube_vert.h"
#include "../generated/spinning_cube_frag.h"
#include <fix_render_on_mac.h>
#include <chrono>

#ifdef DEBUG
// for reading the shader from disk
#include <fstream>
#endif

#define WIDTH 800
#define HEIGHT 600


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int, char **)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Spinning Cube", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float vertices[] = {
       -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       -1.0f, 1.0f, 0.0f,

       -1.0f, 1.0f, 0.0f,
       1.0f, 1.0f, 0.0f,
       1.0f, -1.0f, 0.0f
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    #ifdef DEBUG
    std::string line, vertex_source;
    std::ifstream in_vert("src/spinning_cube_vert.glsl");
    while(std::getline(in_vert, line))
        vertex_source += line + "\n";
    const char* vertex_source_c_str = vertex_source.c_str();
    glShaderSource(vertex_shader, 1, &vertex_source_c_str, NULL);
    #else
    glShaderSource(vertex_shader, 1, &spinning_cube_vertShader, NULL);
    #endif
    glCompileShader(vertex_shader);
    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cerr << "Vertex shader error: " << info_log << std::endl;
        std::cerr << spinning_cube_vertShader << std::endl;
    }

    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    #ifdef DEBUG
    std::string fragment_source;
    std::ifstream in_frag("src/spinning_cube_frag.glsl");
    while(std::getline(in_frag, line))
        fragment_source += line + "\n";
    const char* fragment_source_c_str = fragment_source.c_str();
    glShaderSource(fragment_shader, 1, &fragment_source_c_str, NULL);
    #else
    glShaderSource(fragment_shader, 1, &spinning_cube_fragShader, NULL);
    #endif
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cerr << "Fragment shader error: " << info_log << std::endl;
        std::cerr << spinning_cube_fragShader << std::endl;
    }

    unsigned int shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        std::cerr << "Program error: " << info_log << std::endl;
    }

    glUseProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  


    auto start_time = std::chrono::high_resolution_clock::now();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);

        auto curr_time = std::chrono::high_resolution_clock::now();
        double play_time_s = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - start_time).count() * 1e-9;
        unsigned int iTime = glGetUniformLocation(shader_program, "iTime");
        glUniform1f(iTime, play_time_s);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        auto iResolution = glGetUniformLocation(shader_program, "iResolution");
        glUniform2f(iResolution, width, height);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
        fix_render_on_mac(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
