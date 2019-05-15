#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "ice.h"

bool pause_sim = false;
bool reset_sim = false;
//bool rightly = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

int voxels_per_column = 20;
int voxels_per_row = 20;
int voxel_thicc = 20;
float voxel_edge_length = 1;

float initial_temperature = 270;
float ambient_temerature = 274;
float steps_per_frame = 1;
float timestep = 1.0f / 60.0f / steps_per_frame;

float camera_theta = 30;
float camera_phi = 30;
float camera_dist = 6;
float rotation_speed = 0.3f;
float zoom_speed = 0.1f;

bool mousePressed;
double lastX, lastY;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec4 aColor;\n"
"\n"
"out VS_OUT {\n"
"    vec4 color;\n"
"} vs_out;\n"
"\n"
"void main()\n"
"{\n"
"    vs_out.color = aColor;\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); \n"
"}\n";

const char *geometryShaderSource = "#version 330 core\n"
"layout (points) in;\n"
"layout (triangle_strip, max_vertices = 16) out;\n"
"\n"
"in VS_OUT {\n"
"    vec4 color;\n"
"} gs_in[];\n"
"\n"
"out vec4 fColor;\n"
"out vec3 normal;\n"
"out vec3 FragPos;\n"
"\n"
"uniform mat4 MVP;\n"
"uniform mat4 model;\n"
"\n"
"void build_cube(vec4 position, vec4 color)\n"
"{\n"
"    normal = vec3(0, 0, 1);\n"
"    fColor = color;\n"
"    gl_Position = MVP * (position + vec4(-0.5, -0.5,  0.5, 0.0)); // front face\n"
"    FragPos = vec3(model * (position + vec4(-0.5, -0.5,  0.5, 0.0)));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.5, -0.5,  0.5, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4(-0.5,  0.5,  0.5, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.5,  0.5,  0.5, 0.0));\n"
"    EmitVertex();\n"
"    normal = vec3(0, 1, 0);\n"
"    gl_Position = MVP * (position + vec4(-0.5,  0.5, -0.5, 0.0)); // top face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.5,  0.5, -0.5, 0.0));\n"
"    EmitVertex();\n"
"    normal = vec3(0, 0, -1);\n"
"    gl_Position = MVP * (position + vec4(-0.5, -0.5, -0.5, 0.0)); // back face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.5, -0.5, -0.5, 0.0));\n"
"    EmitVertex();\n"
"    EndPrimitive();\n"
"\n"
"    normal = vec3(-1, 0, 0);\n"
"    gl_Position = MVP * (position + vec4(-0.5,  0.5,  0.5, 0.0)); // left face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4(-0.5,  0.5, -0.5, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4(-0.5, -0.5,  0.5, 0.0));\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4(-0.5, -0.5, -0.5, 0.0));\n"
"    EmitVertex();\n"
"    normal = vec3(0, -1, 0);\n"
"    gl_Position = MVP * (position + vec4( 0.5, -0.5,  0.5, 0.0)); // bottom face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.5, -0.5, -0.5, 0.0));\n"
"    EmitVertex();\n"
"    normal = vec3(1, 0, 0);\n"
"    gl_Position = MVP * (position + vec4( 0.5,  0.5,  0.5, 0.0)); // right face\n"
"    EmitVertex();\n"
"    gl_Position = MVP * (position + vec4( 0.5,  0.5, -0.5, 0.0));\n"
"    EmitVertex();\n"
"    EndPrimitive();\n"
"}\n"
"\n"
"void main() {    \n"
"    build_cube(gl_in[0].gl_Position, gs_in[0].color);\n"
"}\n";

const char *fragmentShaderSource = "#version 330 core\n"
"in vec3 normal;\n"
"in vec3 FragPos;\n"
"out vec4 FragColor;\n"
"\n"
"in vec4 fColor;\n"
"\n"
"void main()\n"
"{\n"
"    vec3 norm = normalize(normal);\n"
"    vec3 lightDir = normalize(vec3(0, 0, 3) - FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec4 diffuse = vec4(diff * vec3(1, 1, 1), 1);\n"
"    FragColor = diffuse * fColor;\n"
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

