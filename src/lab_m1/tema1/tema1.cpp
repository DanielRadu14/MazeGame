#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <string>
#include <iostream>
#include <math.h>

using namespace std;
using namespace m1;

bool gameOver = false;
float life = 100.0f;
float timeRemaining = 100.0f;

int const enemiesCount = 5;
int enemies[enemiesCount][4];
float enemiesMovingCoordinates;
bool enemiesGoingForward = true;
float enemyDeformingTime[enemiesCount];

bool shooting = false;
bool shot = false;
bool aiming = false;
float shotTime = 0;
float projectileSpeed;
float projectileLifespan;
bool projectileWallCollision;

int const mazeSize = 10;
int const mazeScaleFactor = 2;
int labirinth[mazeSize][mazeSize];

int directions[4][2] = {{0,-1}, {-1,0}, {1,0}, {0,1}};
float path[mazeSize * 10][3];
int pathSize = 0;

bool rotateRight = false;
bool rotateLeft = false;

glm::vec3 cameraPos;
glm::vec3 cameraCenter;
glm::vec3 cameraPosition;
glm::vec3 cameraForward;
glm::vec3 cameraUp;
glm::vec3 cameraRight;

glm::vec3 projectilePosition;
glm::vec3 shootingDir;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

int Tema1::NearestEnemy()
{
	float minDistance = 99999;
	int nearestEnemy = 0;
	for (int i = 0; i < enemiesCount; i++)
	{
		if (enemies[i][3] == 1)
		{
			float distance = abs(enemies[i][0] + enemiesMovingCoordinates - move.x) + 
				abs(enemies[i][1] + enemiesMovingCoordinates - move.y);
			if (distance < minDistance)
			{
				nearestEnemy = i;
				minDistance = distance;
			}
		}
		
	}
	return nearestEnemy;
}

void Tema1::GenerateMaze()
{
	int x = 0, y = 0;
	int remaining = mazeSize * mazeSize - 1;

	while (remaining > 0)
	{
		while (1)
		{
			int nx, ny;
			int randomDirection = rand() % 4;
			nx = x + directions[randomDirection][0];
			ny = y + directions[randomDirection][1];
			if (nx >= 0 && ny >= 0 && nx < mazeSize && ny < mazeSize)
			{
				if (labirinth[nx][ny] == 0)
				{
					int wall = 0;
					if (ny - y != 0)
					{
						wall = 1;
					}
					path[pathSize][0] = nx;
					path[pathSize][1] = ny;

					if (wall == 1)
					{
						path[pathSize][0] *= mazeScaleFactor;
					}
					else
					{
						path[pathSize][1] *= mazeScaleFactor;
					}
					path[pathSize][wall] += 0.5f;
					path[pathSize][2] = wall;
					pathSize++;
					labirinth[nx][ny] = 1;
					remaining--;
				}
				x = nx;
				y = ny;
				break;
			}
		}
	}

	//generate enemies at random positions near walls
	for (int i = 0; i < enemiesCount; i++)
	{
		int random = rand() % mazeSize;
		enemies[i][0] = path[random][0];
		enemies[i][1] = path[random][1];
		enemies[i][2] = path[random][2];
	}
}

