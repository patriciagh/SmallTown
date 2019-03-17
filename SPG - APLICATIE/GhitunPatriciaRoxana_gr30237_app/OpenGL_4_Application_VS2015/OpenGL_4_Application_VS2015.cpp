//
//  main.cpp
//  OpenGL Shadows
//
//	GHITUN PATRICIA ROXANA
//	GRUPA 30237
//
//  Created by CGIS on 05/12/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(glm::vec3(0.0f, 1.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
GLfloat cameraSpeed = 0.1f;

bool pressedKeys[1024];
GLfloat angle;
GLfloat lightAngle;

//Obiecte
gps::Model3D ground;//scena
gps::Model3D vultur;
gps::Model3D mana;
gps::Model3D rainbow;
gps::Model3D moara;
gps::Model3D roata;
gps::Model3D skydome;


gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

//SKYBOX
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;


//CEATA
int controlCeata = 0;
GLint locatieCeata;

//LUMINA PUNCTIFORMA - in centru
int controlPunctiforma = 0;
glm::vec3 lightPosition;
GLuint lightPositionLoc;

//SPOT LIGHT
float flashLightCutoff1;
float flashLightCutoff2;
int start_spotlight = 0;
glm::vec3 flashLightDirection;
glm::vec3 flashLightPosition;
//SPOT LIGHT

//VARIABILE ANIMATII
GLfloat animatieVultur = 0.0f;
GLfloat animatieMana = 0.0f;
GLfloat animatieCurcubeu = 0.0f;
GLfloat animatieRoata = 0.0f;

int checkCurcubeu = 0;

//ANIMATIE CAMERA
bool animatieCamera = false;
float cameraAngle;


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)


void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	lightShader.useShaderProgram();
	
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

// pentru mouse 360
float lastX = 320;
float lastY = 240;
float yaw, pitch;
GLboolean firstMouse = true;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw);
}

void processMovement()
{

	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 0.1f;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 0.1f;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		// misca inainte elefantul
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	//wireframe, points, normal

	if (pressedKeys[GLFW_KEY_P]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_O]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_0]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_J]) {

		lightAngle += 0.3f;
		if (lightAngle > 360.0f)
			lightAngle -= 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle -= 0.3f; 
		if (lightAngle < 0.0f)
			lightAngle += 360.0f;
		glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
		myCustomShader.useShaderProgram();
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDirTr));
	}	

	//CEATA
	if (glfwGetKey(glWindow, GLFW_KEY_U) == GLFW_PRESS) 
	{
		//DISPARE CEATA
		myCustomShader.useShaderProgram();
		controlCeata = 0;
		locatieCeata = glGetUniformLocation(myCustomShader.shaderProgram, "controlCeata");
		glUniform1i(locatieCeata, controlCeata);

	}

	if (glfwGetKey(glWindow, GLFW_KEY_I) == GLFW_PRESS) 
	{
		// APARE CEATA
		myCustomShader.useShaderProgram();// modific ceva pe ecran -> folosesc asta ca sa faca randeze.
		controlCeata = 1; 
		locatieCeata = glGetUniformLocation(myCustomShader.shaderProgram, "controlCeata");
		glUniform1i(locatieCeata, controlCeata);

	}
	
	//LUMINA PUNCTIFORMA - START
	if (glfwGetKey(glWindow, GLFW_KEY_V) == GLFW_PRESS)
	{
		myCustomShader.useShaderProgram();
		controlPunctiforma = 1;
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram , "controlPunctiforma") , controlPunctiforma);
	}
	//LUMINA PUNCTIFORMA - STOP
	if (glfwGetKey(glWindow, GLFW_KEY_B) == GLFW_PRESS)
	{
		myCustomShader.useShaderProgram();
		controlPunctiforma = 0;
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "controlPunctiforma"), controlPunctiforma);
	}


	//LUMINA SPOTLIGHT - START
	if (glfwGetKey(glWindow, GLFW_KEY_N) == GLFW_PRESS)
	{
		myCustomShader.useShaderProgram();
		start_spotlight = 1;
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "start_spotlight"), start_spotlight);
	}
	//LUMINA SPOTLIGHT - STOP
	if (glfwGetKey(glWindow, GLFW_KEY_M) == GLFW_PRESS)
	{
		myCustomShader.useShaderProgram();
		start_spotlight = 0;
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "start_spotlight"), start_spotlight);
	}

	//CURCUBEU - START
	if (pressedKeys[GLFW_KEY_X]) {

		checkCurcubeu = 1;
	}
	//CURCUBEU STOP
	if (pressedKeys[GLFW_KEY_C]) {

		checkCurcubeu = 3;		
	}

	// START animatie camera
	if (pressedKeys[GLFW_KEY_8]) {
		animatieCamera = true;
	}

	//STOP animatie camera
	if (pressedKeys[GLFW_KEY_9]) {
		animatieCamera = false;
	}

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
    //glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //functie ca sa nu iasa mouseul din fereastra
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBOs()
{
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 1.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));
	return lightProjection * lightView;
}

