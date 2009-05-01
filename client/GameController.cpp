/*
 * GameController.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#include "GameController.hpp"
#include "GameWindow.hpp"
#include "ClientNetwork.hpp"
#include "GraphicalPlayer.hpp"
#include "Sprite.hpp"
#include "TiledGraphic.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include "common/network.hpp"
#include "common/math.hpp"
#include "common/team.hpp"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

GameController::GameController() {
	init(1024, 768, 24, false);
}

GameController::GameController(int width, int height) {
	init(width, height, 24, true);
}

GameController::~GameController() {
	// TEMPORARY SPRITE CODE
	delete blue_sprite;
	delete blue_front_arm;
	delete blue_back_arm;
	delete red_sprite;
	delete red_front_arm;
	delete red_back_arm;
	delete m_crosshairs;

	for (unsigned int i = 0; i < m_shots.size(); i++) {
		m_window->unregister_graphic(m_shots[i].first);
		m_shots.erase(m_shots.begin() + i);
	}

	for (unsigned int i = 0; i < m_messages.size(); i++) {
		m_text_manager->remove_string(m_messages[i].first);
		m_messages.erase(m_messages.begin() + i);
	}

	// TEMPORARY MAP CODE BY ANDREW
	delete m_map;

	delete m_text_manager;
	delete m_sound_controller;
	delete m_font;
	delete m_path_manager;
	delete m_shot;

	// The GameWindow instance should always be destroyed last, since other stuff may depend on it.
	m_window->destroy_instance();
}

void GameController::init(int width, int height, int depth, bool fullscreen) {
	srand ( time(NULL) );
	
	initialize_key_bindings();
	
	m_game_state = SHOW_MENUS;
	
	m_screen_width = width;
	m_screen_height = height;
	
	m_input_text = "> ";
	m_input_bar = NULL;
	
	m_client_version = "0.0.2";
	m_protocol_number = 1;
	
	m_pixel_depth = depth;
	m_fullscreen = fullscreen;
	m_quit_game = false;
	m_window = GameWindow::get_instance(m_screen_width, m_screen_height, m_pixel_depth, m_fullscreen);

	m_path_manager = new PathManager(""); // TODO: Pass it a path.

	m_time_to_unfreeze = 0;
	m_last_fired = 0;

	m_font = new Font("data/fonts/JuraMedium.ttf", 14);
	m_text_manager = new TextManager(m_font, m_window);
	
	m_menu_font = new Font("data/fonts/JuraDemiBold.ttf", 34);
	
	m_sound_controller = new SoundController();

	// TEMPORARY MAP CODE BY ANDREW
	m_map = new GraphicalMap(m_window);
	//m_map->load_file("data/maps/test.map");
	//m_map_width = m_map->get_width();
	//m_map_height = m_map->get_height();
	m_map_width = 0;
	m_map_height = 0;

	// TEMPORARY SPRITE CODE
	gun_normal = new Sprite(m_path_manager->data_path("gun_noshot.png", "sprites"));
	gun_normal->set_x(3);
	gun_normal->set_y(19);
	gun_normal->set_rotation(-15);
	gun_normal->set_priority(-1);
	
	blue_sprite = new Sprite(m_path_manager->data_path("blue_armless.png","sprites"));
	blue_back_arm = new Sprite(m_path_manager->data_path("blue_backarm.png","sprites"));
	blue_front_arm = new Sprite(m_path_manager->data_path("blue_frontarm.png","sprites"));
	blue_back_arm->set_center_x(27);
	blue_back_arm->set_center_y(29);
	blue_back_arm->set_x(-5);
	blue_back_arm->set_y(-20);
	blue_front_arm->set_center_x(49);
	blue_front_arm->set_center_y(33);
	blue_front_arm->set_x(17);
	blue_front_arm->set_y(-16);
	blue_front_arm->set_priority(-2);
	blue_back_arm->set_priority(1);
	
	blue_player.add_graphic(blue_sprite, "torso");
	blue_player.add_graphic(blue_back_arm, "backarm");
	blue_arm_gun.add_graphic(gun_normal, "gun");
	blue_arm_gun.add_graphic(blue_front_arm, "arm");
	blue_arm_gun.set_center_x(13);
	blue_arm_gun.set_center_y(-18);
	blue_arm_gun.set_x(13);
	blue_arm_gun.set_y(-18);
	blue_player.add_graphic(&blue_arm_gun, "frontarm");
	
	red_sprite = new Sprite(m_path_manager->data_path("red_armless.png","sprites"));
	red_back_arm = new Sprite(m_path_manager->data_path("red_backarm.png","sprites"));
	red_front_arm = new Sprite(m_path_manager->data_path("red_frontarm.png","sprites"));
	red_front_arm->set_center_x(49);
	red_front_arm->set_center_y(33);
	red_front_arm->set_x(17);
	red_front_arm->set_y(-16);
	red_back_arm->set_center_x(27);
	red_back_arm->set_center_y(29);
	red_back_arm->set_x(-5);
	red_back_arm->set_y(-20);
	red_front_arm->set_priority(-2);
	red_back_arm->set_priority(1);
	
	red_player.add_graphic(red_sprite, "torso");
	red_player.add_graphic(red_back_arm, "backarm");
	red_arm_gun.add_graphic(gun_normal, "gun");
	red_arm_gun.add_graphic(red_front_arm, "arm");
	red_arm_gun.set_center_x(13);
	red_arm_gun.set_center_y(-18);
	red_arm_gun.set_x(13);
	red_arm_gun.set_y(-18);
	red_player.add_graphic(&red_arm_gun, "frontarm");
	
	m_crosshairs = new Sprite("data/sprites/crosshairs.png");
	m_crosshairs->set_priority(-1);
	m_window->register_hud_graphic(m_crosshairs);
	
	m_shot = new Sprite("data/sprites/shot.png");
	m_shot->set_invisible(true);
	//m_window->register_graphic(m_shot); // TODO: Remove later when multiple shot graphics are shown.
	
	m_logo = new Sprite("data/sprites/legesmotuslogo.png");
	m_logo->set_x(m_screen_width/2);
	m_logo->set_y(100);
	m_logo->set_priority(-1);
	m_window->register_hud_graphic(m_logo);
	
	m_main_menu_items = map<string, Graphic*>();
	
	m_text_manager->set_active_font(m_menu_font);
	m_main_menu_items["Resume Game"] = m_text_manager->place_string("Resume Game", 50, 200, TextManager::LEFT, TextManager::LAYER_HUD);
	m_main_menu_items["Options"] = m_text_manager->place_string("Options", 50, 250, TextManager::LEFT, TextManager::LAYER_HUD);
	m_main_menu_items["Quit"] = m_text_manager->place_string("Quit", 50, 300, TextManager::LEFT, TextManager::LAYER_HUD);
	m_text_manager->set_active_font(m_font);
	m_text_manager->set_shadow_color(0.0, 0.0, 0.0);
	m_text_manager->set_shadow_alpha(0.7);
	m_text_manager->set_shadow_offset(1.0, 1.0);
	m_text_manager->set_shadow(true);
}

void GameController::run(int lockfps) {
	cerr << "SDL window is: " << m_window->get_width() << " pixels wide and " 
		<< m_window->get_height() << " pixels tall." << endl;
	
	unsigned long startframe = SDL_GetTicks();
	unsigned long lastmoveframe = startframe;
	
	/* 1 second / FPS = milliseconds per frame */
	double delay = 1000.0 / lockfps;
	
	display_message("Welcome to Leges Motus!");
	
	while(m_quit_game == false) {
		process_input();
		
		m_network.receive_packets(*this);
		
		if (m_quit_game == true) {
			break;
		}
		
		if (!m_players.empty() && m_time_to_unfreeze < SDL_GetTicks() && m_time_to_unfreeze != 0) {
			m_sound_controller->play_sound("unfreeze");
			m_players[m_player_id].set_is_frozen(false);
			m_time_to_unfreeze = 0;
		}
		
		// Update graphics if frame rate is correct
		unsigned long currframe = SDL_GetTicks();
		if((currframe - startframe) >= delay) {
			for (unsigned int i = 0; i < m_messages.size(); i++) {
				if (m_messages[i].second < currframe) {
					m_text_manager->remove_string(m_messages[i].first);
					m_messages.erase(m_messages.begin() + i);
				}
			}
			
			for (unsigned int i = 0; i < m_messages.size(); i++) {
				int y = 20 + (m_font->ascent() + m_font->descent() + 5) * i;
				m_text_manager->reposition_string(m_messages[i].first, 20, y, TextManager::LEFT);
			}
			
			for (unsigned int i = 0; i < m_shots.size(); i++) {
				m_shots[i].first->set_scale_x((double)(SHOT_DISPLAY_TIME-(m_shots[i].second - currframe))/SHOT_DISPLAY_TIME);
				m_shots[i].first->set_scale_y((double)(SHOT_DISPLAY_TIME-(m_shots[i].second - currframe))/SHOT_DISPLAY_TIME);
				if (m_shots[i].second < currframe) {
					m_window->unregister_graphic(m_shots[i].first);
					m_shots.erase(m_shots.begin() + i);
				}
			}
			
			move_objects((SDL_GetTicks() - lastmoveframe) / delay); // scale all position changes to keep game speed constant. 
			
			lastmoveframe = SDL_GetTicks();
			
			// TODO: Uncomment if framerate is needed.
			// the framerate:
			//int framerate = (1000/(currframe - startframe));
			
			if (!m_players.empty()) {
				send_my_player_update();
				
				m_offset_x = m_players[m_player_id].get_x() - (m_screen_width/2.0);
				m_offset_y = m_players[m_player_id].get_y() - (m_screen_height/2.0);
				m_window->set_offset_x(m_offset_x);
				m_window->set_offset_y(m_offset_y);
			
				m_crosshairs->set_x(m_mouse_x);
				m_crosshairs->set_y(m_mouse_y);
				
				if (m_game_state == SHOW_MENUS) {
					m_map->set_visible(false);
					set_players_visible(false);
					
					m_logo->set_invisible(false);
					map<string, Graphic*>::iterator it;
					for ( it=m_main_menu_items.begin() ; it != m_main_menu_items.end(); it++ ) {
						Graphic* thisitem = (*it).second;
						thisitem->set_invisible(false);
					}
				} else {
					m_map->set_visible(true);
					set_players_visible(true);
					
					m_logo->set_invisible(true);
					map<string, Graphic*>::iterator it;
					for ( it=m_main_menu_items.begin() ; it != m_main_menu_items.end(); it++ ) {
						Graphic* thisitem = (*it).second;
						thisitem->set_invisible(true);
					}
				}
			}
			
			m_window->redraw();
			startframe = SDL_GetTicks();
		}
	}
	
	disconnect();
}

