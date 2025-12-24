#pragma once
#include"Lenz/App.h"

extern lenz::App* CreateApp();

#ifndef LZ_DISABLE_ENTRYPOINT

int main(int arg, char**argv)
{
	lenz::App* app = CreateApp();
	app->Run();
	delete(app);
}

#endif