void initModels()
{
	vultur = gps::Model3D("objects/Vultur/vultur.obj", "objects/Vultur/");
	ground = gps::Model3D("objects/scena/proiect_nou.obj", "objects/scena/");
	mana = gps::Model3D("objects/Hand/hand.obj", "objects/Hand/");
	rainbow = gps::Model3D("objects/Rainbow/rainbow.obj", "objects/Rainbow/");
	moara = gps::Model3D("objects/Moara/moara.obj", "objects/Moara/");
	roata = gps::Model3D("objects/Roata/roata.obj", "objects/Roata/");
	skydome = gps::Model3D("objects/Skydome/skydome.obj", "objects/Skydome/");
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	depthMapShader.loadShader("shaders/simpleDepthMap.vert", "shaders/simpleDepthMap.frag");
}

void cameraAnimationFunction(float angle) 
{
	if (animatieCamera == true) {
		cameraAngle = cameraAngle +  0.2f;
		myCamera.sceneVisualization(cameraAngle);
	}
}

void initUniforms()// AICI TRIMIT SPRE SHADERE
{
	myCustomShader.useShaderProgram();

	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	
	lightDirMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDirMatrix");

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 2.0f);// *10.0f;

	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color - pentru lumina directionala , o trimit si pentru lumina punctiforma
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//LUMINA PUNCTIFORMA
	lightPosition = glm::vec3(1.0f, 1.0f, -2.0f);//coordonate lumina punctiforma
	lightPositionLoc = glGetUniformLocation(myCustomShader.shaderProgram , "lightPosition");
	glUniform3fv(lightPositionLoc , 1, glm::value_ptr(lightPosition));


	//LUMINA SPOT
	flashLightCutoff1 = glm::cos(glm::radians(40.5f));
	flashLightCutoff2 = glm::cos(glm::radians(100.5f));

	flashLightDirection = glm::vec3(0, -1, 0);
	flashLightPosition = glm::vec3(0.0f, 0.0f, -9.0f);//coordonate lumina spotlight

	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "flashLightCutoff1"), flashLightCutoff1);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "flashLightCutoff2"), flashLightCutoff2);

	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "flashLightDirection"), 1, glm::value_ptr(flashLightDirection));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "flashLightPosition"), 1, glm::value_ptr(flashLightPosition));
	//LUMINA SPOT

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	processMovement();	

	//PENTRU A PORNI ANIMATIA CAMEREI
	double time = glfwGetTime();
	cameraAnimationFunction(time);


	//UMBRE
	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
		
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	

#pragma region Umbra_Scena
	
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"),	1,	GL_FALSE,glm::value_ptr(model));
	ground.Draw(depthMapShader);

#pragma endregion

#pragma region Umbra_Vultur
	
	//drawing the vultur 
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	//se misca in jurul lui
	model = glm::translate(model, glm::vec3(0.0f, 70.0f, -115.0f));
	model = glm::rotate(model, glm::radians(animatieVultur), glm::vec3(0, 1, 0));
	//se misca in jurul biserici
	model = glm::translate(model, glm::vec3(20.0f, 20.0f, 0.0f));
	// am rotit pasarea 
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0, 1, 0));
	
	//send model matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	vultur.Draw(depthMapShader);

#pragma endregion

#pragma region Umbra_Mana
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	model = glm::translate(model, glm::vec3(15.0f, animatieMana, -35.0f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0, 1, 0));

	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	mana.Draw(depthMapShader);
#pragma endregion

#pragma region Umbra_Moara
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
	model = glm::translate(model, glm::vec3(0.0f, -2.8f, -58.0f));
	//send model matrix
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	moara.Draw(depthMapShader);
#pragma endregion

#pragma region Umbra_Roata
	//create model matrix for ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 1.45f, 8.2f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(animatieRoata), glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	roata.Draw(depthMapShader);
