#include <Engine.h>


class Editor : public Engine::Application
{
public:
	virtual void OnCreate() override
	{
		std::cout << "OnCreate" << std::endl;
	}
	virtual void OnDestroy() override
	{
		std::cout << "OnDestroy" << std::endl;
	}
	virtual void OnUpdate() override
	{
		std::cout << "OnUpdate" << std::endl;
	}
	virtual void OnImGui() override
	{
		std::cout << "OnImGui" << std::endl;
	}

	static Engine::Application *Create()
	{
		return new Editor();
	}
};