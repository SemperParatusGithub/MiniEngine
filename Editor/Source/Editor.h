#pragma once
#include <Engine.h>


class Editor : public Engine::Application
{
public:
	Editor();
	~Editor();

	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate(float delta) override;
	virtual void OnEvent(Engine::Event &event) override;
	virtual void OnImGui() override;

	static Engine::Application *Create()
	{
		return new Editor();
	}

private:
	Engine::GraphicsPipeline m_Pipeline;
	Engine::EditorCamera m_Camera;
};