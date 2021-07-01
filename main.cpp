#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "Buffer.h"
#include "IndexBuffer.h"
#include "stb_image/stb_image.h"
#include "Shader.h"


#define W_WIDTH 1920
#define W_HEIGHT 1080
#define W_WIDTHF 1920.0f
#define W_HEIGHTF 1080.0f

#define SIMPLE 0.0f

#define SKY 100.0f


glm::vec3 cameraPosition = glm::vec3(100.0f, 100.0f, 300.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


float previousTime = 0.0f;
float actualTime;
float deltaT;

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = W_WIDTHF/2.0f;
float lastY = W_HEIGHTF/2.0f;

//float lightX = 0.0f;
//float lightY = W_HEIGHTF/2.0f;
//float lightZ = 0.0f; 


float lightX = -10.0f;
float lightY = -130.0f;
float lightZ = 1940.0f;


static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static void GLCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "OPENGL ERROR: " << error << std::endl;
    }
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
static int loadTexture(const std::string& path);
static int loadHDRTex(const std::string& path);
static int loadIrradianceMap(const std::string& path);
void inputHandle(GLFWwindow* window);
void mouseHandle(GLFWwindow* window, double xpos, double ypos);
void renderCube();
void renderSphere();
void renderQuad();



int main() {

    GLFWwindow* window;{}

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    
  

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "MyGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

    glfwSetCursorPosCallback(window, mouseHandle);                             // call the mouseHandle when a mouse movement event occour

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // in positions is also stored the color for each verticies, the texture coordinate, and a texture index. 
    // Positions store 2 different model (a cube and a pyramid) and a plane.
    // position structure: { x, y, z, r, g, b, alpha, texCordU, texCordV, texIndex}
    // texIndex allow us to select the right texture


    float positions[] = {
                          980.0f, 540.0f,  150.0f,  1.0f, 0.6f, 0.96f, 1.0f, 0.0f, 0.0f, 1.0f + SIMPLE,   //0
                         1280.0f, 540.0f,  150.0f,  1.0f, 0.6f, 0.96f, 1.0f, 1.0f, 0.0f, 1.0f + SIMPLE,  //1
                         1280.0f, 240.0f,  150.0f,  1.0f, 0.6f, 0.96f, 1.0f, 1.0f, 1.0f, 1.0f + SIMPLE,  //2
                          980.0f, 240.0f,  150.0f,  1.0f, 0.6f, 0.96f, 1.0f, 0.0f, 1.0f, 1.0f + SIMPLE, //3
                          980.0f, 240.0f, -150.0f,  1.0f, 0.6f, 0.96f, 1.0f, 1.0f, 0.0f, 1.0f + SIMPLE,  //4
                         1280.0f, 240.0f, -150.0f,  1.0f, 0.6f, 0.96f, 1.0f, 0.0f, 0.0f, 1.0f + SIMPLE,  //5
                          980.0f, 540.0f, -150.0f,  1.0f, 0.6f, 0.96f, 1.0f, 1.0f, 1.0f, 1.0f + SIMPLE,  //6
                         1280.0f, 540.0f, -150.0f,  1.0f, 0.6f, 0.96f, 1.0f, 0.0f, 1.0f, 1.0f + SIMPLE,  //7

                         640.0f,  540.0f,  150.0f,  1.0f, 0.93f, 0.24f, 1.0f, 0.0f, 0.0f, 0.0f + SIMPLE,  //8
                         940.0f,  540.0f,  150.0f,  1.0f, 0.93f, 0.24f, 1.0f, 1.0f, 0.0f, 0.0f + SIMPLE,  //9
                         940.0f,  540.0f, -150.0f,  1.0f, 0.93f, 0.24f, 1.0f, 0.0f, 0.0f, 0.0f + SIMPLE,  //10
                         640.0f,  540.0f, -150.0f,  1.0f, 0.93f, 0.24f, 1.0f, 1.0f, 0.0f, 0.0f + SIMPLE,  //11
                         790.0f,  190.0f,    0.0f,  1.0f, 0.93f, 0.24f, 1.0f, 0.5f, 1.0f, 0.0f + SIMPLE,   //12

                          -640.0f,  540.0f,  4096.0f,  1.0f, 0.93f,  0.88f,  1.0f,  0.0f, 0.0f, 2.0f + SIMPLE,   //13
                          4096.0f,  540.0f,  4096.0f,  1.0f, 0.93f,  0.88f,  1.0f,  4.0f, 0.0f, 2.0f + SIMPLE,   //14
                          4096.0f,  540.0f, -4096.0f,  1.0f, 0.93f,  0.88f,  1.0f,  4.0f, 4.0f, 2.0f + SIMPLE,   //15
                          -640.0f,  540.0f, -4096.0f,  1.0f, 0.93f,  0.88f,  1.0f,  0.0f, 4.0f, 2.0f + SIMPLE   // 16

    };

    unsigned int indices[] = {
                               0, 2, 1,
                               2, 0, 3,
                               6, 7, 4,
                               4, 7, 5,
                               3, 4, 2,
                               2, 4, 5,
                               0, 1, 6,
                               6, 1, 7,
                               1, 2, 7,
                               2, 5, 7,
                               0, 6, 3,
                               3, 6, 4,

                                8,  9, 11,
                               11, 9,  10,
                                8, 12,  9,
                                8, 11, 12,
                               11, 10, 12,
                               10, 9,  12,

                               13, 16, 14,
                               16, 15, 14

                            
    };



 

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    
    Buffer buffer(positions, sizeof(positions), false);                         //generate the vertex buffer
    IndexBuffer index_buffer(indices, sizeof(indices), false);                  //generate the index buffer



 
    //setup the layout of the vertex buffer (slot, size, stride, pointer)

    buffer.setLayout(0, 3, 10*sizeof(float), 0);                                   // positions       
    buffer.setLayout(1, 4, 10*sizeof(float), (const void*)12);                     // color
    buffer.setLayout(2, 2, 10*sizeof(float), (const void*)28);                     // texture coordinates
    buffer.setLayout(3, 1, 10*sizeof(float), (const void*)36);                     // texture index (is used for selecting the right texture)


    glBindVertexArray(0);


//    glm::mat4 proj = glm::ortho(0.0f, W_WIDTHF, W_HEIGHTF, 0.0f, -10000.0f, 10000.0f);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), W_WIDTHF/W_HEIGHTF, 0.1f, 50000.0f);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(250.0, 140.0, -500.0));



    glm::mat4 model2 = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));    // z axis rotation


    glm::mat4 model3 = glm::rotate(model2, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // y axis rotation

    glm::mat4 model4 = glm::rotate(model3, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));   // x axis rotation

    //set up frame buffer for shadow mapping (depth mapping)
    
