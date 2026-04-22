#pragma once
#include "Log.h"

#include "Application.h"
#include "Instrumentor.h"

#ifdef __EMSCRIPTEN__
	#include "emscripten.h"
	#include "emscripten//html5.h"
#endif

extern Ezzoo::Application* Ezzoo::CreatApplication(Ezzoo::AppCommandLineArgs args);

//#ifdef EZZOO_WINDOWS_64
Ezzoo::Application* app = nullptr;
void MainLoop(void)
{

	//EZZOO_BEGINE_SESSION();

	//Ezzoo::Log::Init();
	

	EZZOO_CORE_WARNING("Initialization of Engine");

	EZZOO_CLIENT_INFO("Hello!");

	app->Run();

	#ifndef __EMSCRIPTEN__
		delete app;
	#endif

	//EZZOO_END_SESSION();
}


int main(int argc, char** argv)
{
	app = Ezzoo::CreatApplication({ 0 ,0 });
	#ifndef __EMSCRIPTEN__
		MainLoop();
	#else 
		emscripten_set_canvas_element_size("#canvas", 900, 600);
		EM_ASM({ document.getElementById('canvas').focus(); });
		printf("Canvas focused\n");
		fflush(stdout);
		emscripten_set_main_loop(MainLoop, 0, true); //if i want to change fps we can change 0 to 30 for ex. no need to call setVSync (0 is 60 FBS by default)
		//app->GetWindow().SetVSync(true);
	#endif 
}
//#endif