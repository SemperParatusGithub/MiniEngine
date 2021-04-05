#pragma once
#include <Engine.h>


class Editor : public Engine::Application
{
public:
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void OnUpdate() override;
	virtual void OnImGui() override;

	static Engine::Application *Create()
	{
		return new Editor();
	}
};