//    const unsigned int SHADOW_WIDTH = 1024;
//    const unsigned int SHADOW_HEIGHT = 1024;

//    unsigned int depthMapFBO;
//    glGenFramebuffers(1, &depthMapFBO);

    // creating the depth texture

/*    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach texture to frame buffer

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   */

   //set up shader
 

    Shader standard_shader("res/shader/myShader.shader");
    Shader eqToCubeMap_shader("res/shader/eqToCubeMap.shader");
    Shader background_shader("res/shader/background.shader");
    Shader irradianceMap_shader("res/shader/irradianceConvolution_shader.shader");
    Shader prefilter_shader("res/shader/prefilterShader.shader");
    Shader brdf_shader("res/shader/BRDF_Shader.shader");

    background_shader.bind();
    background_shader.setUniform1i("environmentMap", 0);


    // setup frame buffer (pbr)

    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    

    // load HDR environment map

//    unsigned int hdrTexture = loadHDRTex("res/texture/skybox/Mono_Lake_C/Mono_Lake_C_Ref.hdr");
//    unsigned int hdrTexture = loadHDRTex("res/texture/skybox/Newport_Loft/Newport_Loft_Ref.hdr");
    unsigned int hdrTexture = loadHDRTex("res/texture/skybox/MonValley_DirtRoad/MonValley_G_DirtRoad_3k.hdr");

    // load irradiance map

