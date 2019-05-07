#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 fColor;\n"
"\n"
"void main()\n"
"{\n"
"    FragColor = vec4(fColor, 1.0);   \n"
"}\n";

const char *geometryShaderSource = "#version 330 core\n"
"layout (points) in;\n"
"layout (triangle_strip, max_vertices = 4) out;\n"
"\n"
"in VS_OUT {\n"
"    vec3 color;\n"
"} gs_in[];\n"
"\n"
"out vec3 fColor;\n"
"\n"
"void build_house(vec4 position)\n"
"{    \n"
"    fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex\n"
"    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0); // 1:bottom-left   \n"
"    EmitVertex();   \n"
"    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0); // 2:bottom-right\n"
"    EmitVertex();\n"
"    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0); // 3:top-left\n"
"    EmitVertex();\n"
"    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0); // 4:top-right\n"
"    fColor = vec3(1.0, 1.0, 1.0);\n"
"    EmitVertex();\n"
"    EndPrimitive();\n"
"}\n"
"\n"
"void main() {    \n"
"    build_house(gl_in[0].gl_Position);\n"
"}\n";

using namespace std;

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

  // build and compile shaders
  // -------------------------
  //9.1.geometry_shader.vs", "9.1.geometry_shader.fs", "9.1.geometry_shader.gs"
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

  // link shaders
  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glAttachShader(shaderProgram, geometryShader);
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

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  int voxels_per_column = 4;
  int voxels_per_row = 4;
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

  //float points[] = {
  //    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, // top-left
  //     0.5f,  0.5f, 0.0f, 0.0f, 1.0f, // top-right
  //     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
  //    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // bottom-left
  //};

  float *points = p_vec.data();

  unsigned int VBO, VAO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * p_vec.size(), points, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
  glBindVertexArray(0);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw points
    //shader.use();
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, p_vec.size());

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
