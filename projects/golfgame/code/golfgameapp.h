#pragma once
//------------------------------------------------------------------------------
/**
	Golf game application

	(C) 2024 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"

namespace Game
{
class GolfGameApp : public Core::App
{
public:
	/// constructor
	GolfGameApp();
	/// destructor
	~GolfGameApp();

	/// open app
	bool Open();
	/// run app
	void Run();
	/// exit app
	void Exit();
private:

	/// show some ui things
	void RenderUI();
	/// render some nanovg stuff
	void RenderNanoVG(NVGcontext* vg);

	Display::Window* window;
};
} // namespace Game