void GameController::set_screen_dimensions(int width, int height) {
	m_screen_width = width;
	m_screen_height = height;
}

void GameController::process_input() {
	SDL_Event event;
	double x_dist;
	double y_dist;
	double angle;
	while(SDL_PollEvent(&event) != 0) {
		switch(event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == m_key_bindings.quit) {
					if (m_input_bar != NULL) {
						SDL_EnableUNICODE(0);
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = NULL;
						m_input_text = "> ";
					} else {
						cerr << "Quit key pressed - quitting." << endl;
						m_quit_game = true;
					}
				}
				
				if (m_input_bar != NULL) {
					m_text_manager->set_active_color(1.0, 1.0, 1.0);
					if (event.key.keysym.sym == m_key_bindings.send_chat) {
						send_message(m_input_text.substr(2));
					
						SDL_EnableUNICODE(0);
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = NULL;
						m_input_text = "> ";
					} else if (event.key.keysym.sym == SDLK_BACKSPACE) {
						m_input_text.erase(m_input_text.length() - 1);
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
					} else {
						if ( (event.key.keysym.unicode & 0xFF80) == 0 && event.key.keysym.unicode != 0) {
							m_input_text.push_back(event.key.keysym.unicode & 0x7F);
						} else {
							// INTERNATIONAL CHARACTER... DO SOMETHING.
						}
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
					}
				} else {
					//Check which key using: event.key.keysym.sym == SDLK_<SOMETHING>
					if (event.key.keysym.sym == m_key_bindings.jump) {
						attempt_jump();
					} else if (event.key.keysym.sym == m_key_bindings.show_overlay) {
						// TODO: Show the overlay.
					} else if (event.key.keysym.sym == m_key_bindings.open_chat) {
						SDL_EnableUNICODE(1);
						m_text_manager->set_active_color(1.0, 1.0, 1.0);
						if (m_input_bar == NULL) {
							m_input_bar = m_text_manager->place_string("> ", 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
						}
					} else if (event.key.keysym.sym == m_key_bindings.show_menu) {
						if (m_game_state == SHOW_MENUS) {
							m_game_state = GAME_IN_PROGRESS;
						} else {
							m_game_state = SHOW_MENUS;
						}
					}
				}
				break;

			case SDL_KEYUP:
				break;
				
			case SDL_MOUSEMOTION:
				// Send motion to the GameWindow, telling it where the mouse is.
				// Use: event.motion.xrel, event.motion.yrel (changes in position), event.motion.x, event.motion.y
				m_mouse_x = event.motion.x;
				m_mouse_y = event.motion.y;
				m_crosshairs->set_x(m_mouse_x);
				m_crosshairs->set_y(m_mouse_y);
				if (m_players.empty() || m_players[m_player_id].is_frozen()) {
					break;
				}
				x_dist = (m_crosshairs->get_x() + m_offset_x) - m_players[m_player_id].get_x();
				y_dist = (m_crosshairs->get_y() + m_offset_y) - m_players[m_player_id].get_y();
				angle = atan2(y_dist, x_dist) * RADIANS_TO_DEGREES;
				if (angle < 90 && angle > -90) {
					blue_player.set_scale_x(-1);
					red_player.set_scale_x(-1);
					send_animation_packet("all", "scale_x", -1);
					angle *= -1;
					angle += 55;
				} else {
					blue_player.set_scale_x(1);
					red_player.set_scale_x(1);
					send_animation_packet("all", "scale_x", 1);
					angle -= 120;
				}
				m_players[m_player_id].get_sprite()->get_graphic("frontarm")->set_rotation(angle);
				send_animation_packet("frontarm", "rotation", angle);
				//red_arm_gun.set_rotation(angle);
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				// Firing code, use event.button.button, event.button.x, event.button.y
				process_mouse_click(event);
				break;
				
			case SDL_MOUSEBUTTONUP:
				break;
				
			case SDL_QUIT:
				m_quit_game = true;
				break;
				
			default:
				break;
		}
	}
	
	parse_key_input();
}

