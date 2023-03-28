#pragma once
#include"Lenz/App.h"

extern lenz::App* CreateApp();

#ifndef LZ_DESABLE_ENTRYPOINT

int main(int arg, char**argv)
{
	lenz::Log::Init();

	lenz::App* app = CreateApp();
	app->Run();
	delete(app);
}

#endif