//    unsigned int irradianceMap = loadHDRTex("res/texture/skybox/MonValley_DirtRoad/MonValley_G_DirtRoad_Env.hdr");


    // setup cubemap to render to and attach to framebuffer

    unsigned int envCubeMap;
    glGenTextures(1, &envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);

    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // set up projection and view matrices for capturing data onto the 6 cubemap face directions

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    
    };

    // convert HDR equirectangular environment map to cubemap equivalent

    eqToCubeMap_shader.bind();
    eqToCubeMap_shader.setUniform1i("equirectangularMap", 0);
    eqToCubeMap_shader.setUniformMat4f("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    // binding the irradianceMap
    
    


    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (unsigned int i = 0; i < 6; ++i) {
        
        eqToCubeMap_shader.setUniformMat4f("view", captureViews[i]);

        std::cout << "Color Buffer: " << envCubeMap << std::endl;
        std::cout << "Depth Buffer: " << captureRBO << std::endl;
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubeMap, 0);

        GLCheckError();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        renderCube();
    }  
   

    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)  std::cout << "Error! Framebuffer is not complete! Status: " << fboStatus << std::endl;
    if (fboStatus == GL_FRAMEBUFFER_UNSUPPORTED) std::cout << "Framebuffer implementation not supported! Status: " << fboStatus << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // generating the irradiance map

    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // solve the integral by convolution to create the Irradiance Map

    irradianceMap_shader.bind();
    irradianceMap_shader.setUniform1i("environmentMap", 0);
    irradianceMap_shader.setUniformMat4f("projection", captureProjection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    glViewport(0,0,32,32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {

        irradianceMap_shader.setUniformMat4f("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

//  create pre-filtered cube map, and rescale capture FBO to pre-filtered scale

    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

// run quasi-montecarlo simulation on the environment lighting to create a prefilter cubemap

    prefilter_shader.bind();
    prefilter_shader.setUniform1i("environmentMap", 0);
    prefilter_shader.setUniformMat4f("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {

        unsigned int mipWidth = 128 * std::pow(0.5, mip);     // resize framebuffer according to mip level size
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilter_shader.setUniform1f("roughness", roughness);

        for (unsigned int i = 0; i < 6; ++i) {
            prefilter_shader.setUniformMat4f("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

// generate a 2D Look Up Table from the BRDF equations used

    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    brdf_shader.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    background_shader.bind();
    background_shader.setUniformMat4f("projection", proj);

    //textures set up

//    unsigned int ground_albedo = loadTexture("res/texture/PBR/dusty-ground/dusty-ground-albedo.png");
//    unsigned int ground_normalMap = loadTexture("res/texture/PBR/dusty-ground/dusty-ground-normal.png");
//    unsigned int ground_roughness = loadTexture("res/texture/PBR/dusty-ground/dusty-ground-roughness.png");
//    unsigned int ground_metallic = loadTexture("res/texture/PBR/dusty-ground/dusty-ground-metallic.png");

    unsigned int ground_albedo = loadTexture("res/texture/PBR/desert-rocks1/desert-rocks1-albedo.png");
    unsigned int ground_normalMap = loadTexture("res/texture/PBR/desert-rocks1/desert-rocks1-Normal.png");
    unsigned int ground_roughness = loadTexture("res/texture/PBR/desert-rocks1/desert-rocks1-Roughness.png");
    unsigned int ground_metallic = loadTexture("res/texture/PBR/desert-rocks1/desert-rocks1-Metallic.png");


    unsigned int pyramid_albedo = loadTexture("res/texture/PBR/stonewall/stonewall-albedo.png");
    unsigned int pyramid_normalMap = loadTexture("res/texture/PBR/stonewall/stonewall-normal.png");
    unsigned int pyramid_roughness = loadTexture("res/texture/PBR/stonewall/stonewall-roughness.png");
    unsigned int pyramid_metallic = loadTexture("res/texture/PBR/stonewall/stonewall-metallic.png");

//    unsigned int pyramid_albedo = loadTexture("res/texture/PBR/whorn-shiny-metal/worn-shiny-metal-albedo.png");
//    unsigned int pyramid_normalMap = loadTexture("res/texture/PBR/whorn-shiny-metal/worn-shiny-metal-Normal-dx.png");
//    unsigned int pyramid_roughness = loadTexture("res/texture/PBR/whorn-shiny-metal/worn-shiny-metal-Roughness.png");
//    unsigned int pyramid_metallic = loadTexture("res/texture/PBR/whorn-shiny-metal/worn-shiny-metal-Metallic.png");


//    unsigned int cube_albedo = loadTexture("res/texture/PBR/whorn-shiny-metal/worn-shiny-metal-albedo.png");
//    unsigned int cube_normalMap = loadTexture("res/texture/PBR/whorn-shiny-metal/worn-shiny-metal-Normal-dx.png");
//    unsigned int cube_roughness = loadTexture("res/texture/PBR/whorn-shiny-metal/worn-shiny-metal-Roughness.png");
//    unsigned int cube_metallic = loadTexture("res/texture/PBR/whorn-shiny-metal/worn-shiny-metal-Metallic.png");

    unsigned int cube_albedo = loadTexture("res/texture/PBR/pirate-gold/pirate-gold_albedo.png");
    unsigned int cube_normalMap = loadTexture("res/texture/PBR/pirate-gold/pirate-gold_normal.png");
    unsigned int cube_roughness = loadTexture("res/texture/PBR/pirate-gold/pirate-gold_roughness.png");
    unsigned int cube_metallic = loadTexture("res/texture/PBR/pirate-gold/pirate-gold_metallic.png");


    int samplers[3] = {0, 1, 2};
    int samplersNM[3] = { 3, 4, 5 };
    int samplersRM[3] = { 6, 7, 8 };
    int samplersMM[3] = { 9, 10, 11 };

    






    //set up light shader

/*    int lightColor = glGetUniformLocation(shader, "light.color");                   
    int lightPos = glGetUniformLocation(shader, "light.position");
    int lightDir = glGetUniformLocation(shader, "light.direction");


    int u_viewPos = glGetUniformLocation(shader, "u_viewPos");

    int u_model = glGetUniformLocation(shader, "u_model");
    int u_proj = glGetUniformLocation(shader, "u_proj");
    int u_view = glGetUniformLocation(shader, "u_view");
    
    int lightSpaceMatrixDM = glGetUniformLocation(depth_shader, "lightSpaceMatrix");
    int lightSpaceMatrixS = glGetUniformLocation(shader, "lightSpaceMatrix");
    int depth_model = glGetUniformLocation(depth_shader, "model");                          */
    
    // reconfigure viewport to the original dimension

    int srcW, srcH;
    glfwGetFramebufferSize(window, &srcW, &srcH);
    glViewport(0, 0, srcW, srcH);
    


   
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        
//        glEnable(GL_DEPTH_TEST); 
        glEnable(GL_CULL_FACE);                                   // remember to set counter clockwise order for vertex indices to enable faces culling
//        glDepthFunc(GL_LESS);
//        glDepthFunc(GL_LEQUAL);
        

/*        glCullFace(GL_FRONT);
        glColor3f(0, 0, 0);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(1, 1);   */

        actualTime = glfwGetTime();
        deltaT = actualTime - previousTime;
        previousTime = actualTime;

        


        inputHandle(window);

            

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render from light perspective 

        glm::mat4 lightProj, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f;
        float far_plane = 7.5f;

        lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::vec3 lightPosition(-100.0f + lightX, 1200.0f + lightY, 1200.0f + lightZ);
        lightView = glm::lookAt(lightPosition, glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProj * lightView;


        // normal rendering 
        glBindVertexArray(vao);

        standard_shader.bind();


        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

//        glUniform3f(u_viewPos, cameraPosition.x, cameraPosition.y, cameraPosition.z);

        standard_shader.setUniform3f("u_viewPos", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        
        index_buffer.bind();

        //        glUniform3f(lightColor, 4.5f, 4.1f, 4.3f);
//        glUniform4f(lightPos, -100.0f + lightX, 1200.0f + lightY, 1200.0f + lightZ, 1.0);
//        glUniform4f(lightDir, 0.0f, 0.0f, -1.0f, 0.0);

        standard_shader.setUniform3f("light.color", 1.0f, 1.0f, 1.0f);
        standard_shader.setUniform4f("light.position", -100.0f + lightX, 1200.0f + lightY, 1200.0f + lightZ, 1.0);
        standard_shader.setUniform4f("light.direction", 0.0f, 0.0f, -1.0f, 0.0f);

        //        glUniformMatrix4fv(lightSpaceMatrixS, 1, GL_FALSE, &lightSpaceMatrix[0][0]);


        standard_shader.setUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

        standard_shader.setUniformMat4f("u_proj", proj);
        standard_shader.setUniformMat4f("u_view", view);
        standard_shader.setUniformMat4f("u_model", model4);

        standard_shader.setUniform1iv("albedoMap", 3, samplers);
        standard_shader.setUniform1iv("normalMap", 3, samplersNM);
        standard_shader.setUniform1iv("roughnessMap", 3, samplersRM);
        standard_shader.setUniform1iv("metallicMap", 3, samplersMM);

        standard_shader.setUniform1i("irradianceMap", 12);
        standard_shader.setUniform1i("prefilterMap", 13);
        standard_shader.setUniform1i("brdfLUT", 14);
      

        glActiveTexture(GL_TEXTURE0);                                      // binding texture (albedo)
        glBindTexture(GL_TEXTURE_2D, pyramid_albedo);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cube_albedo);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ground_albedo);
        

        glActiveTexture(GL_TEXTURE3);                                     // binding normal map
        glBindTexture(GL_TEXTURE_2D, pyramid_normalMap);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, cube_normalMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, ground_normalMap);


        glActiveTexture(GL_TEXTURE6);                                    // binding roughness map
        glBindTexture(GL_TEXTURE_2D, pyramid_roughness);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, cube_roughness);
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, ground_roughness);

                               
        glActiveTexture(GL_TEXTURE9);                                    // binding metallic map
        glBindTexture(GL_TEXTURE_2D, pyramid_metallic);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, cube_metallic);
        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, ground_metallic);

        // bind pre-computed IBL map

        glActiveTexture(GL_TEXTURE12);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE13);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE14);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    
      

        glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, nullptr);     
        

        
        index_buffer.unbind();

        glBindVertexArray(0);



        // render skybox

        glDisable(GL_CULL_FACE);
    

        background_shader.bind();
        background_shader.setUniformMat4f("view", view);
        background_shader.setUniformMat4f("projection", proj);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        renderCube();          