bool Tema1::CheckPlayerCollisions(int i, int type)
{
	float aminX = 0, aminZ = 0, amaxX = 0, amaxZ = 0;
	float bminX = 0, bminZ = 0, bmaxX = 0, bmaxZ = 0;
	if (type == 1 || type == 4)
	{
		if (path[i][2] == 0)
		{
			bminX = path[i][0] - 0.5f;
			bminZ = path[i][1] - 0.005f;

			bmaxX = path[i][0] + 0.5f;
			bmaxZ = path[i][1] + 0.005f;
		}
		else
		{
			bminX = path[i][0] - 0.005f;
			bminZ = path[i][1] - 0.5f;

			bmaxX = path[i][0] + 0.005f;
			bmaxZ = path[i][1] + 0.5f;
		}
	}
	else if (type == 2 || type == 3)
	{
		bminX = enemies[i][0] - 0.2f + enemiesMovingCoordinates;
		bminZ = enemies[i][1] - 0.2f + enemiesMovingCoordinates;

		bmaxX = enemies[i][0] + 0.2f + enemiesMovingCoordinates;
		bmaxZ = enemies[i][1] + 0.2f + enemiesMovingCoordinates;
	}
	
	if (type == 3 || type == 4)
	{
		aminX = projectilePosition.x - 0.125f;
		aminZ = projectilePosition.z - 0.125f;

		amaxX = projectilePosition.x + 0.125f;
		amaxZ = projectilePosition.z + 0.125f;
	}
	else
	{
		aminX = move.x - (0.2f + 0.125f);
		aminZ = move.z - 0.2f;

		amaxX = move.x + 0.2f + 0.125f;
		amaxZ = move.z + 0.2f;
	}

	if ((aminX <= bmaxX && amaxX >= bminX) &&
		(aminZ <= bmaxZ && amaxZ >= bminZ))
	{
		return true;
	}
	return false;
}

void Tema1::ShootingMechanics(float deltaTimeSeconds)
{
	glm::mat4 projectile = glm::mat4(1);
	if (shooting && !shot)
	{
		shootingDir = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z));
		shooting = false;
		shot = true;
		shotTime = projectileLifespan;
	}

	if (!shot)
	{
		projectile = glm::translate(projectile, glm::vec3(move.x, 0.75f, move.z));
	}
	else
	{
		projectilePosition += shootingDir * deltaTimeSeconds * projectileSpeed;
		shotTime -= deltaTimeSeconds;
		projectile = glm::translate(projectile, glm::vec3(projectilePosition.x,
			0.75f,
			projectilePosition.z));
	}

	projectile = glm::scale(projectile, glm::vec3(0.25f, 0.25f, 0.25f));

	for (int i = 0; i < pathSize; i++)
	{
		if (path[i][0] <= mazeSize && path[i][1] <= mazeSize && path[i][0] != 0 && path[i][1] != 0)
		{
			if (CheckPlayerCollisions(i, 4))
			{
				projectileWallCollision = true;
				break;
			}
		}
	}

	if(!projectileWallCollision)
		RenderMesh(meshes["enemy"], shaders["Simple"], projectile);

	if (shotTime <= 0)
	{
		shot = false;
		shotTime = 0;
		projectilePosition = move;
		projectileWallCollision = false;
	}
}

void Tema1::EnemiesMovement(float deltaTimeSeconds)
{
	if (enemiesGoingForward)
	{
		enemiesMovingCoordinates += 0.5f * deltaTimeSeconds;
		if (enemiesMovingCoordinates >= 0.8f)
		{
			enemiesGoingForward = false;
		}
	}
	else
	{
		enemiesMovingCoordinates -= 0.5f * deltaTimeSeconds;
		if (enemiesMovingCoordinates <= 0.2f)
		{
			enemiesGoingForward = true;
		}
	}
}

bool Tema1::CheckGameFinished()
{
	return (move.x >= mazeSize || move.z >= mazeSize);
}

void Tema1::InitializeVariables()
{
	projectileSpeed = 4.0f;
	projectileWallCollision = false;
	projectileLifespan = 1.0f;
	projectilePosition = glm::vec3(0, 0, 0);
	shootingDir = glm::vec3(0);
	enemiesMovingCoordinates = 0;
	playerRotateAngle = 0;
	move = glm::vec3(0.5f, 0, -1);

	for (int i = 0; i < mazeSize; i++)
	{
		for (int j = 0; j < mazeSize; j++)
		{
			labirinth[i][j] = 0;
		}
	}

	for (int i = 0; i < enemiesCount; i++)
	{
		enemies[i][3] = 1;
	}

	for (int i = 0; i < enemiesCount; i++)
	{
		enemyDeformingTime[i] = 0;
	}

	cameraPos = glm::vec3(0.5f, 2.0f, -2.5f);
	cameraCenter = glm::vec3(0.5f, 1.5f, -1);
}