void GameController::initialize_key_bindings() {
	// -1 = unused
	m_key_bindings.quit = -1;
	m_key_bindings.jump = SDLK_SPACE;
	m_key_bindings.show_overlay = -1;
	m_key_bindings.show_menu = SDLK_ESCAPE;
	m_key_bindings.open_chat = SDLK_t;
	m_key_bindings.send_chat = SDLK_RETURN;
}

void GameController::parse_key_input() {
	// For keys that can be held down:
   	m_keys = SDL_GetKeyState(NULL);
}

void GameController::process_mouse_click(SDL_Event event) {
	if (m_game_state == SHOW_MENUS) {
		map<string, Graphic*>::iterator it;
		for ( it=m_main_menu_items.begin() ; it != m_main_menu_items.end(); it++ ) {
			Graphic* thisitem = (*it).second;
			int x = thisitem->get_x();
			int y = thisitem->get_y();
			if (event.button.x >= x && event.button.x <= x + thisitem->get_image_width()
			    && event.button.y >= y && event.button.y <= y + thisitem->get_image_height()) {
				if ((*it).first == "Quit") {
					m_quit_game = true;
					break;
				} else if ((*it).first == "Options") {
					// TODO: Options menu.
				} else if ((*it).first == "Resume Game") {
					m_game_state = GAME_IN_PROGRESS;
				}
			}
		}
	} else if (m_game_state == GAME_IN_PROGRESS) {
		if (event.button.button == 1) {
			if (m_last_fired != 0 && m_last_fired > SDL_GetTicks() - FIRING_DELAY) {
				return;
			}
			if (m_players.empty() || m_players[m_player_id].is_frozen()) {
				return;
			}
			double x_dist = (event.button.x + m_offset_x) - m_players[m_player_id].get_x();
			double y_dist = (event.button.y + m_offset_y) - m_players[m_player_id].get_y();
			double direction = atan2(y_dist, x_dist) * RADIANS_TO_DEGREES;
			if (m_players[m_player_id].get_x_vel() != 0 || m_players[m_player_id].get_y_vel() != 0) {
				m_players[m_player_id].set_velocity(m_players[m_player_id].get_x_vel() - 1.5 * cos((direction) * DEGREES_TO_RADIANS), m_players[m_player_id].get_y_vel() - 1.5 * sin((direction) * DEGREES_TO_RADIANS));
			}
			m_last_fired = SDL_GetTicks();
			player_fired(m_player_id, m_players[m_player_id].get_x(), m_players[m_player_id].get_y(), direction);
			m_sound_controller->play_sound("fire");
		}
	}
}

