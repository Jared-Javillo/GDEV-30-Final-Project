#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <learnopenggl/camera.h>
#include <learnopenggl/shader_m.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 2.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

struct Vertex
{
    GLfloat x, y, z;	// Position
    GLubyte r, g, b;	// Color
    GLfloat u, v;		// UV coordinates
    GLfloat nx, ny, nz;
};

char textureList[4][15] = { "stone.jpg" , "dailee.jpg", "sun.jpg", "stonebrick.jpg"};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Torture Chamber", NULL, NULL);
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("main.vsh", "main.fsh");
    Shader lightCubeShader("light.vsh", "light.fsh");

    //Vertex
    // ------------------------------------------------------------------
    Vertex vertices[36];

	// Position									Color				UV					Normals
	//Front
	vertices[0] = { -0.5f, -0.5f, -0.5f,	0  , 0  , 255,		0.0f, 0.0f,	     0.0f, 0.0f, -1.0f };	// Lower-left
	vertices[1] = {	 0.5f, -0.5f, -0.5f,	0  , 0  , 255,		1.0f, 0.0f,		 0.0f, 0.0f, -1.0f };	// Lower-right
	vertices[2] = {  0.5f, 0.5f, -0.5f,		0  , 0  , 255,		1.0f, 1.0f,		 0.0f, 0.0f, -1.0f };	// Upper-right

	vertices[3] = {  0.5f, 0.5f, -0.5f,		0  , 0  , 255,		1.0f, 1.0f,		 0.0f, 0.0f, -1.0f };	// Upper-right
	vertices[4] = { -0.5f, 0.5f, -0.5f,		0  , 0  , 255,		0.0f, 1.0f,		 0.0f, 0.0f, -1.0f };	// Upper-left
	vertices[5] = { -0.5f, -0.5f, -0.5f,	0  , 0  , 255,		0.0f, 0.0f,		 0.0f, 0.0f, -1.0f };	// Lower-left


	//Back
	vertices[6] = { -0.5f, -0.5f, 0.5,		0  , 0  , 255,		0.0f, 0.0f, 	 0.0f, 0.0f, 1.0f };	// Lower-left
	vertices[7] = { 0.5f, -0.5f, 0.5f,		0  , 0  , 255,		1.0f, 0.0f,		 0.0f, 0.0f, 1.0f };	// Lower-right
	vertices[8] = { 0.5f, 0.5f, 0.5f,		0  , 0  , 255,		1.0f, 1.0f,		 0.0f, 0.0f, 1.0f };	// Upper-right

	vertices[9] = {   0.5f, 0.5f, 0.5f,		0  , 0  , 255,		1.0f, 1.0f,		 0.0f, 0.0f, 1.0f };	// Upper-right
	vertices[10] = { -0.5f, 0.5f, 0.5f,		0  , 0  , 255,		0.0f, 1.0f,		 0.0f, 0.0f, 1.0f };	// Upper-left
	vertices[11] = { -0.5f, -0.5f, 0.5f,	0  , 0  , 255,		0.0f, 0.0f,		 0.0f, 0.0f, 1.0f };	// Lower-left

	//Side left
	vertices[12] = { -0.5f, -0.5f, -0.5f,	0  , 0  , 255,		0.0f, 0.0f,		-1.0f, 0.0f, 0.0f };	// Lower-left
	vertices[13] = { -0.5f, -0.5f, 0.5f,	0  , 0  , 255,		1.0f, 0.0f,		-1.0f, 0.0f, 0.0f };	// Lower-right
	vertices[14] = { -0.5f, 0.5f, 0.5f,		0  , 0  , 255,		1.0f, 1.0f,		-1.0f, 0.0f, 0.0f };	// Upper-right

	vertices[15] = { -0.5f, 0.5f, 0.5f,		0  , 0  , 255,		1.0f, 1.0f,		-1.0f, 0.0f, 0.0f };	// Upper-right
	vertices[16] = { -0.5f, 0.5f, -0.5f,	0  , 0  , 255,		0.0f, 1.0f,		-1.0f, 0.0f, 0.0f };	// Upper-left
	vertices[17] = { -0.5f, -0.5f, -0.5f,	0  , 0  , 255,		0.0f, 0.0f,		-1.0f, 0.0f, 0.0f };	// Lower-left

	//Side Right
	vertices[18] = { 0.5f, -0.5f, -0.5f,	0  , 0  , 255,		0.0f, 0.0f,		 1.0f, 0.0f, 0.0f };	// Lower-left
	vertices[19] = { 0.5f, -0.5f, 0.5f,		0  , 0  , 255,		1.0f, 0.0f,		 1.0f, 0.0f, 0.0f };	// Lower-right
	vertices[20] = { 0.5f, 0.5f, 0.5f,		0  , 0  , 255,		1.0f, 1.0f,		 1.0f, 0.0f, 0.0f };	// Upper-right

	vertices[21] = { 0.5f, 0.5f, 0.5f,		0  , 0  , 255,		1.0f, 1.0f,		 1.0f, 0.0f, 0.0f };	// Upper-right
	vertices[22] = { 0.5f, 0.5f, -0.5f,		0  , 0  , 255,		0.0f, 1.0f,		 1.0f, 0.0f, 0.0f };	// Upper-left
	vertices[23] = { 0.5f, -0.5f, -0.5f,	0  , 0  , 255,		0.0f, 0.0f,		 1.0f, 0.0f, 0.0f };	// Lower-left

	//Top
	vertices[24] = { -0.5f, 0.5f, 0.5f,		0  , 0  , 255,		0.0f, 0.0f,		 0.0f, 1.0f, 0.0f };	// Lower-left
	vertices[25] = { 0.5f, 0.5f, 0.5f,		0  , 0  , 255,		1.0f, 0.0f,		 0.0f, 1.0f, 0.0f };	// Lower-right
	vertices[26] = { 0.5f, 0.5f, -0.5f,		0  , 0  , 255,		1.0f, 1.0f,		 0.0f, 1.0f, 0.0f };	// Upper-right

	vertices[27] = { 0.5f, 0.5f, -0.5f,		0  , 0  , 255,		1.0f, 1.0f,		 0.0f, 1.0f, 0.0f };	// Upper-right
	vertices[28] = { -0.5f, 0.5f, -0.5f,	0  , 0  , 255,		0.0f, 1.0f,		 0.0f, 1.0f, 0.0f };	// Upper-left
	vertices[29] = { -0.5f, 0.5f, 0.5f,		0  , 0  , 255,		0.0f, 0.0f,		 0.0f, 1.0f, 0.0f };	// Lower-left
     
	//Bottom
	vertices[30] = { -0.5f, -0.5f, 0.5f,	0  , 0  , 255,		0.0f, 0.0f,		 0.0f, -1.0f, 0.0f };	// Lower-left
	vertices[31] = { 0.5f, -0.5f, 0.5f,		0  , 0  , 255,		1.0f, 0.0f,		 0.0f, -1.0f, 0.0f };	// Lower-right
	vertices[32] = { 0.5f, -0.5f, -0.5f,	0  , 0  , 255,  	1.0f, 1.0f,		 0.0f, -1.0f, 0.0f };	// Upper-right

	vertices[33] = { 0.5f, -0.5f, -0.5f,	0  , 0  , 255,		1.0f, 1.0f,		 0.0f, -1.0f, 0.0f };	// Upper-right
	vertices[34] = { -0.5f, -0.5f, -0.5f,	0  , 0  , 255,		0.0f, 1.0f,		 0.0f, -1.0f, 0.0f };	// Upper-left
	vertices[35] = { -0.5f, -0.5f, 0.5f,	0  , 0  , 255,		0.0f, 0.0f,		 0.0f, -1.0f, 0.0f };	// Lower-left

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // Vertex attribute 0 - Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

    // Vertex attribute 1 - Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));

    // Vertex attribute 2 - UV coordinate
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, u)));

    // Vertex attribute 3 - Normal vectors
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, nx)));


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);

    GLuint tex;
    glGenTextures(3, &tex);

    // --- Load our image using stb_image ---

    // Im image-space (pixels), (0, 0) is the upper-left corner of the image
    // However, in u-v coordinates, (0, 0) is the lower-left corner of the image
    // This means that the image will appear upside-down when we use the image data as is
    // This function tells stbi to flip the image vertically so that it is not upside-down when we use it
    stbi_set_flip_vertically_on_load(true);

    // 'imageWidth' and imageHeight will contain the width and height of the loaded image respectively
    int imageWidth, imageHeight, numChannels;
    int textureCount = sizeof textureList / sizeof textureList[0];

    for (int i = 0; i < textureCount; i++) {
        // Read the image data and store it in an unsigned char array
        unsigned char* imageData = stbi_load(textureList[i], &imageWidth, &imageHeight, &numChannels, 0);

        // Make sure that we actually loaded the image before uploading the data to the GPU
        if (imageData != nullptr)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            // Our texture is 2D, so we bind our texture to the GL_TEXTURE_2D target
            glBindTexture(GL_TEXTURE_2D, tex + i);

            // Set the filtering methods for magnification and minification
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            // Set the wrapping method for the s-axis (x-axis) and t-axis (y-axis)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // Upload the image data to GPU memory
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

            // If we set minification to use mipmaps, we can tell OpenGL to generate the mipmaps for us
            //glGenerateMipmap(GL_TEXTURE_2D);

            // Once we have copied the data over to the GPU, we can delete
            // the data on the CPU side, since we won't be using it anymore
            stbi_image_free(imageData);
            imageData = nullptr;
        }
        else
        {
            std::cerr << "Failed to load image" << std::endl;
        }
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float lightX = 2.0f * sin(glfwGetTime());
        float lightY = 2.0f;
        float lightZ = 3.0f + (2.5f * cos(glfwGetTime()));
        glm::vec3 lightPos = glm::vec3(lightX, lightY, lightZ);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 0.01f, 0.01f, 0.01f);
        lightingShader.setVec3("lightColor", 1.0f, 0.68f, 0.26f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        //Table
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(4.0f, 0.1f, 4.0f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.8f, -1.5f, 1.8f));
        model = glm::scale(model, glm::vec3(0.2f, -3.0f, 0.2f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.8f, -1.5f, 1.8f));
        model = glm::scale(model, glm::vec3(0.2f, -3.0f, 0.2f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.8f, -1.5f, -1.8f));
        model = glm::scale(model, glm::vec3(0.2f, -3.0f, 0.2f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.8f, -1.5f, -1.8f));
        model = glm::scale(model, glm::vec3(0.2f, -3.0f, 0.2f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //Walls and Floors
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
        model = glm::scale(model, glm::vec3(8.0f, 8.0f, 0.1f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 3);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-4.0f, 1.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.1f, 8.0f, 10.0f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 3);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 1.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.1f, 8.0f, 10.0f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 3);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 1.0f));
        model = glm::scale(model, glm::vec3(8.0f, 0.1f, 10.0f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 3);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 5.0f, 1.0f));
        model = glm::scale(model, glm::vec3(8.0f, 0.1f, 10.0f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 3);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 6.0f));
        model = glm::scale(model, glm::vec3(8.0f, 8.0f, 0.1f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 3);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //Banner
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, -3.5f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 0.1f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 1);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Rotating Cubes
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
        model = glm::rotate(model, float(glfwGetTime())*2, glm::vec3(1.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
        model = glm::rotate(model, float(glfwGetTime()) *3, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);


        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 2.0f, 1.0f));
        model = glm::rotate(model, float(glfwGetTime()) * 2, glm::vec3(1.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 2.0f, 1.0f));
        model = glm::rotate(model, float(glfwGetTime()) * 3, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);


        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 2.0f, 1.0f));
        model = glm::rotate(model, float(glfwGetTime()) * 2, glm::vec3(1.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 2.0f, 1.0f));
        model = glm::rotate(model, float(glfwGetTime()) * 3, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        lightingShader.setMat4("model", model);
        lightingShader.setInt("tex", 0);


        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setVec3("lightColor", 1.0f, 0.68f, 0.26f);
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::rotate(model, float(glfwGetTime()) * 6, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);
        lightCubeShader.setInt("tex", 2);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
    camera.ProcessMouseScroll(yoffset);
}

