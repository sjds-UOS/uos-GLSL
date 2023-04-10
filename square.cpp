#include <SDL/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

// 设置屏幕宽高为800，位深为32
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int SCREEN_BPP = 32;

// SDL_Surface指针变量screen，用于表示屏幕
SDL_Surface *screen;

// 标志变量，表示窗口是否处于窗口模式下
bool windowed;

// 标志变量，表示窗口是否正常打开
bool windowOK;

// GLSL程序句柄
GLuint programHandle;

// 顶点着色器和片段着色器的句柄
GLuint vShader;
GLuint fShader;

// 顶点数组对象和顶点缓冲区对象
GLuint vao, vbo[2];

/**
 * 退出程序
 * 结束SDL窗口进程
 * @param code 退出状态码
 */
void quit(int code)
{
    SDL_Quit();
    exit(code);
}

/**
 * 读取文件内容
 * @param   fn  文件名
 * @return  字符串指针
 */
char *textFileRead(char *fn)
{
    FILE *fp;
    char *content = NULL;
    int count = 0;
    if (fn != NULL)
    {
        fp = fopen(fn, "rt");
        if (fp != NULL)
        {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);
            if (count > 0)
            {
                content = (char *)malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

/**
 * 切换全屏状态
 */
void toggle_fullscreen()
{
    // 如果当前窗口处于窗口模式
    if (windowed == true)
    {
        // 将窗口设置为全屏模式
        screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN);

        // 如果设置失败，则将窗口标志设置为异常状态，函数结束
        if (screen == NULL)
        {
            windowOK = false;
            return;
        }

        // 设置窗口状态标志
        windowed = false;
    }
    // 如果当前窗口处于全屏模式
    else if (windowed == false)
    {
        // 将窗口恢复为窗口模式
        screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL | SDL_RESIZABLE);

        // 如果设置失败，则将窗口标志设置为异常状态，函数结束
        if (screen == NULL)
        {
            windowOK = false;
            return;
        }

        // 设置窗口状态标志
        windowed = true;
    }
}

/**
 * 处理键盘事件的函数
 * 对输入的按键进行不同的处理
 */
void handleKeyEvent(SDL_keysym *keysym)
{
    switch (keysym->sym)
    {
    case SDLK_ESCAPE: // 如果是ESC键，则退出程序
        quit(0);      // 调用quit函数退出程序
        break;
    case SDLK_SPACE: // 如果是空格键，则不做任何操作
        break;
    case SDLK_F1:            // 如果是F1键，则切换全屏或窗口模式
        toggle_fullscreen(); // 调用toggle_fullscreen函数
        break;
    default: // 如果是其他按键，则不做任何操作
        break;
    }
}

/**
 * 控制OpenGL窗口大小的函数
 * 实现窗口大小调整后的画面显示
 * @param   width   窗口宽度
 * @param   height  窗口高度
 */
void resizeGL(int width, int height)
{
    if (height == 0) // 如果当前高度值为0，则将其设置为1，避免除数为0的情况
    {
        height = 1;
    }
    // 重置视图区域
    glViewport(0, 0, (GLint)width, (GLint)height);
    // 选择矩阵模式
    glMatrixMode(GL_PROJECTION);
    // 重置投影矩阵
    glLoadIdentity();
    // 设置透视投影
    gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0);
    // 选择矩阵模式
    glMatrixMode(GL_MODELVIEW);
    // 重置模型视图矩阵
    glLoadIdentity();
}

/**
 * 处理SDL窗口事件
 */
void handleEvents()
{
    // 创建SDL事件变量
    SDL_Event event;
    // 取出事件队列中的所有事件
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            // 处理键盘事件，调用handleKeyEvent函数
            handleKeyEvent(&event.key.keysym);
            break;
        case SDL_QUIT:
            // 处理退出事件，调用quit函数
            quit(0);
            break;
        case SDL_VIDEORESIZE:
            // 处理窗口大小调整事件
            // 重新设置屏幕参数
            screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16, SDL_OPENGL | SDL_RESIZABLE);
            // 如果成功设置屏幕参数，则进行OpenGL窗口大小调整
            if (screen)
            {
                resizeGL(screen->w, screen->h);
            }
            break;
        }
    }
}

/**
 * 初始化SDL系统
 * @param   width   窗口宽度
 * @param   height  窗口高度
 */
void initSDL(int width, int height, int bpp, int flags)
{
    // 首先初始化SDL的视频子系统
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Video initialization failed: %s\n",
                SDL_GetError());
        quit(1);
    }
    // 注册退出SDL的函数
    atexit(SDL_Quit);

    // 设置OpenGL在SDL中的一些属性
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // 设置视频模式
    screen = SDL_SetVideoMode(width, height, bpp, flags);
    // 如果设置模式失败则提示错误信息并退出程序，否则设置窗口为windowed模式
    if (!screen)
    {
        fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
        quit(1);
        windowed = false;
    }
    else
        windowed = true;

    // 调整OpenGL窗口大小
    resizeGL(screen->w, screen->h);
    // 设置窗口标题
    SDL_WM_SetCaption("OpenGL Shading Language Test", NULL);
}

/**
 * 初始化着色器
 * 创建顶点数组对象和顶点缓冲对象
 * 并将顶点数据传递到顶点缓冲对象中
 */