#pragma endregion 


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//OBIECTE

	myCustomShader.useShaderProgram();

	//send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	//send view matrix to shader
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"),
		1,
		GL_FALSE,
		glm::value_ptr(view));	

	//compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	//send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix)); //NORMALA

	glViewport(0, 0, retina_width, retina_height);
	myCustomShader.useShaderProgram();

	//bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);	

#pragma region Scena
	//create model matrix for ground
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	//send model matrix data to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	ground.Draw(myCustomShader);
#pragma endregion

#pragma region Vultur
	
	//drawing the vultur 
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	
	//se misca in jurul lui
	model = glm::translate(model , glm::vec3(0.0f , 70.0f , -115.0f));
	model = glm::rotate(model, glm::radians(animatieVultur), glm::vec3(0, 1, 0));
	//se misca in jurul biserici
	model = glm::translate(model, glm::vec3(20.0f, 20.0f, 0.0f));
	// am rotit pasarea 
	model = glm::rotate(model , glm::radians(-180.0f),glm::vec3(0,1,0));
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	vultur.Draw(myCustomShader);

	
#pragma endregion

#pragma region Mana

	//drawing the vultur 
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));	
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	model = glm::translate(model, glm::vec3(15.0f, animatieMana, -35.0f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0, 1, 0));
	
	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	mana.Draw(myCustomShader);

#pragma endregion

#pragma region Curcubeu
	//drawing the vultur 
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0.0f, -4.0f, 12.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0, 0, 1));
	model = glm::rotate(model, glm::radians(animatieCurcubeu), glm::vec3(0, 0, 1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	rainbow.Draw(myCustomShader);
#pragma endregion

#pragma region Moara
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));	
	model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
	model = glm::translate(model, glm::vec3(0.0f, -2.8f, -58.0f));
	//send model matrix
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//send normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	moara.Draw(myCustomShader);
#pragma endregion

#pragma region Roata

	//create model matrix for ground
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 1.45f, 8.2f));
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(animatieRoata), glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	roata.Draw(myCustomShader);

#pragma endregion	
	
#pragma region SkyBox
	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, lightDir);
	model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//lightCube.Draw(lightShader);
	mySkyBox.Draw(skyboxShader, view, projection);
#pragma endregion
}


int main(int argc, const char * argv[]) 
{

	initOpenGLWindow();
	initOpenGLState();
	initFBOs();
	initModels();
	initShaders();
	initUniforms();	

	//SKYBOX
	std::vector<const GLchar*> faces;
	faces.push_back("textures/skybox/right.tga");
	faces.push_back("textures/skybox/left.tga");
	faces.push_back("textures/skybox/top.tga");
	faces.push_back("textures/skybox/bottom.tga");
	faces.push_back("textures/skybox/back.tga");
	faces.push_back("textures/skybox/front.tga");
	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
	glCheckError();
	

	int checkMana = 0;
	while (!glfwWindowShouldClose(glWindow)) 
	{
		renderScene();
		//animatie vultur
		if (animatieVultur < 360.0f)
		{
			animatieVultur = animatieVultur + 0.5f;
		}
		else
		{
			animatieVultur = 0.0f;
		}
		
		//animatie mana
		if (checkMana == 0)
		{
			if (animatieMana < -2.4f)
			{
				animatieMana = animatieMana + 0.01f;
			}
			else
			{
				checkMana = 1;// am ajuns sus
			}
		}
		else
		{
			//sunt sus si merg in jos
			if (animatieMana > -5.7f)
			{
				animatieMana = animatieMana - 0.01f;
			}
			else
			{
				checkMana = 0;
			}
		}

		//ANIMATIE CURCUBEU
		//sa apara 
		if (checkCurcubeu == 1)
		{
			if (animatieCurcubeu <= 180.0f)
			{
				animatieCurcubeu = animatieCurcubeu + 1.0f;
			}
			else
			{
				checkCurcubeu = 2;
			}
		}
		
		//sa plece
		if (checkCurcubeu == 3)
		{
			if (animatieCurcubeu >= 180.0f && animatieCurcubeu <360.0f)
			{
				animatieCurcubeu = animatieCurcubeu + 1.0f;
			}
			else
			{
				animatieCurcubeu = 0.0f;
			}
		}

		//ANIMATIE ROATA
		if (animatieRoata < 360.0f)
		{
			animatieRoata = animatieRoata + 0.5f;
		}
		else
		{
			animatieRoata = 0.0f;
		}
		
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}
	glfwTerminate();

	return 0;
}
