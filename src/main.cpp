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
#include "LightManager.h"
#include "GunManager.h"

float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;


bool firstMouse = true;
bool wireframe = false;
bool cursor = false;

LightManager lightManager;
GunManager gunManager;

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

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		gunManager.SwitchGun(0);

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		gunManager.SwitchGun(1);


	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		gunManager.Shoot();

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		gunManager.InspectGun();

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		gunManager.Reload();

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

		const float xoffset = xpos - lastX;
		const float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

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
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "4 - Wolf3D", NULL, NULL);

	if (window == nullptr)
		return 1;

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSwapInterval(1); // vsync
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
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();

	ImVec4 clear_color = ImVec4(.22f, .22f, .22f, 1.00f);

	Shader lightCasterShader("res/shaders/basic.vert", "res/shaders/basic.frag");
	Shader lightShader("res/shaders/light.vert", "res/shaders/light.frag");
	Shader texturedShader("res/shaders/textured.vert", "res/shaders/light.frag");
	Shader cubemapShader("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");
	Shader gunShader("res/shaders/gun.vert", "res/shaders/light.frag");
	Shader rShader("res/shaders/gun.vert", "res/shaders/gun.frag");

	//lightManager.AddShader(&lightCasterShader);
	lightManager.AddShader(&lightShader);
	lightManager.AddShader(&texturedShader);
	//lightManager.AddShader(&cubemapShader);
	lightManager.AddShader(&gunShader);

	gunManager.gunShader = &rShader;

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

	//instanced matrices preparation
	int rows = 100, columns = 100;
	int amount = rows * columns;

	std::vector<Transform*> houseTransforms;
	std::vector<Transform*> roofTransforms;

	auto cubeModel = new Model("res/models/cube/cube.obj");
	auto pyramidModel = new Model("res/models/pyramid/pyramid.obj");
	auto plane = new Model("res/models/plane/plane.obj");

	std::vector<Object*> scene;

	Object neighbourhood(plane, &texturedShader);

	scene.emplace_back(&neighbourhood);

	auto neighTransform = &neighbourhood.transform;

	glm::mat4 temp = glm::translate(glm::mat4(1.0f), { -200, 0, -200 });

	for (auto i = 0; i < columns; i++)
	{
		glm::mat4 model(1.0f);
		for (auto j = 0; j < rows; j++)
		{
			auto houseTransform = new Transform();
			auto roofTransform = new Transform();

			temp = glm::translate(temp, glm::vec3(3.0f, 0.0f, 0.0f));
			model = glm::translate(temp, { 0, 1.5f, 0 });

			houseTransform->SetModelMatrix(model);
			houseTransform->SetParent(neighTransform);

			houseTransforms.emplace_back(houseTransform);

			roofTransform->SetLocalPosition(glm::vec3(0.0f, 2.0f, 0.0f));
			roofTransform->SetParent(houseTransforms.back());

			roofTransforms.emplace_back(roofTransform);
		}
		temp = glm::translate(temp, glm::vec3(-1.0f * static_cast<float>(rows) * 3.0f, 0.0f, 3.0f));
	}

	InstancedObject house(cubeModel, &lightShader, houseTransforms);
	InstancedObject roof(pyramidModel, &lightShader, roofTransforms);

	Object spotLightGizmo("res/models/pyramid/pyramid.obj", &lightCasterShader);
	Object spotLight1Gizmo("res/models/pyramid/pyramid.obj", &lightCasterShader);
	Object pointLight("res/models/cube/cube.obj", &lightCasterShader);
	Object gun("res/models/gun/gun.obj", &rShader);
	Object ump("res/models/ump/ump47.obj", &rShader);


	scene.emplace_back(&house);
	scene.emplace_back(&roof);
	scene.emplace_back(&spotLightGizmo);
	scene.emplace_back(&spotLight1Gizmo);
	scene.emplace_back(&pointLight);
	//scene.emplace_back(&gun);
	//scene.emplace_back(&ump);
	
	gun.transform.SetLocalRotation({ 0, 90.0f, -10.0f });
	gun.transform.SetLocalPosition({ 0, -.8, -1.5 });
	//gun.transform.SetLocalPosition({ 0, 0, 0 });
	ump.transform.SetLocalRotation({ 3.0f, 95.0f, 0 });
	ump.transform.SetLocalScale({ .4f, .4f, .4f });

	ump.transform.SetLocalPosition({ 0, -1.0f, -4.5f });


	//
	gunManager.AddGun(&gun);
	gunManager.AddGun(&ump);

	const auto gizmoScale = glm::vec3(0.2f);
	spotLightGizmo.transform.SetLocalScale(gizmoScale);
	spotLight1Gizmo.transform.SetLocalScale(gizmoScale);
	pointLight.transform.SetLocalScale(gizmoScale);

	spotLightGizmo.transform.SetParent(neighTransform);
	spotLightGizmo.transform.SetLocalPosition({ 0, 10, 0 });
	spotLight1Gizmo.transform.SetParent(neighTransform);
	spotLight1Gizmo.transform.SetLocalPosition({ 10, 10, 0 });
	pointLight.transform.SetParent(neighTransform);

	neighbourhood.Update();

	int chosenBuilding = 0;

	glm::vec3 buildingLocalPos(0.0f);
	glm::vec3 prevBuildingLocalPos = buildingLocalPos;
	glm::vec3 housesLocalPos(0.0f); //house.transform->GetLocalPosition();
	glm::vec3 prevHousesLocalPos = housesLocalPos;
	glm::vec3 neigbourhoodLocalPos(0.0f);
	glm::vec3 prevNeigbourhoodLocalPos = neigbourhoodLocalPos;

	float refractiveRatio = 1.0f;
	bool reflective = false;

	// Main loop
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_BACK);

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1280.0f / 720.0f, 0.1f, 100.0f);

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

			ImGui::InputInt("Chosen building", &lightManager.state.instance);
			ImGui::SliderFloat3("Building local pos", glm::value_ptr(lightManager.state.offset), -10.0f, 10.0f);
			ImGui::InputFloat3("Plane local pos", glm::value_ptr(housesLocalPos));

			ImGui::InputFloat3("N loc", glm::value_ptr(neigbourhoodLocalPos));

			ImGui::Text("Material");
			ImGui::SliderFloat("Shininess", &lightManager.state.shininess, 0.0f, 256.0f);
			ImGui::SliderFloat("Refractive ratio", &refractiveRatio, 0.0f, 1.0f, "%.4f");
			ImGui::Checkbox("Reflective", &reflective);

			ImGui::Checkbox("Directional light", &lightManager.state.dirLight.active);
			ImGui::SliderFloat3("Direction", glm::value_ptr(lightManager.state.dirLight.dir), -1.0f, 1.0f);
			ImGui::ColorEdit3("Ambient", glm::value_ptr(lightManager.state.dirLight.color.ambient));
			ImGui::ColorEdit3("Diffuse", glm::value_ptr(lightManager.state.dirLight.color.diffuse));
			ImGui::ColorEdit3("Specular", glm::value_ptr(lightManager.state.dirLight.color.specular));

			ImGui::Text("POINT LIGHT");
			ImGui::Checkbox("Point light", &lightManager.state.pointLight.active);

			ImGui::ColorEdit3("Point light ambient", glm::value_ptr(lightManager.state.pointLight.color.ambient));
			ImGui::ColorEdit3("Point light diffuse", glm::value_ptr(lightManager.state.pointLight.color.diffuse));
			ImGui::ColorEdit3("Point light specular", glm::value_ptr(lightManager.state.pointLight.color.specular));
			ImGui::InputFloat("Point light constant", &lightManager.state.pointLight.constant);
			ImGui::InputFloat("Point light linear", &lightManager.state.pointLight.linear);
			ImGui::InputFloat("Point light quadratic", &lightManager.state.pointLight.quadratic);

			ImGui::Text("SPOT LIGHT");
			ImGui::Checkbox("Spot light", &lightManager.state.spotLights[0].active);
			ImGui::DragFloat3("Spot light position", glm::value_ptr(lightManager.state.spotLights[0].pos), .1f, -10.0f,
				10.0f);
			ImGui::SliderFloat3("Spot light direction", glm::value_ptr(lightManager.state.spotLights[0].dir), -1.0f,
				1.0f);

			ImGui::ColorEdit3("Spot light ambient", glm::value_ptr(lightManager.state.spotLights[0].color.ambient));
			ImGui::ColorEdit3("Spot light diffuse", glm::value_ptr(lightManager.state.spotLights[0].color.diffuse));
			ImGui::ColorEdit3("Spot light specular", glm::value_ptr(lightManager.state.spotLights[0].color.specular));
			ImGui::InputFloat("Spot light constant", &lightManager.state.spotLights[0].constant);
			ImGui::InputFloat("Spot light linear", &lightManager.state.spotLights[0].linear);
			ImGui::InputFloat("Spot light quadratic", &lightManager.state.spotLights[0].quadratic);
			ImGui::InputFloat("Spot light cut off", &lightManager.state.spotLights[0].cutOff);
			ImGui::InputFloat("Spot light outer cut off", &lightManager.state.spotLights[0].outerCutOff);

			ImGui::Text("SPOT LIGHT 1");
			ImGui::Checkbox("Spot light 1", &lightManager.state.spotLights[1].active);
			ImGui::DragFloat3("Spot light 1 position", glm::value_ptr(lightManager.state.spotLights[1].pos), .1f,
				-10.0f, 10.0f);
			ImGui::SliderFloat3("Spot light 1 direction", glm::value_ptr(lightManager.state.spotLights[1].dir), -1.0f,
				1.0f);

			ImGui::ColorEdit3("Spot light 1 ambient", glm::value_ptr(lightManager.state.spotLights[1].color.ambient));
			ImGui::ColorEdit3("Spot light 1 diffuse", glm::value_ptr(lightManager.state.spotLights[1].color.diffuse));
			ImGui::ColorEdit3("Spot light 1 specular", glm::value_ptr(lightManager.state.spotLights[1].color.specular));
			ImGui::InputFloat("Spot light 1 constant", &lightManager.state.spotLights[1].constant);
			ImGui::InputFloat("Spot light 1 linear", &lightManager.state.spotLights[1].linear);
			ImGui::InputFloat("Spot light 1 quadratic", &lightManager.state.spotLights[1].quadratic);
			ImGui::InputFloat("Spot light 1 cut off", &lightManager.state.spotLights[1].cutOff);
			ImGui::InputFloat("Spot light 1 outer cut off", &lightManager.state.spotLights[1].outerCutOff);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
			ImGui::End();
		}

		{
			ImGui::Begin("Ammo");
			ImGui::Text("%d", gunManager.GetAmmo());
			ImGui::End();
		}

		const auto currentView = camera.GetViewMatrix();

		glm::mat4 VP = projection * currentView;
		glm::mat4 skyboxVP = projection * glm::mat4(glm::mat3(currentView)); //remove translation

		lightManager.state.VP = VP;
		lightManager.state.viewPos = camera.Position;
		lightManager.state.pointLight.pos = pointLight.transform.GetLocalPosition();

		cubemapShader.use();
		cubemapShader.setMat4("VP", skyboxVP);

		lightManager.Update();

		lightCasterShader.use();
		lightCasterShader.setMat4("VP", VP);
		//lightCasterShader.setVec3("diffuse", pointLightDiffuse * pointLightAmbient * pointLightSpecular);

		gunShader.use();
		gunShader.setMat4("projection", projection);
		gunShader.setMat4("VP", VP);
		gunShader.setVec3("viewPos", camera.Position);

		rShader.use();
		rShader.setMat4("VP", VP);
		rShader.setMat4("view", currentView);
		rShader.setVec3("cameraPos", camera.Position);
		rShader.setMat4("projection", projection);
		rShader.setBool("isReflective", reflective);
		rShader.setFloat("refractiveRatio", refractiveRatio);


		// TODO: spot light rotation

		//const auto spotLightRotation =
		//	glm::lookAt(lightManager.state.spotLights[0].pos,
		//		 lightManager.state.spotLights[0].dir + lightManager.state.spotLights[0].pos,
		//		glm::vec3(0.0f, 1.0f, 0.0f));

		//spotLightGizmo.transform.SetModelMatrix(glm::inverse(spotLightRotation));

		spotLightGizmo.transform.SetLocalPosition(lightManager.state.spotLights[0].pos);
		spotLightGizmo.transform.SetLocalRotation(-lightManager.state.spotLights[0].dir * 360.0f);

		spotLight1Gizmo.transform.SetLocalPosition(lightManager.state.spotLights[1].pos);
		spotLight1Gizmo.transform.SetLocalRotation(-lightManager.state.spotLights[1].dir * 360.0f);

		if (buildingLocalPos != prevBuildingLocalPos)
		{
			prevBuildingLocalPos = buildingLocalPos;
			const glm::vec3 housePos = house.instanceTransforms[chosenBuilding]->GetLocalPosition();
			house.instanceTransforms[chosenBuilding]->SetLocalPosition(housePos + buildingLocalPos);
		}

		if (housesLocalPos != prevHousesLocalPos)
		{
			prevHousesLocalPos = housesLocalPos;
			neighTransform->SetLocalPosition(housesLocalPos);
		}

		if (neigbourhoodLocalPos != prevNeigbourhoodLocalPos)
		{
			prevNeigbourhoodLocalPos = neigbourhoodLocalPos;
			house.transform.SetLocalPosition(neigbourhoodLocalPos);
		}

		//house.transform.Update();

		const auto time = static_cast<float>(glfwGetTime());
		pointLight.transform.SetLocalRotationX(15 * time);
		pointLight.transform.SetLocalRotationY(15 * time);
		pointLight.transform.SetLocalPosition({ 10 * glm::sin(time), 10 + 10 * glm::cos(time), 0 });

		neighTransform->Update();

		gunManager.Update();

		for (const auto& object : scene)
			object->Draw();

		// Draw Weapon
		gunManager.DrawGun();

		cubemap.Draw();


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