void Tema1::Init()
{
	InitializeVariables();
	GenerateMaze();

    camera = new implemented::Camera();
    camera->Set(cameraPos, cameraCenter, glm::vec3(0, 1, 0));
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

	{
		Mesh* mesh = new Mesh("enemy");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.2)),
			VertexFormat(glm::vec3(5, 0,  0), glm::vec3(1, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
			VertexFormat(glm::vec3(5,  2,  0), glm::vec3(1, 0, 0), glm::vec3(0.7, 0.7, 0.1)),
			VertexFormat(glm::vec3(0,  2,  0), glm::vec3(0, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
		};

		vector<unsigned int> indices =
		{
			0, 1, 2,
			0, 2, 3,    
		};

		CreateMesh("healthbar", vertices, indices);
	}

	{
		Shader *shader = new Shader("HUDBackground");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab6", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab6", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;

		shader = new Shader("HealthBar");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema1", "healthBarShaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema1", "healthBarShaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;

		shader = new Shader("EnemyShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema1", "enemyShaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema1", "enemyShaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;

		shader = new Shader("ColorShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema1", "colorShaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema1", "colorShaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
	if (life <= 0.0f || timeRemaining < 0.0f)
	{
		gameOver = true;
		cout << "GAME OVER!" << endl;
		timeRemaining = 0.0f;
	}

	if (CheckGameFinished() && !gameOver)
	{
		gameOver = true;
		cout << "CONGRATULATIONS!" << endl;
	}

	if (!gameOver)
	{
		timeRemaining -= deltaTimeSeconds;
		EnemiesMovement(deltaTimeSeconds);

		//hud
		{
			glm::mat4 healthBar = glm::mat4(1);
			healthBar = glm::translate(healthBar, glm::vec3(0.45f, 0.9f, 0));
			healthBar = glm::rotate(healthBar, RADIANS(60.0f), glm::vec3(1, 0, 0));
			healthBar = glm::scale(healthBar, glm::vec3(life / 1000, 0.05f, 1));
			RenderSimpleMesh(meshes["healthbar"], shaders["HealthBar"], healthBar, glm::vec3(0));

			healthBar = glm::mat4(1);
			healthBar = glm::translate(healthBar, glm::vec3(0.45f, 0.9f, 0.1f));
			healthBar = glm::rotate(healthBar, RADIANS(60.0f), glm::vec3(1, 0, 0));
			healthBar = glm::scale(healthBar, glm::vec3(0.1f, 0.05f, 1));
			RenderSimpleMesh(meshes["healthbar"], shaders["HUDBackground"], healthBar, glm::vec3(0));

			healthBar = glm::mat4(1);
			healthBar = glm::translate(healthBar, glm::vec3(-0.95f, 0.9f, 0));
			healthBar = glm::rotate(healthBar, RADIANS(60.0f), glm::vec3(1, 0, 0));
			healthBar = glm::scale(healthBar, glm::vec3(timeRemaining / 1000, 0.05f, 1));
			RenderSimpleMesh(meshes["healthbar"], shaders["HealthBar"], healthBar, glm::vec3(0));

			healthBar = glm::mat4(1);
			healthBar = glm::translate(healthBar, glm::vec3(-0.95f, 0.9f, 0.1f));
			healthBar = glm::rotate(healthBar, RADIANS(60.0f), glm::vec3(1, 0, 0));
			healthBar = glm::scale(healthBar, glm::vec3(0.1f, 0.05f, 1));
			RenderSimpleMesh(meshes["healthbar"], shaders["HUDBackground"], healthBar, glm::vec3(0));
		}

		//player
		{
			head = glm::mat4(1);
			head = glm::translate(head, glm::vec3(move.x, move.y, move.z));
			head = glm::rotate(head, playerRotateAngle, glm::vec3(0, 1, 0));
			head = glm::translate(head, glm::vec3(-move.x, -move.y, -move.z));
			head = glm::translate(head, glm::vec3(0, 1.1f, 0) + move);
			head = glm::scale(head, glm::vec3(0.25f, 0.15f, 0.15f));
			RenderSimpleMesh(meshes["box"], shaders["ColorShader"], head, glm::vec3(0.5f, 0.2f, 0.45f));

			body = glm::mat4(1);
			body = glm::translate(body, glm::vec3(0, 0.77f, 0) + move);
			body = glm::scale(body, glm::vec3(0.40f, 0.40f, 0.40f));
			body = glm::rotate(body, playerRotateAngle, glm::vec3(0, 1, 0));
			RenderSimpleMesh(meshes["box"], shaders["ColorShader"], body, glm::vec3(0.2f, 0.7f, 0.9f));

			leftHand = glm::mat4(1);
			leftHand = glm::translate(leftHand, glm::vec3(move.x, move.y, move.z));
			leftHand = glm::rotate(leftHand, playerRotateAngle, glm::vec3(0, 1, 0));
			leftHand = glm::translate(leftHand, glm::vec3(-move.x, -move.y, -move.z));
			leftHand = glm::translate(leftHand, glm::vec3(-0.28f, 0.77f, 0) + move);
			leftHand = glm::scale(leftHand, glm::vec3(0.125f, 0.55f, 0.25f));
			RenderSimpleMesh(meshes["box"], shaders["ColorShader"], leftHand, glm::vec3(0.2f, 0.7f, 0.9f));

			rightHand = glm::mat4(1);
			rightHand = glm::translate(rightHand, glm::vec3(move.x, move.y, move.z));
			rightHand = glm::rotate(rightHand, playerRotateAngle, glm::vec3(0, 1, 0));
			rightHand = glm::translate(rightHand, glm::vec3(-move.x, -move.y, -move.z));
			rightHand = glm::translate(rightHand, glm::vec3(0.28f, 0.77f, 0) + move);
			rightHand = glm::scale(rightHand, glm::vec3(0.125f, 0.55f, 0.25f));
			RenderSimpleMesh(meshes["box"], shaders["ColorShader"], rightHand, glm::vec3(0.2f, 0.7f, 0.9f));

			leftLeg = glm::mat4(1);
			leftLeg = glm::translate(leftLeg, glm::vec3(move.x, move.y, move.z));
			leftLeg = glm::rotate(leftLeg, playerRotateAngle, glm::vec3(0, 1, 0));
			leftLeg = glm::translate(leftLeg, glm::vec3(-move.x, -move.y, -move.z));
			leftLeg = glm::translate(leftLeg, glm::vec3(-0.1f, 0.25f, 0) + move);
			leftLeg = glm::scale(leftLeg, glm::vec3(0.125f, 0.5f, 0.25f));
			RenderSimpleMesh(meshes["box"], shaders["ColorShader"], leftLeg, glm::vec3(0.5f, 0.2f, 0.45f));

			rightLeg = glm::mat4(1);
			rightLeg = glm::translate(rightLeg, glm::vec3(move.x, move.y, move.z));
			rightLeg = glm::rotate(rightLeg, playerRotateAngle, glm::vec3(0, 1, 0));
			rightLeg = glm::translate(rightLeg, glm::vec3(-move.x, -move.y, -move.z));
			rightLeg = glm::translate(rightLeg, glm::vec3(0.1f, 0.25f, 0) + move);
			rightLeg = glm::scale(rightLeg, glm::vec3(0.125f, 0.5f, 0.25f));
			RenderSimpleMesh(meshes["box"], shaders["ColorShader"], rightLeg, glm::vec3(0.5f, 0.2f, 0.45f));
		}

		//enemies
		{
			for (int i = 0; i < enemiesCount; i++)
			{
				if (CheckPlayerCollisions(i, 2))
				{
					life -= 2.5f * deltaTimeSeconds;
				}

				enemy = glm::mat4(1);
				enemy = glm::translate(enemy, glm::vec3(enemies[i][0] + enemiesMovingCoordinates, 0.75f,
					enemies[i][1] + enemiesMovingCoordinates));
				enemy = glm::scale(enemy, glm::vec3(0.3f, 1, 0.3f));

				if (CheckPlayerCollisions(i, 3) && shot)
				{
					enemies[i][3] = 0;
					enemyDeformingTime[i] = 2.0f;
				}

				if(enemies[i][3] == 1)
					RenderSimpleMesh(meshes["sphere"], shaders["ColorShader"], enemy, glm::vec3(0.45f, 0.29f, 0.9f));
				else
				{
					enemyDeformingTime[i] -= deltaTimeSeconds;
					if(enemyDeformingTime[i] > 0)
						RenderSimpleMesh(meshes["sphere"], shaders["EnemyShader"], enemy, glm::vec3(0));
					else
					{
						enemies[i][0] = -90;
						enemies[i][1] = -90;
					}
				}
			}
		}

		//walls
		{
			wallZ = glm::mat4(1);
			wallZ = glm::translate(wallZ, glm::vec3(mazeSize / 2 + 1, 0.5f, 0));
			wallZ = glm::scale(wallZ, glm::vec3(mazeSize, 2, 0.01f));
			RenderMesh(meshes["box"], shaders["Color"], wallZ);

			wallZ = glm::mat4(1);
			wallZ = glm::translate(wallZ, glm::vec3(mazeSize / 2, 0.5f, mazeSize));
			wallZ = glm::scale(wallZ, glm::vec3(mazeSize, 2, 0.01f));
			RenderMesh(meshes["box"], shaders["Color"], wallZ);

			wallX = glm::mat4(1);
			wallX = glm::translate(wallX, glm::vec3(0, 0.5f, mazeSize/2));
			wallX = glm::scale(wallX, glm::vec3(0.01f, 2, mazeSize));
			RenderMesh(meshes["box"], shaders["Color"], wallX);

			wallX = glm::mat4(1);
			wallX = glm::translate(wallX, glm::vec3(mazeSize + 1, 0.5f, mazeSize/2 - 0.5f));
			wallX = glm::scale(wallX, glm::vec3(0.01f, 2, mazeSize - 1));
			RenderMesh(meshes["box"], shaders["Color"], wallX);

			glm::mat4 floor = glm::mat4(1);
			floor = glm::rotate(floor, (float)M_PI/2, glm::vec3(1, 0, 0));
			floor = glm::translate(floor, glm::vec3(mazeSize/2 + 0.5f, mazeSize/2, 0));
			floor = glm::scale(floor, glm::vec3(mazeSize + 1, mazeSize, 0.1f));
			RenderMesh(meshes["box"], shaders["VertexColor"], floor);
		}

		{
			for (int i = 0; i < pathSize; i++)
			{
				if (path[i][0] <= mazeSize && path[i][1] <= mazeSize && path[i][0] != 0 && path[i][1] != 0)
				{
					if (CheckPlayerCollisions(i, 1))
					{
						life -= 25;
						move = glm::vec3(0.5f, 0, -1);
						camera->Set(cameraPos, cameraCenter, glm::vec3(0, 1, 0));
						playerRotateAngle = 0;
					}

					if (path[i][2] == 0)
					{
						wallZ = glm::mat4(1);
						wallZ = glm::translate(wallZ, glm::vec3(path[i][0], 0.5f, path[i][1]));
						wallZ = glm::scale(wallZ, glm::vec3(1, 2, 0.01f));
						RenderMesh(meshes["box"], shaders["Color"], wallZ);
					}
					else if (path[i][2] == 1)
					{
						wallX = glm::mat4(1);
						wallX = glm::translate(wallX, glm::vec3(path[i][0], 0.5f, path[i][1]));
						wallX = glm::scale(wallX, glm::vec3(0.01f, 2, 1));
						RenderMesh(meshes["box"], shaders["Color"], wallX);
					}
				}
			}
		}
		ShootingMechanics(deltaTimeSeconds);
	}
}


void Tema1::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema1::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    mesh->Render();
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	float cameraSpeed = 2.0f;
	float playerSpeed = 2.0f;
	float playerRotateFactor = 4.0f;
    
	if (window->KeyHold(GLFW_KEY_W)) {
		camera->MoveForward(cameraSpeed * deltaTime);
		glm::vec3 moveFactor = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z)) * playerSpeed * deltaTime;
		moveFactor.y = 0;
		move += moveFactor;
	}
	else if (window->KeyHold(GLFW_KEY_Q)) {
		playerRotateAngle += playerRotateFactor * deltaTime;
		camera->MoveLeft(cameraSpeed * deltaTime);
		camera->RotateThirdPerson_OX(deltaTime * 4);
	}
	else if (window->KeyHold(GLFW_KEY_E)) {
		playerRotateAngle -= playerRotateFactor * deltaTime;
		camera->MoveRight(cameraSpeed * deltaTime);
		camera->RotateThirdPerson_OX(-deltaTime * 4);
	}

	if (window->KeyHold(GLFW_KEY_S)) {
		camera->MoveBackward(cameraSpeed * deltaTime);
		glm::vec3 moveFactor = glm::normalize(glm::vec3(camera->forward.x, 0, camera->forward.z)) * playerSpeed * deltaTime;
		moveFactor.y = 0;
		move -= moveFactor;
	}

	if (window->KeyHold(GLFW_KEY_A)) {
		glm::vec3 moveFactor = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z)) * playerSpeed * deltaTime;
		moveFactor.y = 0;
		move -= moveFactor;
	}

	if (window->KeyHold(GLFW_KEY_D)) {
		glm::vec3 moveFactor = glm::normalize(glm::vec3(camera->right.x, 0, camera->right.z)) * playerSpeed * deltaTime;
		moveFactor.y = 0;
		move += moveFactor;
	}
	
}

