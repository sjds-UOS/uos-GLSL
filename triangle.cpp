#include <cmath>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLuint WIDTH = 800, HEIGHT = 600;

const GLchar *vertexShaderSource = R"glsl(
    #version 450

    layout(location = 0) in vec3 inPosition;
    layout(location = 1) in vec3 inColor;

    out vec3 outColor;

    void main()
    {
        gl_Position = vec4(inPosition, 1.0);
        outColor = inColor;
    }
)glsl";

const GLchar *fragmentShaderSource = R"glsl(
    #version 450

    in vec3 outColor;

    out vec4 fragColor;

    void main()
    {
        fragColor = vec4(outColor, 1.0);
    }
)glsl";

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL 4.5", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // 初始化GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // 创建顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // 创建片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // 创建着色器程序
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 设置顶点属性指针
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f};
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // 设置清屏颜色并启用深度测试
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // 注册键盘回调函数
    glfwSetKeyCallback(window, key_callback);

    // 进入主循环
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // 清空缓冲区
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制三角形
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // 交换缓冲区
        glfwSwapBuffers(window);
    }

    // 删除顶点缓存和顶点数组对象
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // 删除着色器程序
    glDeleteProgram(shaderProgram);

    // 关闭GLFW窗口并退出
    glfwTerminate();
    return 0;
}
