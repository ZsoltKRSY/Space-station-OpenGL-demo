#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm.hpp> //core glm functionality
#include <gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include "ObjectLoader.hpp"

#include <iostream>

// window
gps::Window myWindow;
const unsigned int windowWidth = 1920, windowHeight = 1080;

const unsigned int SHADOW_WIDTH = 16384;
const unsigned int SHADOW_HEIGHT = 16384;

const unsigned int POINT_SHADOW_WIDTH = 2048;
const unsigned int POINT_SHADOW_HEIGHT = 2048;

float counterCoeff = 150.0f;

enum ViewMode {
    SOLID,
    WIREFRAME,
    POLYGONAL,
    SMOOTH
};

enum ViewMode viewModeCurrent = SOLID;

gps::ObjectLoader objectLoader = gps::ObjectLoader();

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

glm::vec4 clearColor = glm::vec4(0.35f, 0.81f, 0.48f, 1.0f);

bool isFog = true;

// light parameters
glm::vec3 lightDir = glm::vec3(219.518845f, 16.828085f, 239.567871f); //directional light - sun
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light;
glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float near_plane = 0.1f, far_plane = 600.0f;
glm::mat4 lightProjection = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, near_plane, far_plane);
glm::mat4 dirLightSpaceTrMatrix = lightProjection * lightView;

// camera
// manual movement
glm::vec3 moveCameraPosition = glm::vec3(-45.63f, 1.0f, -0.42f);
glm::vec3 moveCameraTarget = glm::vec3(-45.63f, 1.0f, -10.42f);
glm::vec3 moveCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float walkCameraSpeed = 4.0f, flyCameraSpeed = 35.0f, mouseSensitivity = 0.05f; //cameraSpeed 3.0f
float totalPitch = 0.0f;
glm::vec2 lastMousePos = glm::vec2(windowWidth / 2.0f, windowHeight / 2.0f);
bool firstMouse = true;
bool flyMode = false;

//auto presentation
glm::vec3 autoCameraPosition = glm::vec3(-67.11f, -6.1f, -91.61f);
glm::vec3 autoCameraTarget = glm::vec3(-67.55f, -6.1f, -90.72);
glm::vec3 autoCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool autoScenePresentation = false;
float autoCameraSpeed = 5.0f;
int autoTurnNumber = 0;
float autoTurnCount = 185.0f;
float autoTurnAngle = 0.4f * counterCoeff;

gps::Camera myCamera(
    moveCameraPosition,
    moveCameraTarget,
    moveCameraUp);

float deltaTime = 0.0f, lastFrame = 0.0f;

GLboolean pressedKeys[1024];


// shaders
gps::Shader shaderTexture;
gps::Shader shaderColorOnly;
gps::Shader skyBoxShader;
gps::Shader depthMapShader;
gps::Shader depthCubemapShader;

//skyBox cubemap
gps::SkyBox mySkyBox;

//shadowmaps
//directional light
GLuint shadowMapFBO;
GLuint depthMapTexture;

//pointlight
GLuint pointShadowMapFBO;
GLuint depthCubemap;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!autoScenePresentation) {
        glm::vec2 newMousePos = glm::vec2(xpos, ypos);

        if (firstMouse) {
            lastMousePos = newMousePos;
            firstMouse = false;
        }

        glm::vec2 mouseDelta = lastMousePos - newMousePos;
        lastMousePos = newMousePos;

        float pitch = mouseSensitivity * mouseDelta.y,
            yaw = mouseSensitivity * mouseDelta.x;
        totalPitch += pitch;
        if (totalPitch > 89.0f) {
            totalPitch = 89.0f;
            pitch = 0.0f;
        }
        else if (totalPitch < -89.0f) {
            totalPitch = -89.0f;
            pitch = 0.0f;
        }

        myCamera.rotate(pitch, yaw);
    }
}

void resetAutoScenePresentationMovementValues() {
    autoTurnCount = 185.0f;
    autoTurnAngle = 0.4f * counterCoeff;
    autoTurnNumber = 0;
}