/*        eqToCubeMap_shader.bind();
        eqToCubeMap_shader.setUniformMat4f("view", view);
        eqToCubeMap_shader.setUniformMat4f("projection", proj);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        renderCube(1.0f);    */

   

/*        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glCullFace(GL_BACK);  */

        glfwSwapInterval(1);



//            glEnable(GL_FRAMEBUFFER_SRGB);              // Gamma correction



        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

//    glDeleteProgram(shader);



    glfwTerminate();
    return 0;

	
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


static int loadTexture(const std::string& path) {
    int w, h, b;


    stbi_set_flip_vertically_on_load(1);
    auto* pixels = stbi_load(path.c_str(), &w, &h, &b, STBI_rgb);
    unsigned int textureId;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);
    return textureId;
}

static int loadHDRTex(const std::string& path) {

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;


    if (data) {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load HDR texture! \n";
    }
    stbi_image_free(data);
    return hdrTexture;

}


void inputHandle(GLFWwindow* window) {

    const float speed = 500.0f * deltaT;
    glm::vec3 RAxis = glm::cross(cameraFront, cameraUp);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  cameraPosition += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  cameraPosition -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  cameraPosition += speed * RAxis;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  cameraPosition -= speed * RAxis;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraPosition += speed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) cameraPosition -= speed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) lightZ -= 10.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) lightZ += 10.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) lightY += 10.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) lightY -= 10.0f;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) lightX += 10.0f;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) lightX -= 10.0f;

    //    std::cout << "X: " << lightX << " Y: " << lightY << " Z: " << lightZ << std::endl;

}

void mouseHandle(GLFWwindow* window, double xpos, double ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float speed = 0.085f;

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= speed;
    yoffset *= speed;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;


    glm::vec3 direction;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

void renderCube() {

    if (cubeVAO == 0) {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
   
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad() {
    if (quadVAO == 0) {

        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        std::vector<float> data;
        for (std::size_t i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}


