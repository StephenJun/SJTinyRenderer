#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
//一个基础的GLSL顶点着色器的源代码
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//一个基础的GLSL片元着色器的源代码
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SJTinyRenderer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //创建并且编译shader程序
    //----------------
    //顶点着色器
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    //检查可能出现的shader编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEXT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //片元着色器
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    //检查可能出现的shader编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //链接两个shader
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    //检查可能出现的链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    //删除shader的目的是释放内存？
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //配置顶点数据（和缓存）和配置顶点属性
    //------------------------------------
    float vertices[] = {
         -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };//此处坐标都是标准化设备坐标，凡是经过了顶点着色器处理过的，应该就是NDC了

    unsigned int VBO, VAO;
    //这个参数1使我们为缓冲配置的独一无二的ID
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    //绑定到GL_ARRAY_BUFFER目标上之后，我们使用的任何缓冲调用都会用来配置当前绑定的缓冲VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //下列调用会将之前定义的顶点数据复制到缓冲的内存当中去
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //GL_STATIC_DRAW ：数据不会或几乎不会改变。
    //GL_DYNAMIC_DRAW：数据会被改变很多。
    //GL_STREAM_DRAW ：数据每次绘制时都会改变。

    //使用glVertexAttribPointer函数告诉OpenGL该如何解析顶点数据（应用到逐个顶点属性上）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //注意，这是允许的，对glVertexAttribPointer的调用将VBO注册为顶点属性的绑定顶点缓冲区对象，
    //这样之后我们可以安全地解除绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //可以在以后解除绑定VAO，这样其他VAO调用就不会意外地修改这个VAO，但这种情况很少发生。
    //修改其他VAOs无论如何都需要调用glBindVertexArray，所以我们通常不会在没有直接必要时解除VAOs（或VBOs）的绑定。
    glBindVertexArray(0);

    //取消注释下列调用可以在线框模式下绘制。
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        //渲染
        //--------
        glClearColor(0.2f, 0.5f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //绘制三角形
        glUseProgram(shaderProgram);
        //因为我们只有一个VAO，所以没有必要每次都绑定它，但是我们这样做是为了让事情更有条理
        glBindVertexArray(VAO);
        //第二个参数指定了顶点数组的起始索引。
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // 就是说假如我们一帧内需要绘制多个VAO，就需要对每个VAO在绘制时绑定，结束时解除绑定？
        // glBindVertexArray(0); 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //可选项：当所有资源超出其用途时取消分配：
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}