void autoScenePresentationMovement() {
    float cameraSpeedWithDeltaTime = static_cast<float>(autoCameraSpeed * deltaTime);

    switch (autoTurnNumber) {
    case 0:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime, true);
            myCamera.rotate(0.0f, deltaTime * autoTurnAngle);
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            autoTurnAngle = 0.2f * counterCoeff;
            autoTurnCount = 100.0f;
            ++autoTurnNumber;
        }
        break;
    case 1:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime, true);
            myCamera.rotate(deltaTime * autoTurnAngle, 0.0f);
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            ++autoTurnNumber;
            autoTurnCount = 520.0f;
        }
        break;
    case 2:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime, true);
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            ++autoTurnNumber;
            autoTurnCount = 100.0f;
        }
        break;
    case 3:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime, true);
            myCamera.rotate(deltaTime  * (- autoTurnAngle), 0.0f);
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            autoTurnCount = 770.0f;
            ++autoTurnNumber;
        }
        break;
    case 4:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime, true);
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            autoTurnCount = 240.0f;
            ++autoTurnNumber;
        }
        break;
    case 5:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime, true);
            myCamera.rotate(0.0f, deltaTime  * (- autoTurnAngle));
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            autoTurnCount = 3100.0f;
            ++autoTurnNumber;
        }
        break;
    case 6:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime, true);
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            autoTurnAngle = 0.25f * counterCoeff;
            autoTurnCount = 350.f;
            ++autoTurnNumber;
        }
        break;
    case 7:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime, true);
            myCamera.rotate(0.0f, deltaTime * (-autoTurnAngle));
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            autoTurnCount = 300.f;
            ++autoTurnNumber;
        }
        break;
    case 8:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime / 2, true);
            myCamera.move(gps::MOVE_RIGHT, cameraSpeedWithDeltaTime / 2, true);
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            autoTurnCount = 4400.f;
            ++autoTurnNumber;
        }
        break;
    case 9:
        if (autoTurnCount > 0.0f) {
            myCamera.move(gps::MOVE_RIGHT, cameraSpeedWithDeltaTime, true);
            autoTurnCount -= deltaTime * counterCoeff;
        }
        else {
            ++autoTurnNumber;
        }
        break;
    default:
        resetAutoScenePresentationMovementValues();
        autoScenePresentation = false;
        myCamera = gps::Camera(moveCameraPosition, moveCameraTarget, moveCameraUp);
        break;
    }
}

void processMovement() {
    if (!autoScenePresentation) {
        float cameraSpeedWithDeltaTime;
        if (flyMode) {
            cameraSpeedWithDeltaTime = static_cast<float>(flyCameraSpeed * deltaTime);
        }
        else {
            cameraSpeedWithDeltaTime = static_cast<float>(walkCameraSpeed * deltaTime);
        }

        if (pressedKeys[GLFW_KEY_W]) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeedWithDeltaTime, flyMode);
        }

        if (pressedKeys[GLFW_KEY_S]) {
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeedWithDeltaTime, flyMode);
        }

        if (pressedKeys[GLFW_KEY_A]) {
            myCamera.move(gps::MOVE_LEFT, cameraSpeedWithDeltaTime, flyMode);
        }

        if (pressedKeys[GLFW_KEY_D]) {
            myCamera.move(gps::MOVE_RIGHT, cameraSpeedWithDeltaTime, flyMode);
        }

        if (pressedKeys[GLFW_KEY_SPACE]) {
            if (flyMode) {
                myCamera.move(gps::MOVE_UP, cameraSpeedWithDeltaTime, flyMode);
            }
        }

        if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
            if (flyMode) {
                myCamera.move(gps::MOVE_DOWN, cameraSpeedWithDeltaTime, flyMode);
            }
        }

        if (pressedKeys[GLFW_KEY_F]) {
            pressedKeys[GLFW_KEY_F] = false;
            flyMode = !flyMode;
        }
    }
    else {
        autoScenePresentationMovement();
    }

    if (pressedKeys[GLFW_KEY_Y]) {
        pressedKeys[GLFW_KEY_Y] = false;

        viewModeCurrent = ViewMode((viewModeCurrent + 1) % 4);
        
        switch(viewModeCurrent) {
        case SOLID:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_POLYGON_SMOOTH);
            break;
        case WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case POLYGONAL:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            //glEnable(GL_POLYGON_MODE);
            break;
        case SMOOTH:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_POLYGON_SMOOTH);
            break;
        default:
            break;
        }
    }

    if (pressedKeys[GLFW_KEY_I]) {
        pressedKeys[GLFW_KEY_I] = false;

        if (!autoScenePresentation) {
            myCamera = gps::Camera(autoCameraPosition, autoCameraTarget, autoCameraUp);
            resetAutoScenePresentationMovementValues();
            objectLoader.resetMovingObjectsPosition();
        }
        else {
            myCamera = gps::Camera(moveCameraPosition, moveCameraTarget, moveCameraUp);
        }

        autoScenePresentation = !autoScenePresentation;
    }

    if (pressedKeys[GLFW_KEY_G]) {
        pressedKeys[GLFW_KEY_G] = false;

        isFog = !isFog;
        shaderTexture.useShaderProgram();
        glUniform1i(glGetUniformLocation(shaderTexture.shaderProgram, "isFog"), isFog);
        shaderColorOnly.useShaderProgram();
        glUniform1i(glGetUniformLocation(shaderColorOnly.shaderProgram, "isFog"), isFog);
    }
}

