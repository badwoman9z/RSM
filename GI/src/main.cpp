
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<random>
#include "shader.h"
#include "camera.h"
#include"stb_image.h"
#include <iostream>
#include"model.h"
#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw_gl3.h"
#define PI 3.141592653589793
#define SAMPLE_NUMBER 40
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderQuad();
// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

// camera
Camera camera(glm::vec3(0, 2.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);





    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);




    //ImGui::CreateContext();
    //ImGui_ImplGlfwGL3_Init(window, true);
    //ImGui::StyleColorsDark();

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float near_plane = 1e-2, far_plane = 20;
    float left = -10;
    float right = 10;
    glm::vec3 o  = glm::vec3(0.0, 0, 0);
    float light_near_plane = 0.1f;
    float light_far_plane = 40.0f;
    const GLuint RSM_WIDTH = 1024, RSM_HEIGHT = 1024;

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    Model box("E:/Desktop/grahpic/RSM/GI/GI/model/scene02.obj");
    Model marry("E:/Desktop/grahpic/RSM/GI/GI/model/Marry.obj");
    Shader shader("./shader/box.vs", "./shader/box.fs");
    Shader debugShader("./shader/debug.vs", "./shader/debug.fs");
    Shader rsm("./shader/rsm.vs", "./shader/rsm.fs");

    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    //法线纹理
    unsigned int normalTexture;
    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,normalTexture,0);

    //位置纹理
    unsigned int positionTexture;
    glGenTextures(1, &positionTexture);
    glBindTexture(GL_TEXTURE_2D, positionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, positionTexture, 0);
    //间接光源纹理
    unsigned int fluxTexture;
    glGenTextures(1, &fluxTexture);
    glBindTexture(GL_TEXTURE_2D, fluxTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, fluxTexture, 0);
    //深度纹理
    unsigned int depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};

    glDrawBuffers(3, attachments);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //随机采样map
    std::vector<glm::vec4> sampleCoordsandWeights;
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    for (int i = 0; i < SAMPLE_NUMBER; i++) {
        float xi1 = u(e);
        float xi2 = u(e);
        sampleCoordsandWeights.push_back({ xi1 * sin(2 * PI * xi2), xi1 * cos(2 * PI * xi2), xi1 * xi1, 0 });
    }
    unsigned int uniformBlockIndex = glGetUniformBlockIndex(rsm.ID,"randomMap");
    glUniformBlockBinding(rsm.ID, uniformBlockIndex, 0);

    unsigned int ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(GL_FLOAT) * sampleCoordsandWeights.size(), sampleCoordsandWeights.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 4 * sizeof(GL_FLOAT) * sampleCoordsandWeights.size());
   
    
    //shader配置
    glm::vec3 lightPos(1, 2.0f, 10.0f);
    shader.use();
    shader.setVec3("lightPos", lightPos);
    shader.setVec3("viewPos", camera.Position);
    debugShader.use();
    debugShader.setInt("normalTexture", 0);
    debugShader.setInt("positionTexture", 1);
    debugShader.setInt("fluxTexture", 2);
    debugShader.setInt("depthTexture", 3);
    rsm.use();
    rsm.setInt("normalTexture", 0);
    rsm.setInt("positionTexture", 1);
    rsm.setInt("fluxTexture", 2);
    rsm.setInt("depthTexture", 3);
    rsm.setFloat("filterRange", 25);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

       

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //ImGui_ImplGlfwGL3_NewFrame();

        //{
        //    static float f = 0.0f;
        //    static int counter = 0;
        //    ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
        //    ImGui::SliderFloat3("float3", &o.x, -2000.0f, 2000.0f);    // Edit 1 float using a slider from 0.0f to 1.0f    
        //    ImGui::SliderFloat("float", &lightPos.x, -2000.0f, 2000.0f);
        //    ImGui::SliderFloat("left", &left, -2000.0f, 2000.0f);
        //    ImGui::SliderFloat("right", &right, -2000.0f, 2000.0f);
        //    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        //    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
        //    ImGui::Checkbox("Another Window", &show_another_window);

        //    if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
        //        counter++;
        //    ImGui::SameLine();
        //    ImGui::Text("counter = %d", counter);

        //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //}
        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);          
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0, 0, -2));
        model = glm::scale(model,glm::vec3(0.5, 0.5, 0.5));
        glm::mat4 lightView = glm::lookAt(lightPos, o, glm::vec3(0.0, 1.0, 0.0));
        
        glm::mat4 lightProjection = glm::ortho(left, right, left, right, near_plane, far_plane);
        glm::mat4 lightSpace = lightProjection * lightView;

        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("lightSpace", lightSpace);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera.Position);
        box.Draw(shader);
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-4, 0, 0));
        model = glm::scale(model, glm::vec3(1, 1, 1));   
        shader.setMat4("model", model);
        marry.Draw(shader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //debugShader.use();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, normalTexture);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, positionTexture);
        //glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, fluxTexture);
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D, depthTexture);
        //renderQuad();

        rsm.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, positionTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, fluxTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        glm::mat4 view(1.0);
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 20.0f);

        rsm.setMat4("view", view);
        rsm.setMat4("projection", projection);
        rsm.setMat4("lightSpace", lightSpace);
        rsm.setVec3("lightPos", lightPos);
        rsm.setVec3("viewPos", camera.Position);

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0, 0, -2));
        model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        rsm.setMat4("model", model);
        
        box.Draw(rsm);
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-4, 0, 0));
        model = glm::scale(model, glm::vec3(1, 1, 1));
        rsm.setMat4("model", model);
        
        marry.Draw(rsm);

        //ImGui::Render();
        //ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    //ImGui_ImplGlfwGL3_Shutdown();
    //ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include<random>