void GameController::move_objects(float timescale) {
	if (m_players.empty()) {
		return;
	}
	
	double player_x_vel = m_players[m_player_id].get_x_vel() * timescale;
	double player_y_vel = m_players[m_player_id].get_y_vel() * timescale;
	
	double new_x = m_players[m_player_id].get_x() + player_x_vel;
	double new_y = m_players[m_player_id].get_y() + player_y_vel;
	double half_width = m_players[m_player_id].get_radius();
	double half_height = m_players[m_player_id].get_radius();
	
	if (new_x - half_width < 0) {
		new_x = half_width;
		m_players[m_player_id].set_velocity(0, 0);
	} else if (new_x + half_width > m_map_width) {
		new_x = m_map_width - half_width;
		m_players[m_player_id].set_velocity(0, 0);
	}
	
	if (new_y - half_height < 0) {
		new_y = half_height;
		m_players[m_player_id].set_velocity(0, 0);
	} else if (new_y + half_height > m_map_height) {
		new_y = m_map_height - half_height;
		m_players[m_player_id].set_velocity(0, 0);
	}
	
	bool holdinggate = false;
	
	const list<MapObject>& map_objects(m_map->get_objects());
	list<MapObject>::const_iterator thisobj;
	int radius = m_players[m_player_id].get_radius();
	Point currpos = Point(new_x, new_y);
	Point oldpos = Point(m_players[m_player_id].get_x(), m_players[m_player_id].get_y());
	//cerr << "Start: " << SDL_GetTicks() << endl;
	for (thisobj = map_objects.begin(); thisobj != map_objects.end(); thisobj++) {
		if (thisobj->get_sprite() == NULL) {
			continue;
		}
		const Polygon& poly(thisobj->get_bounding_polygon());
		double newdist = poly.intersects_circle(currpos, radius);
		double olddist = poly.intersects_circle(oldpos, radius);
		
		// REPEL FROZEN PLAYERS AWAY FROM GATES.
		if (thisobj->get_type() == Map::GATE && m_players[m_player_id].is_frozen()) {
			double newdist_repulsion = poly.dist_from_circle(currpos, radius);
			if (newdist_repulsion < 400) {
				double gate_x = thisobj->get_upper_left().x + thisobj->get_sprite()->get_image_width()/2;
				double gate_y = thisobj->get_upper_left().y + thisobj->get_sprite()->get_image_height()/2;
				double angle = atan2(gate_y - new_y, gate_x - new_x);
				m_players[m_player_id].set_velocity(m_players[m_player_id].get_x_vel() - .02 * cos(angle), m_players[m_player_id].get_y_vel() - .02 * sin(angle));
			}
		}
		
		if (newdist != -1) {
			if (newdist < olddist) {
				//cerr << "New dist: " << newdist << " Old dist: " << olddist << endl;
				//cerr << "Hitting object" << endl;
				if (thisobj->get_type() == Map::OBSTACLE) {
					m_players[m_player_id].set_velocity(0, 0);
					new_x = m_players[m_player_id].get_x();
					new_y = m_players[m_player_id].get_y();
				}
			}
			if (thisobj->get_type() == Map::GATE && thisobj->get_team() != m_players[m_player_id].get_team() && !m_players[m_player_id].is_frozen()) {
				if (!m_holding_gate) {
					send_gate_hold(true);
				}
				m_holding_gate = true;
				holdinggate = true;
			}
		}
	}
	//cerr << "End: " << SDL_GetTicks() << endl;
	
	if (!holdinggate) {
		if (m_holding_gate) {
			send_gate_hold(false);
		}
		m_holding_gate = false;
	}
	
	//m_text_manager->reposition_string(m_players[m_player_id].get_name_sprite(), new_x, new_y, TextManager::CENTER);
	m_players[m_player_id].set_x(new_x);
	m_players[m_player_id].set_y(new_y);
	
	map<int, GraphicalPlayer>::iterator it;
	for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
		GraphicalPlayer currplayer = (*it).second;
		if (currplayer.is_invisible()) {
			currplayer.get_name_sprite()->set_invisible(true);
		} else {
			currplayer.get_name_sprite()->set_invisible(false);
			m_text_manager->reposition_string(currplayer.get_name_sprite(), currplayer.get_x(), currplayer.get_y() - (currplayer.get_radius()+30), TextManager::CENTER);
		}
	}
}

