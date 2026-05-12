#include "glad.h"
#include "glfw3.h"

#include <cmath>
#include <iostream>

// Function prototypes
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Shaders
const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform float time;\n"
    "void main()\n"
    "{\n"
    "   // Scaling factor (Magnification between 0.5 and 1.5)\n"
    "   float scale = 1.0 + sin(time) * 0.5;\n"
    "   \n"
    "   // Rotation angle\n"
    "   float angle = time;\n"
    "   float s = sin(angle);\n"
    "   float c = cos(angle);\n"
    "   \n"
    "   // Rotation Matrix (XY plane)\n"
    "   mat4 rotation = mat4(\n"
    "       c,   s, 0.0, 0.0,\n"
    "      -s,   c, 0.0, 0.0,\n"
    "      0.0, 0.0, 1.0, 0.0,\n"
    "      0.0, 0.0, 0.0, 1.0\n"
    "   );\n"
    "   \n"
    "   // Scaling Matrix\n"
    "   mat4 scaling = mat4(\n"
    "       scale, 0.0,   0.0,   0.0,\n"
    "       0.0,   scale, 0.0,   0.0,\n"
    "       0.0,   0.0,   scale, 0.0,\n"
    "       0.0,   0.0,   0.0,   1.0\n"
    "   );\n"
    "   \n"
    "   gl_Position = scaling * rotation * vec4(aPos, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "uniform vec4 rectColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = rectColor;\n"
                                   "}\n\0";

// --- USER INFORMATION ---
const char *STUDENT_ID = "1116";
// ------------------------

int main() {
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                       "Assignment 05 -Sabikun nahar Alina- 1016", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // build and compile our shader program
  // vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // link shaders
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // set up vertex data (rectangle)
  float vertices[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    // input
    processInput(window);

    // render
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader program
    glUseProgram(shaderProgram);

    float timeValue = glfwGetTime();

    // Update time uniform in vertex shader
    int timeLocation = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(timeLocation, timeValue);

    // Color Logic: Red (1,0,0) to White (1,1,1)
    float t = (sin(timeValue) + 1.0f) / 2.0f; // Interpolation factor
    float r = 1.0f;
    float g = t;
    float b = t;

    int colorLocation = glGetUniformLocation(shaderProgram, "rectColor");
    glUniform4f(colorLocation, r, g, b, 1.0f);

    // draw rectangle
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glfw: swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  // glfw: terminate
  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}