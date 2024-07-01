// Copyright (C) 2021 Emilio J. Padrón
// Released as Free Software under the X11 License
// https://spdx.org/licenses/X11.html
//
// Strongly inspired by spinnycube.cpp in OpenGL Superbible
// https://github.com/openglsuperbible

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// GLM library to deal with matrix operations
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::perspective
#include <glm/gtc/type_ptr.hpp>

// stb_image for texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int gl_width = 640;
int gl_height = 480;

void glfw_window_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void render(double);

GLuint shader_program = 0; // shader program to set render pipeline
GLuint vao = 0; // Vertext Array Object to set input data
GLuint texture = 0; // Texture ID
GLint mv_location, proj_location, texture_location; // Uniforms for transformation matrices and texture

GLuint loadTexture(const char* filename);

int main() {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  GLFWwindow* window = glfwCreateWindow(gl_width, gl_height, "My spinning cube", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwSetWindowSizeCallback(window, glfw_window_size_callback);
  glfwMakeContextCurrent(window);

  glewInit();

  // get version info
  const GLubyte* vendor = glGetString(GL_VENDOR); // get vendor string
  const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte* glversion = glGetString(GL_VERSION); // version as a string
  const GLubyte* glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION); // version as a string
  printf("Vendor: %s\n", vendor);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", glversion);
  printf("GLSL version supported %s\n", glslversion);
  printf("Starting viewport: (width: %d, height: %d)\n", gl_width, gl_height);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); // set a smaller value as "closer"

  // Vertex Shader
  const char* vertex_shader =
    "#version 130\n"

    "in vec4 v_pos;"
    "in vec2 v_tex_coord;"

    "out vec2 vs_tex_coord;"
    "out vec4 vs_color;"

    "uniform mat4 mv_matrix;"
    "uniform mat4 proj_matrix;"

    "void main() {"
    "  gl_Position = proj_matrix * mv_matrix * v_pos;"
    "  vs_color = v_pos * 2.0 + vec4(0.4, 0.4, 0.4, 0.0);"
    "  vs_tex_coord = v_tex_coord;"
    "}";


  // Fragment Shader
  const char* fragment_shader =
    "#version 130\n"

    "in vec4 vs_color;"
    "in vec2 vs_tex_coord;"

    "out vec4 frag_col;"

    "uniform sampler2D texture;"

    "void main() {"
    "  if (vs_tex_coord.x == 0.0 && vs_tex_coord.y == 0.0) {"
    "    frag_col = vs_color;"
    "  } else {"
    "    frag_col = texture2D(texture, vs_tex_coord);"
    "  }"
    "}";

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);

  shader_program = glCreateProgram();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Cube vertices with texture coordinates
  const GLfloat vertex_positions[] = {
    // Positions         // Texture Coords
    -0.25f, -0.25f, -0.25f,  0.0f, 0.0f,
    -0.25f,  0.25f, -0.25f,  0.0f, 0.0f,
     0.25f, -0.25f, -0.25f,  0.0f, 0.0f,
     0.25f,  0.25f, -0.25f,  0.0f, 0.0f,
     0.25f, -0.25f, -0.25f,  0.0f, 0.0f,
    -0.25f,  0.25f, -0.25f,  0.0f, 0.0f,

     0.25f, -0.25f, -0.25f,  0.0f, 0.0f,
     0.25f,  0.25f, -0.25f,  0.0f, 0.0f,
     0.25f, -0.25f,  0.25f,  0.0f, 0.0f,
     0.25f,  0.25f,  0.25f,  0.0f, 0.0f,
     0.25f, -0.25f,  0.25f,  0.0f, 0.0f,
     0.25f,  0.25f, -0.25f,  0.0f, 0.0f,

     0.25f, -0.25f,  0.25f,  0.0f, 0.0f,
     0.25f,  0.25f,  0.25f,  0.0f, 0.0f,
    -0.25f, -0.25f,  0.25f,  0.0f, 0.0f,
    -0.25f,  0.25f,  0.25f,  0.0f, 0.0f,
    -0.25f, -0.25f,  0.25f,  0.0f, 0.0f,
     0.25f,  0.25f,  0.25f,  0.0f, 0.0f,

    -0.25f, -0.25f,  0.25f,  0.0f, 0.0f,
    -0.25f,  0.25f,  0.25f,  0.0f, 1.0f,
    -0.25f, -0.25f, -0.25f,  1.0f, 0.0f,
    -0.25f,  0.25f, -0.25f,  1.0f, 1.0f,
    -0.25f, -0.25f, -0.25f,  1.0f, 0.0f,
    -0.25f,  0.25f,  0.25f,  0.0f, 1.0f,

     0.25f, -0.25f, -0.25f,  0.0f, 0.0f,
     0.25f, -0.25f,  0.25f,  0.0f, 0.0f,
    -0.25f, -0.25f, -0.25f,  0.0f, 0.0f,
    -0.25f, -0.25f,  0.25f,  0.0f, 0.0f,
    -0.25f, -0.25f, -0.25f,  0.0f, 0.0f,
     0.25f, -0.25f,  0.25f,  0.0f, 0.0f,

     0.25f,  0.25f,  0.25f,  0.0f, 0.0f,
     0.25f,  0.25f, -0.25f,  0.0f, 0.0f,
    -0.25f,  0.25f,  0.25f,  0.0f, 0.0f,
    -0.25f,  0.25f, -0.25f,  0.0f, 0.0f,
    -0.25f,  0.25f,  0.25f,  0.0f, 0.0f,
     0.25f,  0.25f, -0.25f,  0.0f, 0.0f
  };

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);

  // Vertex attributes
  // 0: vertex position (x, y, z)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(0);

  // 1: texture coordinates (u, v)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  mv_location = glGetUniformLocation(shader_program, "mv_matrix");
  proj_location = glGetUniformLocation(shader_program, "proj_matrix");
  texture_location = glGetUniformLocation(shader_program, "texture");

  // Load texture
  texture = loadTexture("texture.jpg");

  while(!glfwWindowShouldClose(window)) {
    processInput(window);
    render(glfwGetTime());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

GLuint loadTexture(const char* filename) {
  int width, height, nrChannels;
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (!data) {
    fprintf(stderr, "Failed to load texture\n");
    return 0;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  return texture;
}

void render(double currentTime) {
  float f = (float)currentTime * 0.3f;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, gl_width, gl_height);

  glUseProgram(shader_program);
  glBindVertexArray(vao);

  glm::mat4 mv_matrix, proj_matrix;

  mv_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.0f, -4.0f));
  mv_matrix = glm::translate(mv_matrix,
                             glm::vec3(sinf(2.1f * f) * 0.5f,
                                       cosf(1.7f * f) * 0.5f,
                                       sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));

  mv_matrix = glm::rotate(mv_matrix,
                          glm::radians((float)currentTime * 45.0f),
                          glm::vec3(0.0f, 1.0f, 0.0f));
  mv_matrix = glm::rotate(mv_matrix,
                          glm::radians((float)currentTime * 81.0f),
                          glm::vec3(1.0f, 0.0f, 0.0f));

  glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));

  proj_matrix = glm::perspective(glm::radians(50.0f),
                                 (float) gl_width / (float) gl_height,
                                 0.1f, 1000.0f);
  glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(texture_location, 0);

  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
}

void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
  gl_width = width;
  gl_height = height;
  printf("New viewport: (width: %d, height: %d)\n", width, height);
}