void GameController::attempt_jump() {
	if (m_players.empty()) {
		return;
	}
	
	if (m_players[m_player_id].is_frozen()) {
		return;
	}
	
	GraphicalPlayer* player = &m_players[m_player_id];
	
	double half_width = m_players[m_player_id].get_radius();
	double half_height = m_players[m_player_id].get_radius();
	
	double x_dist = (m_crosshairs->get_x() + m_offset_x) - player->get_x();
	double y_dist = (m_crosshairs->get_y() + m_offset_y) - player->get_y();
	double x_vel = 6 * cos(atan2(y_dist, x_dist));
	double y_vel = 6 * sin(atan2(y_dist, x_dist));
	
	if (player->get_x() - (half_width) <= 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	} else if (player->get_x() + (half_width) >= m_map_width - 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	}
	
	if (player->get_y() - (half_height) <= 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	} else if (player->get_y() + (half_height) >= m_map_height - 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
	}

	list<MapObject>::const_iterator thisobj;
	const list<MapObject>& map_objects(m_map->get_objects());
	Point currpos = Point(player->get_x(), player->get_y());
	for (thisobj = map_objects.begin(); thisobj != map_objects.end(); thisobj++) {
		if (thisobj->get_sprite() == NULL) {
			continue;
		}
		if (thisobj->get_type() != Map::OBSTACLE) {
			// Only obstacles can be jumped from
			continue;
		}
		const Polygon& poly(thisobj->get_bounding_polygon());
		double newdist = poly.intersects_circle(currpos, player->get_radius()+5);
		if (newdist != -1) {
			player->set_x_vel(x_vel);
			player->set_y_vel(y_vel);
		}
	}
}

void GameController::player_fired(unsigned int player_id, double start_x, double start_y, double direction) {
	const list<MapObject>& map_objects(m_map->get_objects());
	list<MapObject>::const_iterator thisobj;
	Point startpos = Point(start_x, start_y);
	
	double shortestdist = numeric_limits<double>::max();
	Point wallhitpoint = Point(0, 0);
	double end_x = -1;
	double end_y = -1;
	
	for (thisobj = map_objects.begin(); thisobj != map_objects.end(); thisobj++) {
		if (thisobj->get_sprite() == NULL) {
			continue;
		}
		const Polygon& poly(thisobj->get_bounding_polygon());
		double dist_to_obstacle = dist_between_points(start_x, start_y, thisobj->get_sprite()->get_x() + thisobj->get_sprite()->get_image_width()/2, thisobj->get_sprite()->get_y() + thisobj->get_sprite()->get_image_height()/2) + 100;
		Point endpos = Point(start_x + dist_to_obstacle * cos(direction * DEGREES_TO_RADIANS), start_y + dist_to_obstacle * sin(direction * DEGREES_TO_RADIANS));
		
		Point newpoint = poly.intersects_line(startpos, endpos);
		
		if (newpoint.x == -1 && newpoint.y == -1) {
			continue;
		}
		
		double newdist = dist_between_points(start_x, start_y, newpoint.x, newpoint.y);
		
		if (newdist != -1 && newdist < shortestdist) {
			shortestdist = newdist;
			wallhitpoint = newpoint;
			end_x = newpoint.x;
			end_y = newpoint.y;
		}
	}
	
	if (player_id == m_player_id) {
		double player_hit = -1;
		
		map<int, GraphicalPlayer>::iterator it;
		for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
			GraphicalPlayer currplayer = (*it).second;
			if (currplayer.get_id() == player_id || currplayer.is_frozen()) {
				continue;
			}
			double playerdist = dist_between_points(start_x, start_y, currplayer.get_x(), currplayer.get_y());
			
			if (playerdist > shortestdist) {
				continue;
			}
			
			int end_x = start_x + playerdist * cos(direction * DEGREES_TO_RADIANS);
			int end_y = start_y + playerdist * sin(direction * DEGREES_TO_RADIANS);
			vector<double> closestpoint = closest_point_on_line(start_x, start_y, end_x, end_y, currplayer.get_x(), currplayer.get_y());
			
			if (closestpoint.size() == 0) {
				continue;
			}
			
			double dist = dist_between_points(currplayer.get_x(), currplayer.get_y(), closestpoint.at(0), closestpoint.at(1));
			
			// If the closest point was behind the beginning of the shot, it's not a hit.
			if (closestpoint.at(2) < 0) {
				continue;
			}
			
			// If the shot hit the player:
			if (dist < currplayer.get_radius()) {
				shortestdist = playerdist;
				wallhitpoint = Point(0, 0);
				player_hit = currplayer.get_id();
				end_x = closestpoint.at(0);
				end_y = closestpoint.at(1);
			}
		}
		
		// TODO: HANDLE COLLISIONS WITH MAP EDGES?
		
		PacketWriter gun_fired(GUN_FIRED_PACKET);
		gun_fired << player_id;
		gun_fired << start_x;
		gun_fired << start_y;
		gun_fired << direction;
		if (end_x != -1 || end_y != -1) {
			gun_fired << end_x;
			gun_fired << end_y;
			Graphic* this_shot = new Sprite(*m_shot);
			this_shot->set_x(end_x);
			this_shot->set_y(end_y);
			this_shot->set_scale_x(.1);
			this_shot->set_scale_y(.1);
			this_shot->set_invisible(false);
			pair<Graphic*, unsigned int> new_shot(this_shot, SDL_GetTicks() + SHOT_DISPLAY_TIME);
			m_shots.push_back(new_shot);
			m_window->register_graphic(this_shot);
		}
		
		m_network.send_packet(gun_fired);
		
		if (player_hit != -1) {
			send_player_shot(player_id, player_hit);
		}
	}
}

