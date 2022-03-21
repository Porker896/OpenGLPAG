// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>

#include <glad/glad.h>  // Initialize with gladLoadGL()
#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Object.h"

float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;


bool firstMouse = true;
bool wireframe = false;
bool cursor = false;


Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void processInput(GLFWwindow* window, float deltaTime)
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

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		if (cursor)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cursor = false;
			firstMouse = true;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cursor = true;
			int w, h;
			glfwGetWindowSize(window, &w, &h);
			glfwSetCursorPos(window, w / 2, h / 2);
		}
	}

}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (!cursor)
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

}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!cursor)
		camera.ProcessMouseScroll(yoffset);
}

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		return 1;

	// GL 4.3 + GLSL 430
	const char* glsl_version = "#version 430";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "3 - Neighbourhood", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSwapInterval(1); // Enable vsync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();

	ImVec4 clear_color = ImVec4(.22f, .22f, .22f, 1.00f);


	//Shader basicShader("res/shaders/basic.vert", "res/shaders/basic.frag");
	Shader lightShader("res/shaders/light.vert", "res/shaders/light.frag");

	//Object nanosuit("res/models/nanosuit/nanosuit.obj", &basicShader);
	Object cube("res/models/cube/cube.obj", &lightShader);

	//Model pointLight("res/models/cube/cube.obj");
	//Model spotLight("res/models/cone/cone.obj");

	float deltaTime = 0;
	float lastFrame = 0;

	//DIR LIGHT PROPERTIES
	glm::vec3 direction(0, 0, 0);
	glm::vec3 ambient(0);
	glm::vec3 diffuse(0);
	glm::vec3 specular(0);

	//POINT LIGHT PROPERTIES
	bool isPointLight = false;
	glm::vec3 pointLightPosition(0.0f);
	glm::vec3 pointLightAmbient(1.0f);
	glm::vec3 pointLightDiffuse(1.0f);
	glm::vec3 pointLightSpecular(1.0f);
	float pointLightConstant = 1.0f;
	float pointLightLinear = .7f;
	float pointLightQuadratic = 1.8f;


	//SPOTLIGHT PROPERTIES
	bool isSpotActive = false;
	glm::vec3 spotLightDirection;
	glm::vec3 spotLightAmbient;
	glm::vec3 spotLightDiffuse;
	glm::vec3 spotLightSpecular;
	float spotLightConstant;
	float spotLightLinear;
	float spotLightQuadratic;

	//SPOTLIGHT 1 PROPERTIES 
	bool isSpot1Active = false;
	glm::vec3 spotLight1Direction;
	glm::vec3 spotLight1Ambient;
	glm::vec3 spotLight1Diffuse;
	glm::vec3 spotLight1Specular;
	float spotLight1Constant;
	float spotLight1Linear;
	float spotLight1Quadratic;

	static float shininess = 0.0f;
	static bool isDirLight = true;


	// Main loop
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		processInput(window, deltaTime);

		//nanosuit.Update();

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Inspector");

			ImGui::Text("Material");
			ImGui::SliderFloat("Shininess", &shininess, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color));
			ImGui::Checkbox("Directional light", &isDirLight);
			ImGui::SliderFloat3("Direction", glm::value_ptr(direction), -1.0f, 1.0f);
			ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient));
			ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
			ImGui::ColorEdit3("Specular", glm::value_ptr(specular));
			
			ImGui::Text("POINT LIGHT");
			ImGui::Checkbox("Point light", &isPointLight);
			ImGui::DragFloat3("Point light position", glm::value_ptr(pointLightPosition),
				1,-10,10);
			ImGui::ColorEdit3("Point light ambient", glm::value_ptr(pointLightAmbient));
			ImGui::ColorEdit3("Point light diffuse", glm::value_ptr(pointLightDiffuse));
			ImGui::ColorEdit3("Point light specular", glm::value_ptr(pointLightSpecular));
			ImGui::InputFloat("Point light constant", &pointLightConstant);
			ImGui::InputFloat("Point light linear", &pointLightLinear);
			ImGui::InputFloat("Point light quadratic", &pointLightQuadratic);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.1f, 100.0f);
		glm::mat4 VP = projection * camera.GetViewMatrix();

		//basicShader.use();
		//basicShader.setMat4("VP", VP);
		lightShader.use();
		lightShader.setMat4("VP", VP);
		lightShader.setVec3("viewPos", camera.Position);

		lightShader.setFloat("shininess", shininess);


		lightShader.setBool("dirLight.isActive", isDirLight);
		lightShader.setVec3("dirLight.direction", direction);
		lightShader.setVec3("dirLight.ambient", ambient);
		lightShader.setVec3("dirLight.diffuse", diffuse);
		lightShader.setVec3("dirLight.specular", specular);

		//POINT LIGHT
		lightShader.setBool("pointLights[0].isActive", isPointLight);
		
		lightShader.setVec3("pointLights[0].position", pointLightPosition);
		lightShader.setFloat("pointLights[0].constant", pointLightConstant);
		lightShader.setFloat("pointLights[0].linear", pointLightLinear);
		lightShader.setFloat("pointLights[0].quadratic", pointLightQuadratic);
		
		lightShader.setVec3("pointLights[0].ambient", pointLightAmbient);
		lightShader.setVec3("pointLights[0].diffuse", pointLightDiffuse);
		lightShader.setVec3("pointLights[0].ambient", pointLightSpecular);
		
		////POINT LIGHT 1
		//lightShader.setBool("pointLight[1].isActive", pointLight[1].isActive);
		//
		//lightShader.setVec3("pointLight[1].position", pointLight[1].position);
		//lightShader.setFloat("pointLight[1].constant", pointLight[1].constant);
		//lightShader.setFloat("pointLight[1].linear", pointLight[1].linear);
		//lightShader.setFloat("pointLight[1].quadratic", pointLight[1].quadratic);
		//
		//lightShader.setVec3("pointLight[1].ambient", pointLight[1].ambient);
		//lightShader.setVec3("pointLight[1].diffuse", pointLight[1].diffuse);
		//lightShader.setVec3("pointLight[1].ambient", pointLight[1].specular);
		//
		////SPOT LIGHT 
		//lightShader.setBool("spotLight[0].isActive", spotLight[0].isActive);
		//
		//lightShader.setVec3("spotLight[0].position", spotLight[0].position);
		//lightShader.setFloat("spotLight[0].constant", spotLight[0].constant);
		//lightShader.setFloat("spotLight[0].linear", spotLight[0].linear);
		//lightShader.setFloat("spotLight[0].quadratic", spotLight[0].quadratic);
		//
		//lightShader.setVec3("spotLight[0].ambient", spotLight[0].ambient);
		//lightShader.setVec3("spotLight[0].diffuse", spotLight[0].diffuse);
		//lightShader.setVec3("spotLight[0].ambient", spotLight[0].specular);
		//

		//nanosuit.Draw();
		cube.Draw();

		// Rendering
		ImGui::Render();

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}