void Tema1::OnKeyPress(int key, int mods)
{
}

void Tema1::OnKeyRelease(int key, int mods)
{	
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	float sensivityOX = 0.01f;
	float sensivityOY = 0.01f;
	
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
		camera->RotateFirstPerson_OX(-deltaX * sensivityOX);
		camera->RotateFirstPerson_OY(-deltaY * sensivityOY);
    }
	if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) 
	{
		camera->RotateThirdPerson_OX(-deltaX * sensivityOX);
		camera->RotateThirdPerson_OY(-deltaY * sensivityOY);
	}
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_3)
	{
		cameraPosition = camera->position;
		cameraForward = camera->forward;
		cameraUp = camera->up;
		cameraRight = camera->right;

		int nearestEnemy = NearestEnemy();
		camera->Set(glm::vec3(move.x, 1.25f, move.z), 
			glm::vec3(enemies[nearestEnemy][0] + enemiesMovingCoordinates, 
				1.0f, 
				enemies[nearestEnemy][1] + enemiesMovingCoordinates),
			glm::vec3(0, 1, 0));
		
		aiming = true;
	}

	if (button == GLFW_MOUSE_BUTTON_2 && aiming)
	{
		shooting = true;
		projectilePosition = move;
	}
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_3)
	{
		camera->position = cameraPosition;
		camera->forward = cameraForward;
		camera->up = cameraUp;
		camera->right = cameraRight;
		aiming = false;
	}
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}

Mesh* Tema1::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices)
{
	unsigned int VAO = 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

	glBindVertexArray(0);

	CheckOpenGLError();

	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}

void Tema1::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, glm::vec3 color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	glUseProgram(shader->program);

	int location = glGetUniformLocation(shader->program, "time");
	glUniform1f(location, timeRemaining);

	location = glGetUniformLocation(shader->program, "color");
	glUniform3fv(location, 1, glm::value_ptr(color));

	location = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	location = glGetUniformLocation(shader->program, "View");
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	location = glGetUniformLocation(shader->program, "Projection");
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}