void GameController::set_players_visible(bool visible) {
	if (m_players.empty()) {
		return;
	}

	map<int, GraphicalPlayer>::iterator it;
	for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
		GraphicalPlayer currplayer = (*it).second;
		if (currplayer.get_sprite() == NULL) {
			continue;
		}
		if (visible) {
			currplayer.get_sprite()->set_invisible(currplayer.is_invisible());
			currplayer.get_name_sprite()->set_invisible(currplayer.is_invisible());
		} else {
			currplayer.get_sprite()->set_invisible(true);
			currplayer.get_name_sprite()->set_invisible(true);
		}
	}
}

void GameController::send_player_shot(unsigned int shooter_id, unsigned int hit_player_id) {
	PacketWriter player_shot(PLAYER_SHOT_PACKET);
	player_shot << shooter_id;
	player_shot << hit_player_id;
	
	m_network.send_packet(player_shot);
}

void GameController::connect_to_server(const char* host, unsigned int port, string name, char team) {
	if (!m_network.connect(host, port)) {
		cerr << "Error: Could not connect to server at " << host << ":" << port << endl;
	}
	
	PacketWriter join_request(JOIN_PACKET);
	join_request << m_protocol_number;
	m_name = name;
	join_request << name;
	if (is_valid_team(team)) {
		join_request << team;
	}
	
	m_network.send_packet(join_request);
}

void GameController::disconnect() {
	PacketWriter leave_request(LEAVE_PACKET);
	leave_request << m_player_id;
	
	m_network.send_packet(leave_request);
	
	m_network.disconnect();
}

