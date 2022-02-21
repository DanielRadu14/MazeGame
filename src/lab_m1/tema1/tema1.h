#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema1/lab_camera.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
		void GenerateMaze();
		void InitializeVariables();
		int NearestEnemy();
		void EnemiesMovement(float deltaTimeSeconds);
		void ShootingMechanics(float deltaTimeSeconds);
		//type 1 -> player-wall, type 2 -> player-enemy, type 3 -> projectile-enemy, type 4 -> projectile wall
		bool CheckPlayerCollisions(int i, int type);
		bool CheckGameFinished();
		Mesh *CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);
		void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, glm::vec3 color);

     protected:
        implemented::Camera *camera;
        glm::mat4 projectionMatrix;

        // TODO(student): If you need any other class variables, define them here.
		glm::mat4 head;
		glm::mat4 body;
		glm::mat4 leftHand;
		glm::mat4 rightHand;
		glm::mat4 leftLeg;
		glm::mat4 rightLeg;
		glm::mat4 enemy;
		glm::mat4 wallX;
		glm::mat4 wallZ;

		glm::vec3 move;
		float playerRotateAngle;
    };
}   // namespace m1