//#include "shader.h"
//#include "camera.h"
//#include"stb_image.h"
//#include <iostream>
//#include"model.h"
//#include"imgui/imgui.h"
//#include"imgui/imgui_impl_glfw_gl3.h"
//
//const float PI = 3.14159265358979;
//
//const int enable_debug = 0;
//
//// settings
//const unsigned int SCR_WIDTH = 1024;
//const unsigned int SCR_HEIGHT = 1024;
//
//const unsigned int MAX_SAMPLE_NUM = 32;
//const float MAX_SAMPLE_RADIUS = 0.3;
//
////Camera
//Camera camera(glm::vec3(-4.0f, 3.0f, 4.0f));
//float lastX = 400, lastY = 300;
//bool firstMouse = true;
//
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//glm::vec3 lightPos = glm::vec3(-8.0f, 6.0f, 6.0f);
//glm::vec3 light_diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
//float light_near_plane = 0.5f;
//float light_far_plane = 20.0f;
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//
//GLuint createRandomTexture(int size);
//
//class Planes {
//	unsigned int groundvao, backwallvao, rightwallvao;
//	unsigned int groundvbo, backwallvbo, rightwallvbo;
//	unsigned int groundebo, backwallebo, rightwallebo;
//
//public:
//	Planes() {
//		float ground_vertices[] = {
//			//position           //normal
//			 0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,
//			-5.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f,
//			-5.0f,  0.0f,  5.0f,  0.0f, 1.0f, 0.0f,
//			 0.0f,  0.0f,  5.0f,  0.0f, 1.0f, 0.0f
//		};
//		float backwall_vertices[] = {
//			//position           //normal
//			 0.0f,  0.0f,  0.0f,  0.0f, 0.0f, 1.0f,
//			 0.0f,  5.0f,  0.0f,  0.0f, 0.0f, 1.0f,
//			-5.0f,  5.0f,  0.0f,  0.0f, 0.0f, 1.0f,
//			-5.0f,  0.0f,  0.0f,  0.0f, 0.0f, 1.0f
//		};
//		float rightwall_vertices[] = {
//			//position          //normal
//			 0.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
//			 0.0f,  0.0f,  5.0f, -1.0f, 0.0f, 0.0f,
//			 0.0f,  5.0f,  5.0f, -1.0f, 0.0f, 0.0f,
//			 0.0f,  5.0f,  0.0f, -1.0f, 0.0f, 0.0f
//		};
//		unsigned int plane_ebo[] = {
//			0, 1, 3,
//			1, 2, 3
//		};
//		//ground
//		glGenVertexArrays(1, &groundvao);
//		glGenBuffers(1, &groundvbo);
//		glGenBuffers(1, &groundebo);
//		glBindVertexArray(groundvao);
//		glBindBuffer(GL_ARRAY_BUFFER, groundvbo);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices), ground_vertices, GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundebo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_ebo), plane_ebo, GL_STATIC_DRAW);
//		//backwall
//		glGenVertexArrays(1, &backwallvao);
//		glGenBuffers(1, &backwallvbo);
//		glGenBuffers(1, &backwallebo);
//		glBindVertexArray(backwallvao);
//		glBindBuffer(GL_ARRAY_BUFFER, backwallvbo);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(backwall_vertices), backwall_vertices, GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backwallebo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_ebo), plane_ebo, GL_STATIC_DRAW);
//		//rightwall
//		glGenVertexArrays(1, &rightwallvao);
//		glGenBuffers(1, &rightwallvbo);
//		glGenBuffers(1, &rightwallebo);
//		glBindVertexArray(rightwallvao);
//		glBindBuffer(GL_ARRAY_BUFFER, rightwallvbo);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(rightwall_vertices), rightwall_vertices, GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rightwallebo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_ebo), plane_ebo, GL_STATIC_DRAW);
//	}
//	void draw(Shader& shader) {
//		shader.use();
//		glm::mat4 model = glm::mat4(1.0f);
//		shader.setMat4("model", model);
//		glBindVertexArray(groundvao);
//		shader.setVec3("material.diffuse", 0.0f, 0.0f, 0.8f);
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//		glBindVertexArray(backwallvao);
//		shader.setVec3("material.diffuse", 0.0f, 0.8f, 0.0f);
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//		glBindVertexArray(rightwallvao);
//		shader.setVec3("material.diffuse", 0.8f, 0.0f, 0.0f);
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//	}
//};
//class CubeFrame {
//	unsigned int vao, vbo;
//public:
//	CubeFrame() {
//		float vertices[] = {
//			 0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,
//			 0.0f,  0.0f, 0.25f,  0.0f, -1.0f,  0.0f,
//			0.25f,  0.0f, 0.25f,  0.0f, -1.0f,  0.0f,
//			0.25f,  0.0f, 0.25f,  0.0f, -1.0f,  0.0f,
//			0.25f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,
//			 0.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.0f,
//
//			 0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//			 0.0f,  2.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//			 0.0f,  2.0f, 0.25f, -1.0f,  0.0f,  0.0f,
//			 0.0f,  2.0f, 0.25f, -1.0f,  0.0f,  0.0f,
//			 0.0f,  0.0f, 0.25f, -1.0f,  0.0f,  0.0f,
//			 0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
//
//			 0.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,
//			0.25f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,
//			0.25f,  2.0f,  0.0f,  0.0f,  0.0f, -1.0f,
//			0.25f,  2.0f,  0.0f,  0.0f,  0.0f, -1.0f,
//			 0.0f,  2.0f,  0.0f,  0.0f,  0.0f, -1.0f,
//			 0.0f,  0.0f,  0.0f,  0.0f,  0.0f, -1.0f,
//
//			 0.0f,  0.0f, 0.25f,  0.0f,  0.0f,  1.0f,
//			0.25f,  0.0f, 0.25f,  0.0f,  0.0f,  1.0f,
//			0.25f,  2.0f, 0.25f,  0.0f,  0.0f,  1.0f,
//			0.25f,  2.0f, 0.25f,  0.0f,  0.0f,  1.0f,
//			 0.0f,  2.0f, 0.25f,  0.0f,  0.0f,  1.0f,
//			 0.0f,  0.0f, 0.25f,  0.0f,  0.0f,  1.0f,
//
//			0.25f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//			0.25f,  2.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//			0.25f,  2.0f, 0.25f,  1.0f,  0.0f,  0.0f,
//			0.25f,  2.0f, 0.25f,  1.0f,  0.0f,  0.0f,
//			0.25f,  0.0f, 0.25f,  1.0f,  0.0f,  0.0f,
//			0.25f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
//
//			 0.0f,  2.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//			 0.0f,  2.0f, 0.25f,  0.0f,  1.0f,  0.0f,
//			0.25f,  2.0f, 0.25f,  0.0f,  1.0f,  0.0f,
//			0.25f,  2.0f, 0.25f,  0.0f,  1.0f,  0.0f,
//			0.25f,  2.0f,  0.0f,  0.0f,  1.0f,  0.0f,
//			 0.0f,  2.0f,  0.0f,  0.0f,  1.0f,  0.0f
//		};
//		glGenVertexArrays(1, &vao);
//		glGenBuffers(1, &vbo);
//		glBindVertexArray(vao);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//	}
//	void draw(Shader& shader) {
//		shader.use();
//		shader.setVec3("material.diffuse", 0.8f, 0.8f, 0.8f);
//		glBindVertexArray(vao);
//		glm::mat4 model = glm::mat4(1.0f);
//		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 1.0f));
//		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 3.0f));
//		shader.setMat4("model", model);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//		model = glm::translate(model, glm::vec3(1.75f, 0.0f, 0.0f));
//		shader.setMat4("model", model);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
//		shader.setMat4("model", model);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//		model = glm::translate(model, glm::vec3(-1.75f, 0.0f, 0.0f));
//		shader.setMat4("model", model);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//		model = glm::mat4(1.0f);
//		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 1.0f));
//		model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 1.0f));
//		glm::mat4 rotate = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//		shader.setMat4("model", rotate);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
//		rotate = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//		shader.setMat4("model", rotate);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//		model = glm::translate(model, glm::vec3(-0.25f, 0.0f, 0.0f));
//		rotate = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//		shader.setMat4("model", rotate);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//		model = glm::translate(model, glm::vec3(-1.75f, 0.0f, 0.0f));
//		rotate = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//		shader.setMat4("model", rotate);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//	}
//};
//class Debug {
//	unsigned vao, vbo;
//public:
//	Debug() {
//		float debugVertices[] = {
//			// positions        // texture Coords
//		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
//		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
//		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
//		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//		};
//		glGenVertexArrays(1, &vao);
//		glGenBuffers(1, &vbo);
//		glBindVertexArray(vao);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(debugVertices), debugVertices, GL_STATIC_DRAW);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//	}
//	void draw(Shader& shader) {
//		shader.use();
//		glBindVertexArray(vao);
//		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//		glBindVertexArray(0);
//	}
//};
//
//int main() {
//	//initialize glfw
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shadow Map", NULL, NULL);
//	if (window == NULL) {
//		std::cout << "Failed to Create glfw window\n";
//		return -1;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//		std::cout << "Failed to initialize GLAD\n";
//		return -1;
//	}
//
//	glEnable(GL_DEPTH_TEST);
//
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//
//	Shader main_light_shader("./shader/result_shader.vert", "./shader/result_shader.frag");
//	Shader light_space_shader("./shader/lightSpaceShader.vert", "./shader/lightSpaceShader.frag");
//	//Shader debug_shader("./shader/debug.vert", "./shader/debug.frag");
//
//	Planes planes;
//	CubeFrame cubeFrame;
//	Debug debug;
//
//
//
//	//创建帧缓冲
//	GLuint rsmFBO;
//	GLuint depthMap, normalMap, worldPosMap, fluxMap;
//	glGenFramebuffers(1, &rsmFBO);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, rsmFBO);
//	const GLuint RSM_WIDTH = 1024, RSM_HEIGHT = 1024;
//	//深度缓存
//	glGenTextures(1, &depthMap);
//	glBindTexture(GL_TEXTURE_2D, depthMap);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, RSM_WIDTH, RSM_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	GLfloat depth_borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, depth_borderColor);
//	//法线缓存
//	glGenTextures(1, &normalMap);
//	glBindTexture(GL_TEXTURE_2D, normalMap);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, RSM_WIDTH, RSM_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	GLfloat border_Color[] = { 0.0, 0.0, 0.0, 0.0 };
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, depth_borderColor);
//	//世界坐标缓存
//	glGenTextures(1, &worldPosMap);
//	glBindTexture(GL_TEXTURE_2D, worldPosMap);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, RSM_WIDTH, RSM_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, depth_borderColor);
//	//光通量缓存
//	glGenTextures(1, &fluxMap);
//	glBindTexture(GL_TEXTURE_2D, fluxMap);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RSM_WIDTH, RSM_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, depth_borderColor);
//
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, worldPosMap, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, fluxMap, 0);
//
//	GLenum rsm_draw_buffers[] = {
//	GL_COLOR_ATTACHMENT0,
//	GL_COLOR_ATTACHMENT1,
//	GL_COLOR_ATTACHMENT2
//	};
//	glDrawBuffers(3, rsm_draw_buffers);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	//生成一个用于采样的随机纹理
//	GLuint randomMap = createRandomTexture(MAX_SAMPLE_NUM);
//
//	//绑定纹理
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, depthMap);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, normalMap);
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, worldPosMap);
//	glActiveTexture(GL_TEXTURE3);
//	glBindTexture(GL_TEXTURE_2D, fluxMap);
//	glActiveTexture(GL_TEXTURE4);
//	glBindTexture(GL_TEXTURE_2D, randomMap);
//
//
//
//	//配置光源空间的着色器
//	glm::mat4 lightProjection = glm::perspective(glm::radians(60.0f), (float)RSM_WIDTH / (float)RSM_HEIGHT, light_near_plane, light_far_plane);
//	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
//	light_space_shader.use();
//	light_space_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
//	light_space_shader.setVec3("light.position", lightPos);
//	light_space_shader.setVec3("light.diffuse", light_diffuse);
//
//
//	//配置主绘制着色器
//	main_light_shader.use();
//	main_light_shader.setVec3("material.ambient", 0.1f, 0.1f, 0.1f);
//	main_light_shader.setVec3("material.specular", 0.1f, 0.1f, 0.1f);
//	main_light_shader.setFloat("material.shininess", 8.0f);
//	main_light_shader.setVec3("light.position", lightPos);
//	main_light_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
//	main_light_shader.setVec3("light.diffuse", light_diffuse);
//	main_light_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
//	main_light_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
//	main_light_shader.setInt("sample_num", MAX_SAMPLE_NUM);
//	main_light_shader.setFloat("sample_radius", MAX_SAMPLE_RADIUS);
//	main_light_shader.setFloat("shadow_bias", 0.05);
//
//	//指定采样器
//	main_light_shader.setInt("depthMap", 0);
//	main_light_shader.setInt("normalMap", 1);
//	main_light_shader.setInt("worldPosMap", 2);
//	main_light_shader.setInt("fluxMap", 3);
//	main_light_shader.setInt("randomMap", 4);
//	main_light_shader.setFloat("near_plane", light_near_plane);
//	main_light_shader.setFloat("far_plane", light_far_plane);
//
//
//	//debug
//	//debug_shader.use();
//	//debug_shader.setFloat("near_plane", light_near_plane);
//	//debug_shader.setFloat("far_plane", light_far_plane);
//	//debug_shader.setInt("depthMap", 0);
//	//debug_shader.setInt("normalMap", 1);
//	//debug_shader.setInt("worldPosMap", 2);
//	//debug_shader.setInt("fluxMap", 3);
//
//	while (!glfwWindowShouldClose(window)) {
//		//time
//		float currentFrame = glfwGetTime();
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//		processInput(window);
//
//		//rsm render
//		glBindFramebuffer(GL_FRAMEBUFFER, rsmFBO);
//		glClear(GL_DEPTH_BUFFER_BIT);
//		light_space_shader.use();
//		glViewport(0, 0, RSM_WIDTH, RSM_HEIGHT);
//		planes.draw(light_space_shader);
//		cubeFrame.draw(light_space_shader);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//
//		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//		//debug_shader.use();
//		//debug.draw(debug_shader);
//
//
//		main_light_shader.use();
//		main_light_shader.setVec3("viewPos", camera.Position);
//		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//		glm::mat4 view = camera.GetViewMatrix();
//		main_light_shader.setMat4("projection", projection);
//		main_light_shader.setMat4("view", view);
//		glm::mat4 model = glm::mat4(1.0f);
//		main_light_shader.setMat4("model", model);
//
//		planes.draw(main_light_shader);
//		cubeFrame.draw(main_light_shader);
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	return 0;
//}
//
//
//void processInput(GLFWwindow* window) {
//	float cameraSpeed = 2.5f * deltaTime;
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//	glViewport(0, 0, width, height);
//}
//void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//	if (firstMouse) {
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos;
//	lastX = xpos;
//	lastY = ypos;
//
//	float sensitivity = 0.05f;
//	xoffset *= sensitivity;
//	yoffset *= sensitivity;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
//	camera.ProcessMouseScroll(yoffset);
//}
//
////生成采样用的随机纹理
//GLuint createRandomTexture(int size) {
//	std::default_random_engine eng;
//	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
//	eng.seed(std::time(0));
//	glm::vec3* randomData = new glm::vec3[size];
//	for (int i = 0; i < size; ++i) {
//		float r1 = dist(eng);
//		float r2 = dist(eng);
//		randomData[i].x = r1 * std::sin(2 * PI * r2);
//		randomData[i].y = r1 * std::cos(2 * PI * r2);
//		randomData[i].z = r1 * r1;
//	}
//	GLuint randomTexture;
//	glGenTextures(1, &randomTexture);
//	glBindTexture(GL_TEXTURE_2D, randomTexture);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, 1, 0, GL_RGB, GL_FLOAT, randomData);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	delete[] randomData;
//	return randomTexture;
//}