#include "TestShaders.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include <vendor/imgui/imgui.h>

namespace test {

	TestShaders::TestShaders()
        : m_Translation(500, 300, 0),
        m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
	{

        float positions[] = {
			-90.0f, -90.0f, 0.0f, 0.0f,
			 90.0f, -90.0f, 1.0f, 0.0f,
			 90.0f,  90.0f, 1.0f, 1.0f,
			-90.0f,  90.0f, 0.0f, 1.0f,
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();
        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        m_VAO->AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader = std::make_unique<Shader>("res/shaders/Police.shader");
        m_Shader->Bind();
        m_Texture = std::make_unique<Texture>("res/textures/vitalik.png");
        m_Shader->SetUniform1i("u_Texture", 0);
        m_ShaderStartTime = std::chrono::system_clock::now();
	}

	TestShaders::~TestShaders()
	{
        m_VAO->Unbind();
        m_IndexBuffer->Unbind();
        m_Shader->Unbind();
	}

	void TestShaders::OnUpdate(float deltaTime)
	{
	}

	void TestShaders::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;
        m_Texture->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();

            std::chrono::duration<float> elapsedTime = std::chrono::system_clock::now() - m_ShaderStartTime;

            m_Shader->SetUniform1f("u_Time", elapsedTime.count());
            m_Shader->SetUniformMat4f("u_MVP", mvp);

            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }

	}

	void TestShaders::OnImGuiRender()
	{
        ImGui::Begin("Move Vitalik");
        ImGui::SliderFloat2("Translation", &m_Translation.x, 0.0f, 960.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        ImGui::Begin("Choose shader");
		if (ImGui::Button("Basic"))
		{
			m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
			m_Shader->Bind();
            m_ShaderStartTime = std::chrono::system_clock::now();
		} else if (ImGui::Button("Police"))
        {
			m_Shader = std::make_unique<Shader>("res/shaders/Police.shader");
			m_Shader->Bind();
            m_ShaderStartTime = std::chrono::system_clock::now();
        }

        ImGui::End();
	}

}