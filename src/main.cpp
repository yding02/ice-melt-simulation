#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"\n"
"out VS_OUT {\n"
"    vec3 color;\n"
"} vs_out;\n"
"\n"
"void main()\n"
"{\n"
"    vs_out.color = aColor;\n"
"    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); \n"
"}\n";

const char *geometryShaderSource = "#version 330 core\n"
"layout (points) in;\n"
"layout (triangle_strip, max_vertices = 16) out;\n"
"\n"
"in VS_OUT {\n"
"    vec3 color;\n"
"} gs_in[];\n"
"\n"
"out vec3 fColor;\n"
"\n"
"uniform mat4 MVP;\n"
"\n"
"void build_cube(vec4 position, vec3 color)\n"
"{\n"
"    fColor = color;\n"
"    gl_Position = MVP * (position + vec4(-0.2, -0.2,  0.2, 0.0)); // front face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.2, -0.2,  0.2, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4(-0.2,  0.2,  0.2, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.2,  0.2,  0.2, 0.0));\n"
"    fColor = vec3(1.0, 1.0, 1.0);\n"
"    EmitVertex();\n"
"    fColor = color;\n"
"    gl_Position = MVP * (position + vec4(-0.2,  0.2, -0.2, 0.0)); // top face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.2,  0.2, -0.2, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4(-0.2, -0.2, -0.2, 0.0)); // back face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.2, -0.2, -0.2, 0.0));\n"
"    EmitVertex();\n"
"    EndPrimitive();\n"
"\n"
"    gl_Position = MVP * (position + vec4(-0.2,  0.2,  0.2, 0.0)); // left face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4(-0.2,  0.2, -0.2, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4(-0.2, -0.2,  0.2, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4(-0.2, -0.2, -0.2, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.2, -0.2,  0.2, 0.0)); // bottom face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.2, -0.2, -0.2, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.2,  0.2,  0.2, 0.0)); // right face\n"
"    fColor = vec3(1.0, 1.0, 1.0);\n"
"    EmitVertex();\n"
"    fColor = color;\n"
"    gl_Position = MVP * (position + vec4( 0.2,  0.2, -0.2, 0.0));\n"
"    EmitVertex();\n"
"    EndPrimitive();\n"
"}\n"
"\n"
"void main() {    \n"
"    build_cube(gl_in[0].gl_Position, gs_in[0].color);\n"
"}\n";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 fColor;\n"
"\n"
"void main()\n"
"{\n"
"    FragColor = vec4(fColor, 1.0);\n"
"}\n";

using namespace std;

int compileShaders() {
   // vertex shader
  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // geometry shader
  int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
  glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
  glCompileShader(geometryShader);
  // check for shader compile errors
  glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // link shaders
  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, geometryShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteShader(geometryShader);

  return shaderProgram;
}

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
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // build and compile shaders
  // -------------------------
  int shaderProgram = compileShaders();

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  int voxels_per_column = 4;
  int voxels_per_row = 4;
  int num_voxels = voxels_per_column * voxels_per_row;

  vector<float> p_vec;
  for (int i = 0; i < voxels_per_row; i++) {
    for (int j = 0; j < voxels_per_column; j++) {
      p_vec.push_back(-0.6 + i * 0.4);
      p_vec.push_back(-0.6 + j * 0.4);
      p_vec.push_back(0.0f);
      p_vec.push_back(0.0f);
      p_vec.push_back(1.0f);
    }
  }
  float *points = p_vec.data();

  unsigned int VBO, VAO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, num_voxels * 5 * sizeof(float), points, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindVertexArray(0);

  // generate view and projection matrices
  // -------------------------------------
  // Model matrix
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3((-voxels_per_row + 1) / 2.0f, (-voxels_per_column + 1) / 2.0f, 0));
  model = glm::scale(model, glm::vec3(0.5f));

  // Camera matrix
  glm::mat4 view = glm::lookAt(
    glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
    glm::vec3(0, 0, 0), // and looks at the origin
    glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

  // Compute MVP matrix and send to shader
  glm::mat4 MVP = projection * view * model;

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw points
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, num_voxels);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();
  return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}
