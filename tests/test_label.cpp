#include "gui/SDLWindow.hpp"
#include "gui/Font.hpp"
#include "gui/Label.hpp"
#include "gui/GLESContext.hpp"
#include "gui/ConvolveKernel.hpp"
#include "gui/ResourceCache.hpp"
#include "client/Curve.hpp"
#include <iostream>
#include <sstream>
#include "SDL_image.h"
#include "common/math.hpp"

using namespace LM;
using namespace std;

int main(int argc, char *argv[]) {
	SDLWindow* window = SDLWindow::get_instance(500, 100, 24, Window::FLAG_VSYNC);
	GLESContext* ctx = window->get_context();
	ResourceCache* cache = new ResourceCache("data", ctx);
	LinearCurve curve(1, 0);
	ConstantCurve bcurve(1, 1);
	ConvolveKernel kernel(&curve, 7, 2);
	ConvolveKernel bkernel(&bcurve, 4, 1, 1);
	
	Font font("DustHomeMedium.ttf", 20, cache, false, NULL);
	Font cfont("DustHomeMedium.ttf", 20, cache, false, &kernel);
	//Font icfont(string("data/fonts/JuraMedium.ttf"), 30, &ctx, true, &kernel);
	Label l(string("I'm nauseous! I'm nauseous..."), &font);
	Label sl(&cfont);
	l.set_shadow(&sl);
	l.set_x(250);
	l.set_y(50);
	Label p(&font);
	l.set_align(Label::ALIGN_CENTER);
	l.set_skew_align(Label::VALIGN_BOTTOM);
	sl.set_skew_align(Label::VALIGN_TOP);
	p.set_align(Label::ALIGN_RIGHT);
	
	SDL_ShowCursor(SDL_TRUE);

	glClearColor(0.5, 0.5, 0.5, 1.0);

	bool running = true;
	int phase = 0;

	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		ctx->load_identity();

		float f = sin(phase*M_PI/50.0f);
		float c = (1-f)*0.5f;
		//ctx.scale(1, 1/(c+0.8));
		l.set_color(Color(c, c, c, 1.0f));
		l.set_tracking(4.0f*c);
		l.set_skew(f);
		l.draw(ctx);
		//ctx.scale(1, (c+0.8));

		ctx->translate(245, 35);
		ctx->scale(0.5f, 0.5f);
		wstringstream s;
		s << phase;
		p.set_string(s.str());
		p.draw(ctx);
		ctx->scale(2, 2);
		ctx->translate(-245, -35);

		SDL_GL_SwapBuffers();
		SDL_Delay(10);

		phase = (phase+1)%100;
	}
	
	l.set_shadow(NULL);

	return 0;
}
