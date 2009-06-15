#include "client/Transition.hpp"
#include "client/GameWindow.hpp"
#include "client/ClientSDL.hpp"
#include "client/Sprite.hpp"
#include "common/timer.hpp"
#include "common/PathManager.hpp"

extern "C" int main(int argc, char* argv[]) {
	ClientSDL sdl;
	(void)(sdl);
	GameWindow *window = GameWindow::get_instance(300, 300, 24, false);
	PathManager pman(argv[0]);
	Sprite g(pman.data_path("shot.png","sprites"));
	window->register_graphic(&g);
	bool running = true;
	uint64_t start = get_ticks();
	LinearCurve cp(5.0,280.0);
	LinearCurve cs(0.5,2.0);
	LinearCurve cr(0,180);
	Transition tx(&g,&Graphic::set_x,&cp,start,2000);
	Transition ty(&g,&Graphic::set_y,&cp,start,2000);
	Transition tsx(&g,&Graphic::set_scale_x,&cs,start,2000);
	Transition tsy(&g,&Graphic::set_scale_y,&cs,start,2000);
	Transition tr(&g,&Graphic::set_rotation,&cr,start,2000);
	while(running) {
		SDL_Event e;
		while(SDL_PollEvent(&e) != 0) {
			switch(e.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				} break;
			}
		}
		uint64_t current = get_ticks();
		if(tx.update(current)) tx = Transition(&g,&Graphic::set_x,&cp,current,2000);
		if(ty.update(current)) ty = Transition(&g,&Graphic::set_y,&cp,current,2000);
		if(tsx.update(current)) tsx = Transition(&g,&Graphic::set_scale_x,&cs,current,2000);
		if(tsy.update(current)) tsy = Transition(&g,&Graphic::set_scale_y,&cs,current,2000);
		if(tr.update(current)) tr = Transition(&g,&Graphic::set_rotation,&cr,current,2000);
		window->redraw();
	}
	
	GameWindow::destroy_instance();
	return 0;
}