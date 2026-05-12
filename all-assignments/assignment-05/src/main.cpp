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
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "uniform vec4 triangleColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = triangleColor;\n"
                                   "}\n\0";

// State variables
bool isRedPermanently = false;
float animationStartTime = 0.0f;
float timeOffset = 0.0f;

// --- USER INFORMATION ---
// Replace these with your actual ID and the first letter of your name
const char *STUDENT_ID = "1016";
const int CLOSE_WINDOW_KEY =
    GLFW_KEY_ESCAPE; // Replace with GLFW_KEY_<YOUR_INITIAL> (e.g. GLFW_KEY_A)
// ------------------------

int main() {
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, STUDENT_ID, NULL, NULL);
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
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // link shaders
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // set up vertex data (and buffer(s)) and configure vertex attributes
  float vertices[] = {
      -0.5f, -0.5f, 0.0f, // left
      0.5f,  -0.5f, 0.0f, // right
      0.0f,  0.5f,  0.0f  // top
  };

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    // input
    processInput(window);

    // render
    glClearColor(0.1f, 0.1f, 0.1f,
                 1.0f); // Dark background to see colors better
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader program
    glUseProgram(shaderProgram);

    // Color Logic
    float r = 0.0f, g = 0.0f, b = 0.0f;

    if (isRedPermanently) {
      // Rule 2b: Triangle turns red permanently, animation stops
      r = 1.0f;
      g = 0.0f;
      b = 0.0f;
    } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      // Rule 2a: Triangle turns white while W is held down
      r = 1.0f;
      g = 1.0f;
      b = 1.0f;
      // Capture time to maintain animation continuity when released
      timeOffset = glfwGetTime();
    } else {
      // Rule 1: Initially colored cyan (0,1,1), animates to magenta (1,0,1)
      float timeValue = glfwGetTime();
      // Use sin to interpolate between 0 and 1
      float t = (sin(timeValue) + 1.0f) / 2.0f;

      // Cyan: (0, 1, 1), Magenta: (1, 0, 1)
      // R: 0 -> 1 (t)
      // G: 1 -> 0 (1-t)
      // B: 1 -> 1 (always 1)
      r = t;
      g = 1.0f - t;
      b = 1.0f;
    }

    int vertexColorLocation =
        glGetUniformLocation(shaderProgram, "triangleColor");
    glUniform4f(vertexColorLocation, r, g, b, 1.0f);

    // draw triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // glfw: swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  // glfw: terminate
  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  // Rule 3b: Press the first letter of your name -> closes the window
  // Rule 3b fallback: ESC also closes for safety
  if (glfwGetKey(window, CLOSE_WINDOW_KEY) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // Rule 2b: Press R -> Triangle turns red permanently
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    isRedPermanently = true;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