void initOpenGLWindow() {
    myWindow.Create(windowWidth, windowHeight, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	//glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initShaders() {
	shaderTexture.loadShader("shaders/shaderTexture.vert", "shaders/shaderTexture.frag");
    shaderTexture.useShaderProgram();
    shaderColorOnly.loadShader("shaders/shaderColorOnly.vert", "shaders/shaderColorOnly.frag");
    shaderColorOnly.useShaderProgram();
    skyBoxShader.loadShader("shaders/skyBoxShader.vert", "shaders/skyBoxShader.frag");
    skyBoxShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    depthMapShader.useShaderProgram();
    //depthCubemapShader.loadShader("shaders/depthCubemap.vert", "shaders/depthCubemap.geom", "shaders/depthCubemap.frag");
    //depthCubemapShader.useShaderProgram();

}

void initObjectLoader() {
    objectLoader.initObjects();
    objectLoader.initUniforms(shaderTexture, shaderColorOnly, depthMapShader,
        myWindow, myCamera,
        projection, lightDir, lightColor, dirLightSpaceTrMatrix);
}

void initSkyBox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/front.tga");
    faces.push_back("skybox/back.tga");

    mySkyBox.Load(faces);
}

void initFBO() {
    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initPointFBO(GLuint &shadowMapFBO, GLuint &depthCubemap, glm::vec3 lightPos) {
    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            POINT_SHADOW_WIDTH, POINT_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float farPlane = 60.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, farPlane);
    glm::mat4 shadowTransforms[] =
    {
    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
    shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
    };
    // Export all matrices to shader
    depthCubemapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthCubemapShader.shaderProgram, "shadowMatrices[0]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[0]));
    glUniformMatrix4fv(glGetUniformLocation(depthCubemapShader.shaderProgram, "shadowMatrices[1]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[1]));
    glUniformMatrix4fv(glGetUniformLocation(depthCubemapShader.shaderProgram, "shadowMatrices[2]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[2]));
    glUniformMatrix4fv(glGetUniformLocation(depthCubemapShader.shaderProgram, "shadowMatrices[3]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[3]));
    glUniformMatrix4fv(glGetUniformLocation(depthCubemapShader.shaderProgram, "shadowMatrices[4]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[4]));
    glUniformMatrix4fv(glGetUniformLocation(depthCubemapShader.shaderProgram, "shadowMatrices[5]"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[5]));
    glUniform3f(glGetUniformLocation(depthCubemapShader.shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform1f(glGetUniformLocation(depthCubemapShader.shaderProgram, "farPlane"), farPlane);
}

void renderScene() {
    objectLoader.objectsMovement(myCamera, deltaTime);
    objectLoader.operateLights(shaderTexture, shaderColorOnly, deltaTime, counterCoeff);

    //calculating depth map for directional light (sun)
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    //glCullFace(GL_FRONT);
    objectLoader.drawObjects(depthMapShader, depthMapShader, myCamera, glm::mat4(1.0f), true);
    //glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //normal scene render
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    skyBoxShader.useShaderProgram();
    mySkyBox.Draw(skyBoxShader, view, projection);

    glEnable(GL_DEPTH_TEST);

    view = myCamera.getViewMatrix();
    shaderTexture.useShaderProgram();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(shaderTexture.shaderProgram, "depthMap"), 3);
    glUniformMatrix4fv(glGetUniformLocation(shaderTexture.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    shaderColorOnly.useShaderProgram();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(shaderColorOnly.shaderProgram, "depthMap"), 3);
    glUniformMatrix4fv(glGetUniformLocation(shaderColorOnly.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

    objectLoader.drawObjects(shaderTexture, shaderColorOnly, myCamera, view, false);
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initShaders();
    //initPointFBO(pointShadowMapFBO, depthCubemap, glm::vec3(0.0f));
    initObjectLoader();
    initSkyBox();
    initFBO();
    setWindowCallbacks();

    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
