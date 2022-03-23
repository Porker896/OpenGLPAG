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
#include "LightObject.h"

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

	Shader basicShader("res/shaders/basic.vert", "res/shaders/basic.frag");
	Shader lightShader("res/shaders/light.vert", "res/shaders/light.frag");

	//Object nanosuit("res/models/nanosuit/nanosuit.obj", &basicShader);
	Object cube("res/models/cube/cube.obj", &lightShader);
	Object roof("res/models/pyramid/pyramid.obj", &lightShader);
	Object spotLightGizmo("res/models/pyramid/pyramid.obj", &basicShader);
	Object pointLight("res/models/cube/cube.obj", &basicShader);

	//std::vector<LightObject> spotLights;
	//spotLights.emplace_back(spotLightGizmo);

	cube.AddChild(&roof);
	cube.AddChild(&pointLight);
	cube.AddChild(&spotLightGizmo);

	spotLightGizmo.transform->setLocalScale({.2f,.2f,.2f});
	pointLight.transform->setLocalScale({.2f,.2f,.2f});
	roof.transform->setLocalPosition({0,2.0f,0});

	cube.Update();

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
	glm::vec3 spotLightPosition(0.0f);
	glm::vec3 spotLightDirection(0.0f);
	glm::vec3 spotLightAmbient(1.0f);
	glm::vec3 spotLightDiffuse(1.0f);
	glm::vec3 spotLightSpecular(1.0f);
	float spotLightConstant = 1.0f;
	float spotLightLinear = .7f;
	float spotLightQuadratic = 1.8f;
	float spotLightCutOff = 12.5f;
	float spotLightOuterCutOff = 17.5f;

	bool isSpot1Active = false;
	glm::vec3 spotLight1Position(0.0f);
	glm::vec3 spotLight1Direction(0.0f);
	glm::vec3 spotLight1Ambient(1.0f);
	glm::vec3 spotLight1Diffuse(1.0f);
	glm::vec3 spotLight1Specular(1.0f);
	float spotLight1Constant = 1.0f;
	float spotLight1Linear = .7f;
	float spotLight1Quadratic = 1.8f;
	float spotLight1CutOff = 12.5f;
	float spotLight1OuterCutOff = 17.5f;

	static float shininess = 2.0f;
	static bool isDirLight = false;



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
			ImGui::SliderFloat("Shininess", &shininess, 0.0f, 256.0f);
			ImGui::ColorEdit3("clear color", reinterpret_cast<float*>(&clear_color));
			ImGui::Checkbox("Directional light", &isDirLight);
			ImGui::SliderFloat3("Direction", glm::value_ptr(direction), -1.0f, 1.0f);
			ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient));
			ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
			ImGui::ColorEdit3("Specular", glm::value_ptr(specular));

			ImGui::Text("POINT LIGHT");
			ImGui::Checkbox("Point light", &isPointLight);
			ImGui::DragFloat3("Point light position", glm::value_ptr(pointLightPosition),
				.1f, -10.0f, 10.0f);
			ImGui::ColorEdit3("Point light ambient", glm::value_ptr(pointLightAmbient));
			ImGui::ColorEdit3("Point light diffuse", glm::value_ptr(pointLightDiffuse));
			ImGui::ColorEdit3("Point light specular", glm::value_ptr(pointLightSpecular));
			ImGui::InputFloat("Point light constant", &pointLightConstant);
			ImGui::InputFloat("Point light linear", &pointLightLinear);
			ImGui::InputFloat("Point light quadratic", &pointLightQuadratic);

			ImGui::Text("SPOT LIGHT");
			ImGui::Checkbox("Spot light", &isSpotActive);
			ImGui::DragFloat3("Spot light position", glm::value_ptr(spotLightPosition),.1f, -10.0f,10.0f);
			ImGui::SliderFloat3("Spot light direction", glm::value_ptr(spotLightDirection), -1.0f, 1.0f);

			ImGui::ColorEdit3("Spot light ambient", glm::value_ptr(spotLightAmbient));
			ImGui::ColorEdit3("Spot light diffuse", glm::value_ptr(spotLightDiffuse));
			ImGui::ColorEdit3("Spot light specular", glm::value_ptr(spotLightSpecular));
			ImGui::InputFloat("Spot light constant", &spotLightConstant);
			ImGui::InputFloat("Spot light linear", &spotLightLinear);
			ImGui::InputFloat("Spot light quadratic", &spotLightQuadratic);
			ImGui::InputFloat("Spot light cut off", &spotLightCutOff);
			ImGui::InputFloat("Spot light outer cut off", &spotLightOuterCutOff);


			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.1f, 100.0f);
		glm::mat4 VP = projection * camera.GetViewMatrix();


		lightShader.use();
		lightShader.setMat4("VP", VP);
		lightShader.setVec3("viewPos", camera.Position);

		lightShader.setFloat("shininess", shininess);

		lightShader.setBool("dirLight.isActive", isDirLight);
		lightShader.setVec3("dirLight.direction", direction);
		lightShader.setVec3("dirLight.colors.ambient", ambient);
		lightShader.setVec3("dirLight.colors.diffuse", diffuse);
		lightShader.setVec3("dirLight.colors.specular", specular);

		//POINT LIGHT
		lightShader.setBool("pointLights[0].isActive", isPointLight);

		lightShader.setVec3("pointLights[0].position", pointLightPosition);
		lightShader.setFloat("pointLights[0].att.constant", pointLightConstant);
		lightShader.setFloat("pointLights[0].att.linear", pointLightLinear);
		lightShader.setFloat("pointLights[0].att.quadratic", pointLightQuadratic);

		lightShader.setVec3("pointLights[0].colors.ambient", pointLightAmbient);
		lightShader.setVec3("pointLights[0].colors.diffuse", pointLightDiffuse);
		lightShader.setVec3("pointLights[0].colors.specular", pointLightSpecular);

		//SPOT LIGHT 
		lightShader.setBool("spotLights[0].isActive", isSpotActive);
		
		lightShader.setVec3("spotLights[0].position", spotLightPosition);
		lightShader.setVec3("spotLights[0].direction", spotLightDirection);

		lightShader.setFloat("spotLights[0].att.constant", spotLightConstant);
		lightShader.setFloat("spotLights[0].att.linear", spotLightLinear);
		lightShader.setFloat("spotLights[0].att.quadratic", spotLightQuadratic);
		
		lightShader.setVec3("spotLights[0].colors.ambient", spotLightAmbient);
		lightShader.setVec3("spotLights[0].colors.diffuse", spotLightDiffuse);
		lightShader.setVec3("spotLights[0].colors.specular", spotLightSpecular);

		lightShader.setFloat("spotLights[0].cutOff", glm::radians(spotLightCutOff));
		lightShader.setFloat("spotLights[0].outerCutOff", glm::radians(spotLightOuterCutOff));
		
		//SPOT LIGHT 
		lightShader.setBool("spotLights[1].isActive", isSpot1Active);
		
		lightShader.setVec3("spotLights[1].position", spotLight1Position);
		lightShader.setVec3("spotLights[1].direction", spotLight1Direction);
		lightShader.setFloat("spotLights[1].att.constant", spotLight1Constant);
		lightShader.setFloat("spotLights[1].att.linear", spotLight1Linear);
		lightShader.setFloat("spotLights[1].att.quadratic", spotLight1Quadratic);
		
		lightShader.setVec3("spotLights[1].colors.ambient", spotLight1Ambient);
		lightShader.setVec3("spotLights[1].colors.diffuse", spotLight1Diffuse);
		lightShader.setVec3("spotLights[1].colors.specular", spotLight1Specular);

		lightShader.setFloat("spotLights[1].cutOff", spotLight1CutOff);
		lightShader.setFloat("spotLights[1].outerCutOff", spotLight1OuterCutOff);
		
		basicShader.use();
		basicShader.setMat4("VP", VP);
		basicShader.setVec3("diffuse", pointLightDiffuse * pointLightAmbient * pointLightSpecular);

		pointLight.transform->setLocalPosition(pointLightPosition);
		spotLightGizmo.transform->setLocalPosition(spotLightPosition);
		spotLightGizmo.transform->setLocalRotation(180.0f * spotLightDirection);

		cube.transform->setLocalPosition({0,0,0});

		cube.Update();

		pointLight.Draw();
		spotLightGizmo.Draw();

		lightShader.use();
		roof.Draw();
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