void GameController::welcome(PacketReader& reader) {
	string serverversion;
	int playerid;
	char team;
	
	reader >> serverversion >> playerid >> team;
	
	m_player_id = playerid;

	cerr << "Received welcome packet. Version: " << serverversion << ", Player ID: " << playerid << ", Team: " << team << endl;
	
	m_players.clear();
	
	if (team == 'A') {
		m_players.insert(pair<int, GraphicalPlayer>(m_player_id,GraphicalPlayer(m_name.c_str(), m_player_id, team, &blue_player, blue_sprite->get_width()/2, blue_sprite->get_height()/2)));
		m_text_manager->set_active_color(0.2, 0.2, 1.0);
		m_window->register_graphic(&blue_player);
	} else {
		m_players.insert(pair<int, GraphicalPlayer>(m_player_id,GraphicalPlayer(m_name.c_str(), m_player_id, team, &red_player, red_sprite->get_width()/2, red_sprite->get_height()/2)));
		m_text_manager->set_active_color(1.0, 0.2, 0.2);
		m_window->register_graphic(&red_player);
	}
	
	m_players[m_player_id].set_radius(30);
	m_players[m_player_id].set_name_sprite(m_text_manager->place_string(m_players[m_player_id].get_name(), m_screen_width/2, (m_screen_height/2)-(m_players[m_player_id].get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
	
	// REMOVE THESE WHEN THE SERVER SENDS GAME START, ETC.
	//m_players[m_player_id].set_is_invisible(false);
	//m_players[m_player_id].set_is_frozen(false);
	
	send_my_player_update();
}

void GameController::announce(PacketReader& reader) {
	unsigned int playerid;
	string playername;
	char team;
	
	reader >> playerid >> playername >> team;
	if (playerid == m_player_id) {
		return;
	}
	
	if (team == 'A') {
		m_players.insert(pair<int, GraphicalPlayer>(playerid,GraphicalPlayer((const char*)playername.c_str(), playerid, team, new GraphicGroup(blue_player))));
		m_text_manager->set_active_color(0.2, 0.2, 1.0);
	} else {
		m_players.insert(pair<int, GraphicalPlayer>(playerid,GraphicalPlayer((const char*)playername.c_str(), playerid, team, new GraphicGroup(red_player))));
		m_text_manager->set_active_color(1.0, 0.2, 0.2);
	}
	// TEMPORARY SPRITE CODE
	m_window->register_graphic(m_players[playerid].get_sprite());
	m_players[playerid].set_name_sprite(m_text_manager->place_string(m_players[playerid].get_name(), m_players[playerid].get_x(), m_players[playerid].get_y()-(m_players[playerid].get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
	m_players[playerid].set_radius(50);
}

void GameController::player_update(PacketReader& reader) {
	if (m_players.empty()) {
		return;
	}

	unsigned int player_id;
	long x;
	long y;
	long velocity_x;
	long velocity_y;
	string flags;
	reader >> player_id >> x >> y >> velocity_x >> velocity_y >> flags;
	
	GraphicalPlayer* currplayer = get_player_by_id(player_id);
	if (currplayer == NULL) {
		cerr << "Error: Received update packet for non-existent player " << player_id << endl;
		return;
	}
	
	currplayer->set_position(x, y);
	currplayer->set_velocity(velocity_x, velocity_y);
	
	
	if (flags.find_first_of('I') == string::npos) {
		currplayer->set_is_invisible(false);
		m_text_manager->reposition_string(m_players[player_id].get_name_sprite(), x, y - (m_players[player_id].get_radius()+30), TextManager::CENTER);
		m_players[player_id].get_name_sprite()->set_invisible(false);
	} else {
		currplayer->set_is_invisible(true);
		m_players[player_id].get_name_sprite()->set_invisible(true);
	}
	
	if (flags.find_first_of('F') == string::npos) {
		currplayer->set_is_frozen(false);
	} else {
		currplayer->set_is_frozen(true);
	}
	
}

void GameController::send_my_player_update() {
	PacketWriter player_update(PLAYER_UPDATE_PACKET);
	if (m_players.empty()) {
		return;
	}
	
	GraphicalPlayer* my_player = &m_players[m_player_id];
	string flags;
	
	if (my_player->is_invisible()) {
		flags.push_back('I');
	}
	if (my_player->is_frozen()) {
		flags.push_back('F');
	}
	
	player_update << m_player_id << my_player->get_x() << my_player->get_y() << my_player->get_x_vel() 
		<< my_player->get_y_vel() << flags;
		
	m_network.send_packet(player_update);
}

void GameController::send_message(string message) {
	PacketWriter message_writer(MESSAGE_PACKET);
	// TODO: Add recipient before message.
	message_writer << m_player_id << "" << message;
	m_network.send_packet(message_writer);
}

void GameController::leave(PacketReader& reader) {
	unsigned int playerid;
	reader >> playerid;
	
	m_text_manager->remove_string(m_players[playerid].get_name_sprite());
	m_window->unregister_graphic(m_players[playerid].get_sprite());
	delete m_players[playerid].get_sprite();
	m_players.erase(playerid);
}

void GameController::gun_fired(PacketReader& reader) {
	unsigned int playerid;
	double start_x;
	double start_y;
	double rotation;
	double end_x;
	double end_y;
	reader >> playerid >> start_x >> start_y >> rotation >> end_x >> end_y;
	
	if (playerid == m_player_id) {
		return;
	}
	
	Graphic* this_shot = new Sprite(*m_shot);
	this_shot->set_x(end_x);
	this_shot->set_y(end_y);
	this_shot->set_invisible(false);
	this_shot->set_scale_x(.1);
	this_shot->set_scale_y(.1);
	pair<Graphic*, unsigned int> new_shot(this_shot, SDL_GetTicks() + SHOT_DISPLAY_TIME);
	m_shots.push_back(new_shot);
	m_window->register_graphic(this_shot);
	m_sound_controller->play_sound("fire");
	player_fired(playerid, start_x, start_y, rotation);
}

void GameController::player_shot(PacketReader& reader) {
	unsigned int shooter_id;
	unsigned int shot_id;
	unsigned long time_to_unfreeze;
	
	reader >> shooter_id >> shot_id >> time_to_unfreeze;
	
	if (shot_id == m_player_id) {
		m_sound_controller->play_sound("freeze");
		cerr << "I was hit! Time to unfreeze: " << time_to_unfreeze << endl;
		m_players[m_player_id].set_is_frozen(true);
		m_time_to_unfreeze = SDL_GetTicks() + time_to_unfreeze;
	}
}

void GameController::message(PacketReader& reader) {
	unsigned int sender_id;
	string recipient;
	string message_text;
	
	reader >> sender_id >> recipient >> message_text;
	
	string message = m_players[sender_id].get_name();
	message.append(": ");
	message.append(message_text);
	
	char team = m_players[sender_id].get_team();
	if (team == 'A') {
		display_message(message, 0.4, 0.4, 1);
	} else {
		display_message(message, 1, 0.4, 0.4);
	}
}

void GameController::gate_lowering(PacketReader& reader) {
	uint32_t	lowering_player_id; 	// Who's lowering the gate?
	char		team;			// Which team's gate is being lowered
	double		progress;		// How much has the gate gone down? 0 == not at all .. 1 == all the way
	reader >> lowering_player_id >> team >> progress;
	
	if (progress == 0) {
		m_sound_controller->play_sound("gatelower");
	}
	
	m_map->set_gate_progress(team, progress);

	// TODO: use the player id to display a HUD message or something...
}

void GameController::game_start(PacketReader& reader) {
	string 		mapname;
	bool		game_started;
	uint32_t	timeleft;
	reader >> mapname >> game_started >> timeleft;

	// Load the map
	mapname += ".map";
	m_map->load_file(m_path_manager->data_path(mapname.c_str(), "maps"));
	m_map_width = m_map->get_width();
	m_map_height = m_map->get_height();

	// Tell the player what's going on
	ostringstream	message;
	if (game_started) {
		message << "Game started!";
		if (timeleft > 0) {
			message << " Time until spawn: " << timeleft;
		}
		m_sound_controller->play_sound("begin");
	} else {
		message << "Game starts in " << timeleft;
	}
	
	display_message(message.str().c_str(), 1.0, 1.0, 1.0);
}

void GameController::game_stop(PacketReader& reader) {
	char		winningteam;
	int 		teamascore;
	int		teambscore;
	
	reader >> winningteam >> teamascore >> teambscore;
	
	if (winningteam == m_players[m_player_id].get_team()) {
		display_message("VICTORY!", 1.0, 1.0, 1.0);
		m_sound_controller->play_sound("victory");
	} else {
		display_message("DEFEAT!", 1.0, 1.0, 1.0);
		m_sound_controller->play_sound("defeat");
	}

	// Temporary score display
	ostringstream	score_msg;
	score_msg << "Team A: " << teamascore << " / Team B: " << teambscore;
	display_message(score_msg.str().c_str(), 1.0, 1.0, 1.0);
	// End temporary score display

	m_map->reset_gates();
	m_players[m_player_id].set_is_invisible(true);
	m_players[m_player_id].set_is_frozen(true);
}

void GameController::score_update(PacketReader& reader) {
	uint32_t	player_id;
	int		score;
	reader >> player_id >> score;

	if (GraphicalPlayer* player = get_player_by_id(player_id)) {
		player->set_score(score);
	}
}

void GameController::animation_packet(PacketReader& reader) {
	uint32_t	player_id;
	string		sprite;
	string		field;
	int		value;
	
	reader >> player_id >> sprite >> field >> value;
	
	Graphic* the_sprite;
	if (sprite == "all") {
		the_sprite = m_players[player_id].get_sprite();
	} else {
		the_sprite = m_players[player_id].get_sprite()->get_graphic(sprite);
	}
	
	if (the_sprite == NULL) {
		return;
	}
	
	if (field == "rotation") {
		the_sprite->set_rotation(value);
	} else if (field == "scale_x") {
		the_sprite->set_scale_x(value);
	} else if (field == "scale_y") {
		the_sprite->set_scale_y(value);
	} else if (field == "x") {
		the_sprite->set_x(value);
	} else if (field == "y") {
		the_sprite->set_y(value);
	} else if (field == "center_x") {
		the_sprite->set_center_x(value);
	} else if (field == "center_y") {
		the_sprite->set_center_y(value);
	}
}

void GameController::send_animation_packet(string sprite, string field, int value) {
	PacketWriter animation_packet(PLAYER_ANIMATION_PACKET);
	animation_packet << m_player_id << sprite << field << value;
	
	m_network.send_packet(animation_packet);
}

void GameController::send_gate_hold(bool holding) {
	PacketWriter gate_hold(GATE_LOWERING_PACKET);
	if (holding) {
		gate_hold << m_player_id << get_other_team(m_players[m_player_id].get_team()) << 1;
	} else {
		gate_hold << m_player_id << get_other_team(m_players[m_player_id].get_team()) << 0;
	}
	m_network.send_packet(gate_hold);
}

void GameController::display_message(string message, double red, double green, double blue) {
	m_text_manager->set_active_color(red, green, blue);
	int y = 20 + (m_font->ascent() + m_font->descent() + 5) * m_messages.size();
	Graphic* message_sprite = m_text_manager->place_string(message, 20, y, TextManager::LEFT, TextManager::LAYER_HUD);
	m_messages.push_back(pair<Graphic*, int>(message_sprite, SDL_GetTicks() + MESSAGE_DISPLAY_TIME));
}

GraphicalPlayer* GameController::get_player_by_id(unsigned int player_id) {
	map<int, GraphicalPlayer>::iterator it(m_players.find(player_id));
	return it == m_players.end() ? NULL : &it->second;
}

