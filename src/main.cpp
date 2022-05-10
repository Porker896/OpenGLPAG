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
#include "Cubemap.h"
#include "Object.h"

float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;


bool firstMouse = true;
bool wireframe = false;
bool cursor = false;


Camera camera(glm::vec3(0.0f, 5.0f, 0.0f));

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

	//if (glfwGetKey(window, GLFW_MOUSE_BUTTON_1))
	//	return;

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		printf("RRRRRRR\n");

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
	if (window == nullptr)
		return 1;

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	glfwSwapInterval(0); // Enable vsync
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
	Shader texturedShader("res/shaders/textured.vert", "res/shaders/light.frag");
	Shader cubemapShader("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");
	Shader gunShader("res/shaders/gun.vert", "res/shaders/light.frag");

	static const std::vector<std::string> f =
	{
	"res/textures/skybox/right.jpg",
	"res/textures/skybox/left.jpg",
	"res/textures/skybox/top.jpg",
	"res/textures/skybox/bottom.jpg",
	"res/textures/skybox/front.jpg",
	"res/textures/skybox/back.jpg",
	};

	Cubemap cubemap(f, &cubemapShader);
	float deltaTime = 0;
	float lastFrame = 0;

	float shininess = 2.0f;

	//DIR LIGHT PROPERTIES
	bool isDirLight = true;
	glm::vec3 direction(0, 0, 0);
	glm::vec3 ambient(.19f);
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

	//instanced matrices preparation
	int rows = 200, columns = 200;
	int amount = rows * columns;

	std::vector<Transform*> houseTransforms;
	std::vector<Transform*> roofTransforms;

	auto cubeModel = new Model("res/models/cube/cube.obj");
	auto pyramidModel = new Model("res/models/pyramid/pyramid.obj");
	auto plane = new Model("res/models/plane/plane.obj");

	auto neighbourhood = new Object(plane, &texturedShader);

	auto neighTransform = &neighbourhood->transform;

	glm::mat4 temp = glm::translate(glm::mat4(1.0f), { -400,0,-400 });
	for (auto i = 0; i < columns; i++)
	{
		glm::mat4 model(1.0f);
		for (auto j = 0; j < rows; j++)
		{
			auto houseTransform = new Transform();
			auto roofTransform = new Transform();

			temp = glm::translate(temp, glm::vec3(3.0f, 0.0f, 0.0f));
			model = glm::translate(temp, { 0,1.5f,0 });

			houseTransform->SetModelMatrix(model);
			houseTransform->SetParent(neighTransform);

			houseTransforms.emplace_back(houseTransform);

			roofTransform->SetLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));
			roofTransform->SetParent(houseTransforms.back());

			roofTransforms.emplace_back(roofTransform);
		}
		temp = glm::translate(temp, glm::vec3(-1.0f * static_cast<float>(rows) * 3.0f, 0.0f, 3.0f));
	}

	auto house = new InstancedObject(cubeModel, &lightShader, houseTransforms);
	auto roof = new InstancedObject(pyramidModel, &lightShader, roofTransforms);

	Object spotLightGizmo("res/models/pyramid/pyramid.obj", &basicShader);
	Object spotLight1Gizmo("res/models/pyramid/pyramid.obj", &basicShader);
	Object pointLight("res/models/cube/cube.obj", &basicShader);
	Object gun("res/models/gun/Handgun_obj.obj", &gunShader);
	
	gun.transform.SetLocalRotation({0,90.0f,10.0f});
	gun.transform.SetLocalRotationZ(10.0f);
	gun.transform.SetLocalPosition({0,-.8,-1.5});
	gun.transform.Update();

	const auto gizmoScale = glm::vec3(0.2f);
	spotLightGizmo.transform.SetLocalScale(gizmoScale);
	spotLight1Gizmo.transform.SetLocalScale(gizmoScale);
	pointLight.transform.SetLocalScale(gizmoScale);

	spotLightGizmo.transform.SetParent(neighTransform);
	spotLight1Gizmo.transform.SetParent(neighTransform);
	pointLight.transform.SetParent(neighTransform);

	neighbourhood->Update();

	int chosenBuilding = 0;

	glm::vec3 buildingLocalPos(0.0f);
	glm::vec3 prevBuildingLocalPos = buildingLocalPos;
	glm::vec3 housesLocalPos(0.0f); //house.transform->GetLocalPosition();
	glm::vec3 prevHousesLocalPos = housesLocalPos;
	glm::vec3 neigbourhoodLocalPos(0.0f);
	glm::vec3 prevNeigbourhoodLocalPos = neigbourhoodLocalPos;

	// Main loop
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	unsigned ammoCount = 10;
	while (!glfwWindowShouldClose(window))
	{
		auto currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		processInput(window, deltaTime);

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//UI
		{
			ImGui::Begin("Inspector");

			ImGui::InputInt("Chosen building", &chosenBuilding);
			ImGui::SliderFloat3("Building local pos", glm::value_ptr(buildingLocalPos), -10.0f, 10.0f);
			ImGui::InputFloat3("Plane local pos", glm::value_ptr(housesLocalPos));

			ImGui::InputFloat3("N loc", glm::value_ptr(neigbourhoodLocalPos));

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
			//ImGui::DragFloat3("Point light position", glm::value_ptr(pointLightPosition),
			//	.1f, -10.0f, 10.0f);
			ImGui::ColorEdit3("Point light ambient", glm::value_ptr(pointLightAmbient));
			ImGui::ColorEdit3("Point light diffuse", glm::value_ptr(pointLightDiffuse));
			ImGui::ColorEdit3("Point light specular", glm::value_ptr(pointLightSpecular));
			ImGui::InputFloat("Point light constant", &pointLightConstant);
			ImGui::InputFloat("Point light linear", &pointLightLinear);
			ImGui::InputFloat("Point light quadratic", &pointLightQuadratic);

			ImGui::Text("SPOT LIGHT");
			ImGui::Checkbox("Spot light", &isSpotActive);
			ImGui::DragFloat3("Spot light position", glm::value_ptr(spotLightPosition), .1f, -10.0f, 10.0f);
			ImGui::SliderFloat3("Spot light direction", glm::value_ptr(spotLightDirection), -1.0f, 1.0f);

			ImGui::ColorEdit3("Spot light ambient", glm::value_ptr(spotLightAmbient));
			ImGui::ColorEdit3("Spot light diffuse", glm::value_ptr(spotLightDiffuse));
			ImGui::ColorEdit3("Spot light specular", glm::value_ptr(spotLightSpecular));
			ImGui::InputFloat("Spot light constant", &spotLightConstant);
			ImGui::InputFloat("Spot light linear", &spotLightLinear);
			ImGui::InputFloat("Spot light quadratic", &spotLightQuadratic);
			ImGui::InputFloat("Spot light cut off", &spotLightCutOff);
			ImGui::InputFloat("Spot light outer cut off", &spotLightOuterCutOff);

			ImGui::Text("SPOT LIGHT 1");
			ImGui::Checkbox("Spot light 1", &isSpot1Active);
			ImGui::DragFloat3("Spot light 1 position", glm::value_ptr(spotLight1Position), .1f, -10.0f, 10.0f);
			ImGui::SliderFloat3("Spot light 1 direction", glm::value_ptr(spotLight1Direction), -1.0f, 1.0f);

			ImGui::ColorEdit3("Spot light 1 ambient", glm::value_ptr(spotLight1Ambient));
			ImGui::ColorEdit3("Spot light 1 diffuse", glm::value_ptr(spotLight1Diffuse));
			ImGui::ColorEdit3("Spot light 1 specular", glm::value_ptr(spotLight1Specular));
			ImGui::InputFloat("Spot light 1 constant", &spotLight1Constant);
			ImGui::InputFloat("Spot light 1 linear", &spotLight1Linear);
			ImGui::InputFloat("Spot light 1 quadratic", &spotLight1Quadratic);
			ImGui::InputFloat("Spot light 1 cut off", &spotLight1CutOff);
			ImGui::InputFloat("Spot light 1 outer cut off", &spotLight1OuterCutOff);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		{
			ImGui::Begin("Ammo");
			ImGui::Text("%d", ammoCount);
			ImGui::End();
		}

		pointLightPosition = pointLight.transform.GetLocalPosition();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.1f, 100.0f);
		glm::mat4 VP = projection * camera.GetViewMatrix();
		glm::mat4 skyboxVP = projection * glm::mat4(glm::mat3(camera.GetViewMatrix())); //remove translation

		//...::SHADER UPDATES::...
		lightShader.use();
		lightShader.setMat4("VP", VP);
		lightShader.setVec3("viewPos", camera.Position);

		lightShader.setFloat("shininess", shininess);
		lightShader.setVec3("offset", buildingLocalPos);
		lightShader.setInt("chosenInstance", chosenBuilding);

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

		lightShader.setFloat("spotLights[0].cutOff", glm::cos(glm::radians(spotLightCutOff)));
		lightShader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(spotLightOuterCutOff)));

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

		lightShader.setFloat("spotLights[1].cutOff", glm::cos(glm::radians(spotLight1CutOff)));
		lightShader.setFloat("spotLights[1].outerCutOff", glm::cos(glm::radians(spotLight1OuterCutOff)));


		texturedShader.use();
		texturedShader.setMat4("VP", VP);
		texturedShader.setVec3("viewPos", camera.Position);

		texturedShader.setFloat("shininess", shininess);

		texturedShader.setBool("dirLight.isActive", isDirLight);
		texturedShader.setVec3("dirLight.direction", direction);
		texturedShader.setVec3("dirLight.colors.ambient", ambient);
		texturedShader.setVec3("dirLight.colors.diffuse", diffuse);
		texturedShader.setVec3("dirLight.colors.specular", specular);

		//POINT LIGHT
		texturedShader.setBool("pointLights[0].isActive", isPointLight);

		texturedShader.setVec3("pointLights[0].position", pointLightPosition);
		texturedShader.setFloat("pointLights[0].att.constant", pointLightConstant);
		texturedShader.setFloat("pointLights[0].att.linear", pointLightLinear);
		texturedShader.setFloat("pointLights[0].att.quadratic", pointLightQuadratic);

		texturedShader.setVec3("pointLights[0].colors.ambient", pointLightAmbient);
		texturedShader.setVec3("pointLights[0].colors.diffuse", pointLightDiffuse);
		texturedShader.setVec3("pointLights[0].colors.specular", pointLightSpecular);

		//SPOT LIGHT 
		texturedShader.setBool("spotLights[0].isActive", isSpotActive);

		texturedShader.setVec3("spotLights[0].position", spotLightPosition);
		texturedShader.setVec3("spotLights[0].direction", spotLightDirection);

		texturedShader.setFloat("spotLights[0].att.constant", spotLightConstant);
		texturedShader.setFloat("spotLights[0].att.linear", spotLightLinear);
		texturedShader.setFloat("spotLights[0].att.quadratic", spotLightQuadratic);

		texturedShader.setVec3("spotLights[0].colors.ambient", spotLightAmbient);
		texturedShader.setVec3("spotLights[0].colors.diffuse", spotLightDiffuse);
		texturedShader.setVec3("spotLights[0].colors.specular", spotLightSpecular);

		texturedShader.setFloat("spotLights[0].cutOff", glm::cos(glm::radians(spotLightCutOff)));
		texturedShader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(spotLightOuterCutOff)));

		//SPOT LIGHT 
		texturedShader.setBool("spotLights[1].isActive", isSpot1Active);

		texturedShader.setVec3("spotLights[1].position", spotLight1Position);
		texturedShader.setVec3("spotLights[1].direction", spotLight1Direction);
		texturedShader.setFloat("spotLights[1].att.constant", spotLight1Constant);
		texturedShader.setFloat("spotLights[1].att.linear", spotLight1Linear);
		texturedShader.setFloat("spotLights[1].att.quadratic", spotLight1Quadratic);

		texturedShader.setVec3("spotLights[1].colors.ambient", spotLight1Ambient);
		texturedShader.setVec3("spotLights[1].colors.diffuse", spotLight1Diffuse);
		texturedShader.setVec3("spotLights[1].colors.specular", spotLight1Specular);

		texturedShader.setFloat("spotLights[1].cutOff", glm::cos(glm::radians(spotLight1CutOff)));
		texturedShader.setFloat("spotLights[1].outerCutOff", glm::cos(glm::radians(spotLight1OuterCutOff)));

		basicShader.use();
		basicShader.setMat4("VP", VP);
		basicShader.setVec3("diffuse", pointLightDiffuse * pointLightAmbient * pointLightSpecular);

		cubemapShader.use();
		cubemapShader.setMat4("VP", skyboxVP);

		

		gunShader.use();
		gunShader.setMat4("VP", VP);
		gunShader.setVec3("viewPos", camera.Position);

		gunShader.setFloat("shininess", shininess);

		gunShader.setBool("dirLight.isActive", isDirLight);
		gunShader.setVec3("dirLight.direction", direction);
		gunShader.setVec3("dirLight.colors.ambient", ambient);
		gunShader.setVec3("dirLight.colors.diffuse", diffuse);
		gunShader.setVec3("dirLight.colors.specular", specular);

		//POINT LIGHT
		gunShader.setBool("pointLights[0].isActive", isPointLight);

		gunShader.setVec3("pointLights[0].position", pointLightPosition);
		gunShader.setFloat("pointLights[0].att.constant", pointLightConstant);
		gunShader.setFloat("pointLights[0].att.linear", pointLightLinear);
		gunShader.setFloat("pointLights[0].att.quadratic", pointLightQuadratic);

		gunShader.setVec3("pointLights[0].colors.ambient", pointLightAmbient);
		gunShader.setVec3("pointLights[0].colors.diffuse", pointLightDiffuse);
		gunShader.setVec3("pointLights[0].colors.specular", pointLightSpecular);

		//SPOT LIGHT 
		gunShader.setBool("spotLights[0].isActive", isSpotActive);

		gunShader.setVec3("spotLights[0].position", spotLightPosition);
		gunShader.setVec3("spotLights[0].direction", spotLightDirection);

		gunShader.setFloat("spotLights[0].att.constant", spotLightConstant);
		gunShader.setFloat("spotLights[0].att.linear", spotLightLinear);
		gunShader.setFloat("spotLights[0].att.quadratic", spotLightQuadratic);

		gunShader.setVec3("spotLights[0].colors.ambient", spotLightAmbient);
		gunShader.setVec3("spotLights[0].colors.diffuse", spotLightDiffuse);
		gunShader.setVec3("spotLights[0].colors.specular", spotLightSpecular);

		gunShader.setFloat("spotLights[0].cutOff", glm::cos(glm::radians(spotLightCutOff)));
		gunShader.setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(spotLightOuterCutOff)));

		
		gunShader.setBool("spotLights[1].isActive", isSpot1Active);

		gunShader.setVec3("spotLights[1].position", spotLight1Position);
		gunShader.setVec3("spotLights[1].direction", spotLight1Direction);
		gunShader.setFloat("spotLights[1].att.constant", spotLight1Constant);
		gunShader.setFloat("spotLights[1].att.linear", spotLight1Linear);
		gunShader.setFloat("spotLights[1].att.quadratic", spotLight1Quadratic);

		gunShader.setVec3("spotLights[1].colors.ambient", spotLight1Ambient);
		gunShader.setVec3("spotLights[1].colors.diffuse", spotLight1Diffuse);
		gunShader.setVec3("spotLights[1].colors.specular", spotLight1Specular);

		gunShader.setFloat("spotLights[1].cutOff", glm::cos(glm::radians(spotLight1CutOff)));
		gunShader.setFloat("spotLights[1].outerCutOff", glm::cos(glm::radians(spotLight1OuterCutOff)));

		//...::SHADER UPDATES END::...


		spotLightGizmo.transform.SetLocalPosition(spotLightPosition);
		spotLightGizmo.transform.SetLocalRotation(spotLightDirection);

		spotLight1Gizmo.transform.SetLocalPosition(spotLight1Position);
		spotLight1Gizmo.transform.SetLocalRotation(spotLight1Direction);

		if (buildingLocalPos != prevBuildingLocalPos)
		{
			prevBuildingLocalPos = buildingLocalPos;
			const auto housePos = house->instanceTransforms[chosenBuilding]->GetLocalPosition();
			house->instanceTransforms[chosenBuilding]->SetLocalPosition(housePos + buildingLocalPos);
		}

		if (housesLocalPos != prevHousesLocalPos)
		{
			prevHousesLocalPos = housesLocalPos;
			neighTransform->SetLocalPosition(housesLocalPos);
		}

		if (neigbourhoodLocalPos != prevNeigbourhoodLocalPos)
		{
			prevNeigbourhoodLocalPos = neigbourhoodLocalPos;
			house->transform.SetLocalPosition(neigbourhoodLocalPos);
		}

		house->transform.Update();


		const auto a = glfwGetTime();
		pointLight.transform.SetLocalRotationX(15 * static_cast<float>(glfwGetTime()));
		pointLight.transform.SetLocalRotationY(15 * static_cast<float>(glfwGetTime()));
		pointLight.transform.SetLocalPosition({ 10 * glm::sin(a), 10 + 10 * glm::cos(a), 0 });


		neighTransform->Update();

		neighbourhood->Draw();
		house->Draw();
		roof->Draw();
		pointLight.Draw();
		spotLightGizmo.Draw();
		spotLight1Gizmo.Draw();

		cubemap.Draw();

		// Draw Weapon
		glClear(GL_DEPTH_BUFFER_BIT);
		gun.Draw();


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

	delete neighbourhood;

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}