void initShader()
{
    // 初始化顶点数组对象(VAO)和顶点缓冲对象(VBO)

    // 创建一个由线条组成的简单菱形

    const GLfloat diamond[4][2] = {
        {0.0, 1.0},   /* 上点 */
        {1.0, 0.0},   /* 右点 */
        {0.0, -1.0},  /* 下点 */
        {-1.0, 0.0}}; /* 左点 */

    const GLfloat colors[4][3] = {
        {1.0, 0.0, 0.0},  /* 红 */
        {0.0, 1.0, 0.0},  /* 绿 */
        {0.0, 0.0, 1.0},  /* 蓝 */
        {1.0, 1.0, 1.0}}; /* 白 */

    /* 定义指向顶点着色器和片段着色器源代码的指针 */
    GLchar *vertexsource, *fragmentsource;

    /* 定义用于引用着色器的句柄 */
    GLuint vertexshader, fragmentshader;

    /* 定义用于引用着色器程序的句柄 */
    GLuint shaderprogram;

    /* 分配并指定顶点数组对象的句柄 */
    glGenVertexArrays(1, &vao);

    /* 绑定顶点数组对象为当前的对象 */
    glBindVertexArray(vao);

    /* 分配并指定两个顶点缓冲对象的句柄 */
    glGenBuffers(2, vbo);

    /* 将第一个顶点缓冲绑定为当前的活动缓冲，并存储顶点数据 */
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), diamond, GL_STATIC_DRAW);

    /* 指定坐标数据存储在索引为0的位置，并且每个顶点包含两个浮点型数值 */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    /* 启用索引为0的位置的属性 */
    glEnableVertexAttribArray(0);

    /* 将第二个顶点缓冲绑定为当前的活动缓冲，并存储颜色数据 */
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

    /* 指定颜色数据存储在索引为1的位置，并且每个顶点包含三个浮点型数值 */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /* 启用索引为1的位置的属性 */
    glEnableVertexAttribArray(1);

    // 创建并编译顶点着色器和片段着色器
    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);

    GLchar *vShaderCode = textFileRead("square.vert");
    GLchar *fShaderCode = textFileRead("square.frag");
    const GLchar *vCodeArray[1] = {vShaderCode};
    const GLchar *fCodeArray[1] = {fShaderCode};

    /* 将着色器源代码和句柄关联 */
    glShaderSource(vShader, 1, vCodeArray, NULL);
    glShaderSource(fShader, 1, fCodeArray, NULL);

    /* 编译着色器程序 */
    glCompileShader(vShader);
    glCompileShader(fShader);

    /* 释放着色器源代码占用的内存空间 */
    free(vShaderCode);
    free(fShaderCode);

    // 检查编译是否成功
    GLint logLen;
    glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLen);
    if (logLen > 0)
    {
        char *log = (char *)malloc(logLen);
        GLsizei written;
        glGetShaderInfoLog(vShader, logLen, &written, log);
        printf("Shader compile error log: %s\n", log);
        free(log);
    }

    // 创建着色器程序并链接顶点着色器和片段着色器
    programHandle = glCreateProgram();
    if (0 == programHandle)
    {
        fprintf(stderr, "Error creating programHandle.\n");
        quit(1);
    }

    // 将顶点着色器和片段着色器附加到着色器程序上
    glAttachShader(programHandle, vShader);
    glAttachShader(programHandle, fShader);

    // 将顶点属性索引绑定到着色器中的标识符
    glBindAttribLocation(programHandle, 0, "in_Position");
    glBindAttribLocation(programHandle, 1, "in_Color");

    // 链接着色器程序并使用它
    glLinkProgram(programHandle);
    // glUseProgram(programHandle);
}

/**
 * 释放着色器和缓冲区资源
 */
void freeShader()
{
    // 禁用顶点属性数组
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // 分离已连接的着色器程序
    glDetachShader(programHandle, fShader);
    glDetachShader(programHandle, vShader);

    // 删除着色器对象
    glDeleteShader(fShader);
    glDeleteShader(vShader);

    // 删除缓冲区对象
    glDeleteBuffers(2, vbo);

    // 删除顶点数组对象
    glDeleteVertexArrays(1, &vao);
}

/*
 * 函数：渲染OpenGL场景
 */
void renderGL()
{
    // 统计帧率所需变量
    static GLint T0 = 0;
    static GLint Frames = 0;

    // 清空颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 设置模型视图矩阵，并将坐标系沿z轴平移，以便观察到绘制的对象
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);

    // 使用着色器程序进行绘制
    glUseProgram(programHandle);
    glDrawArrays(GL_QUADS, 0, 4);
    glUseProgram(0);

    // 交换双缓冲区
    SDL_GL_SwapBuffers();
}

/*
 * 函数：初始化OpenGL环境
 */
void initGL(int width, int height)
{
    // 计算窗口宽高比
    float ratio = (float)width / (float)height;

    // 设定着色模型为Gouraud（平滑）着色
    glShadeModel(GL_SMOOTH);

    // 设定清屏颜色
    glClearColor(0, 0, 0, 0);

    // 设定视口大小
    glViewport(0, 0, width, height);

    // 切换到投影矩阵，并设置视角体
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, ratio, 1.0, 100.0);
}

/**
 * 主程序
 */
int main(int argc, char *argv[])
{
    // 定义窗口标志位，包含OpenGL和可调整大小属性
    int flags = SDL_OPENGL | SDL_RESIZABLE;

    // 初始化SDL，并创建窗口及OpenGL上下文
    initSDL(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, flags);

    // 初始化GLEW，如果失败则退出程序
    if (glewInit() != GLEW_OK)
        exit(EXIT_FAILURE);

    // 初始化顶点着色器程序
    initShader();

    // 设置OpenGL环境
    initGL(SCREEN_WIDTH, SCREEN_HEIGHT);

    // 进入主循环
    while (true)
    {
        /* 处理输入事件 */
        handleEvents();
        /* 绘制OpenGL场景 */
        renderGL();
    }

    // 释放顶点着色器程序
    freeShader();

    // 返回0表示程序执行成功
    return 0;
}