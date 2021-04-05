#include "Editor.h"


int main(int argc, char **argv)
{
	Engine::Application *editor = Editor::Create();

	editor->Run();

	delete editor;

	return EXIT_SUCCESS;
}