// Extraction of voxel information from ice struct
vector<float> ice_to_voxels(vector<float>& voxel_data, Ice& ice) {
  voxel_data.clear();
	vector<IceVoxel>& ice_voxels = ice.ice_voxels;
	for (int z = 0; z < voxel_thicc; z++) {
		for (int y = 0; y < voxels_per_column; y++) {
			for (int x = 0; x < voxels_per_row; x++) {
				int index = x + y * voxels_per_row + z * voxels_per_row * voxels_per_column;
        if (ice_voxels[index].state == 0) continue;
				voxel_data.push_back((float)x);
				voxel_data.push_back((float)y);
				voxel_data.push_back((float)z);
				voxel_data.push_back(63.0f/255); //R
				voxel_data.push_back(208.0f/255); //G
				voxel_data.push_back(212.0f/255); //B
        voxel_data.push_back(ice_voxels[index].state); //A
			}
		}
	}
	return voxel_data;
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
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetKeyCallback(window, key_callback);


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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // build and compile shaders
  // -------------------------
  int shaderProgram = compileShaders();

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  int num_voxels = voxels_per_column * voxels_per_row * voxel_thicc;

  unsigned int VBO, VAO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
  glBindVertexArray(0);

  // generate model matrix
  // ---------------------
  // Model matrix
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::scale(model,
    glm::vec3(2.0f / glm::max(glm::max(voxels_per_row, voxels_per_column), voxel_thicc)));
  model = glm::translate(model, glm::vec3((-voxels_per_row + 1) / 2.0f,
                                          (-voxels_per_column + 1) / 2.0f,
                                          (-voxel_thicc + 1) / 2.0f));

  // initialize ice simulation
  // ---------------------
  Ice ice = Ice(voxel_edge_length, 
    voxels_per_row, 
    voxels_per_column, 
    voxel_thicc,
    initial_temperature,
    ambient_temerature,
    timestep);

	vector<float> voxel_data = vector<float>();

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    
    // run ice simulation
    // ------------------
    
    for (int i = 0; i < steps_per_frame; i++) {
      if (!pause_sim)
        ice.simulate();
      if (reset_sim) {
          ice.reset();
          reset_sim = false;
      }
      /*
      if (rightly) {
          //ice.simulate();
          rightly = !rightly;
      }
      */
    }
    

    // generate view and projection matrices
    // -------------------------------------
    // Compute camera position
    float theta = glm::radians(camera_theta);
    float phi = glm::radians(camera_phi);
    float cos_phi = cos(phi);
    glm::vec3 camera_pos = glm::vec3(camera_dist * cos_phi * sin(theta),
                                     camera_dist * sin(phi),
                                     camera_dist * cos_phi * cos(theta));

    // Camera matrix
    glm::mat4 view = glm::lookAt(
      camera_pos, // Camera is at (4,3,3), in World Space
      glm::vec3(0, 0, 0), // and looks at the origin
      glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
      );

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Compute MVP matrix and send to shader
    glm::mat4 MVP = projection * view * model;

    // render
    // ------
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw points
    ice_to_voxels(voxel_data, ice);
    glBufferData(GL_ARRAY_BUFFER, voxel_data.size() * sizeof(float), voxel_data.data(), GL_STATIC_DRAW);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, voxel_data.size() / 7);

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    pause_sim = !pause_sim;
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    //replay 
    pause_sim = true;
    reset_sim = true;
  }

  /*
  
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    //replay 
    pause_sim = true;
    rightly = true;
  }
  */
    

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  SCR_WIDTH = width;
  SCR_HEIGHT = height;
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
  float xoffset = (float)(xpos - lastX);
  float yoffset = (float)(lastY - ypos); // reversed since y-coordinates go from bottom to top

  if (mousePressed) {
    camera_theta -= rotation_speed * xoffset;
    camera_phi -= rotation_speed * yoffset;
    if (abs(camera_phi) >= 90) camera_phi = signbit(camera_phi) ? -89.999f : 89.999f;
  }

  lastX = xpos;
  lastY = ypos;
}

// glfw: whenever a mouse button is pressed or released, this callback is called
// ---------------------------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    mousePressed = (action == GLFW_PRESS);
  }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera_dist = glm::max(0.0f, camera_dist - zoom_speed * (float)yoffset);
}
