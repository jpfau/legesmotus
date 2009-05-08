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
#include "TableBackground.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include "common/network.hpp"
#include "common/math.hpp"
#include "common/team.hpp"
#include "common/StringTokenizer.hpp"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

const int GameController::MESSAGE_DISPLAY_TIME = 10000;
const int GameController::FIRING_DELAY = 700;
const int GameController::SHOT_DISPLAY_TIME = 180;
const int GameController::MUZZLE_FLASH_LENGTH = 80;
const int GameController::GATE_WARNING_FLASH_LENGTH = 3000;
const double GameController::FIRING_RECOIL = 1.5;
const double GameController::RANDOM_ROTATION_SCALE = 1.0;
const double GameController::MINIMAP_SCALE = 0.1;
const Color GameController::BLUE_COLOR(0.4, 0.4, 1);
const Color GameController::RED_COLOR(1, 0.4, 0.4);

GameController::GameController() {
	init(1024, 768, 24, false);
}

GameController::GameController(int width, int height) {
	init(width, height, 24, true);
}

/*
 * Delete all of the sprites and subsystems.
 */
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
	
	m_text_manager->remove_all_strings();

	// TEMPORARY MAP CODE BY ANDREW
	delete m_map;

	delete m_text_manager;
	delete m_sound_controller;
	delete m_font;
	delete m_path_manager;
	delete m_shot;

	m_minimap->unregister_with_window(m_window);
	delete m_minimap;

	// The GameWindow instance should always be destroyed last, since other stuff may depend on it.
	m_window->destroy_instance();
}

/*
 * Initialize all of the subsystems, sprites, etc.
 */
void GameController::init(int width, int height, int depth, bool fullscreen) {
	srand ( time(NULL) );
	
	initialize_key_bindings();
	
	// Initial game state will be showing the main menu.
	m_game_state = SHOW_MENUS;
	
	m_screen_width = width;
	m_screen_height = height;
	
	m_input_text = "> ";
	m_input_bar = NULL;
	
	m_client_version = "0.0.3";
	m_protocol_number = 1;
	
	m_pixel_depth = depth;
	m_fullscreen = fullscreen;
	m_quit_game = false;
	m_window = GameWindow::get_instance(m_screen_width, m_screen_height, m_pixel_depth, m_fullscreen);

	m_path_manager = new PathManager(""); // TODO: Pass it a path.

	m_time_to_unfreeze = 0;
	m_last_fired = 0;

	m_font = new Font(m_path_manager->data_path("JuraMedium.ttf", "fonts"), 14);
	m_text_manager = new TextManager(m_font, m_window);
	
	m_menu_font = new Font(m_path_manager->data_path("JuraDemiBold.ttf", "fonts"), 34);
	m_medium_font = new Font(m_path_manager->data_path("JuraMedium.ttf", "fonts"), 20);
	
	m_sound_controller = new SoundController();
	m_holding_gate = false;

	m_map = new GraphicalMap(m_window);
	m_map_width = 0;
	m_map_height = 0;

	// Initialize the gun sprites.
	gun_normal = new Sprite(m_path_manager->data_path("gun_noshot.png", "sprites"));
	gun_normal->set_x(3);
	gun_normal->set_y(19);
	gun_normal->set_rotation(-15);
	gun_normal->set_priority(-1);
	
	m_gun_fired = new Sprite(m_path_manager->data_path("gun_shot.png", "sprites"));
	m_gun_fired->set_x(3);
	m_gun_fired->set_y(19);
	m_gun_fired->set_rotation(-15);
	m_gun_fired->set_priority(-1);
	m_gun_fired->set_invisible(true);
	
	// Initialize all of the components of the player sprites.
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
	blue_arm_gun.add_graphic(m_gun_fired, "gun_fired");
	blue_arm_gun.add_graphic(blue_front_arm, "arm");
	blue_arm_gun.set_center_x(13);
	blue_arm_gun.set_center_y(-18);
	blue_arm_gun.set_x(13);
	blue_arm_gun.set_y(-18);
	blue_player.add_graphic(&blue_arm_gun, "frontarm");
	blue_player.set_invisible(true);
	
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
	red_arm_gun.add_graphic(m_gun_fired, "gun_fired");
	red_arm_gun.add_graphic(red_front_arm, "arm");
	red_arm_gun.set_center_x(13);
	red_arm_gun.set_center_y(-18);
	red_arm_gun.set_x(13);
	red_arm_gun.set_y(-18);
	red_player.add_graphic(&red_arm_gun, "frontarm");
	red_player.set_invisible(true);
	
	m_crosshairs = new Sprite("data/sprites/crosshairs.png");
	m_crosshairs->set_priority(-1);
	m_window->register_hud_graphic(m_crosshairs);
	
	m_shot = new Sprite("data/sprites/shot.png");
	m_shot->set_invisible(true);
	
	m_logo = new Sprite("data/sprites/legesmotuslogo.png");
	m_logo->set_x(m_screen_width/2);
	m_logo->set_y(100);
	m_logo->set_priority(-1);
	m_window->register_hud_graphic(m_logo);
	
	m_main_menu_items = map<string, Graphic*>();
	m_options_menu_items = map<string, Graphic*>();
	
	// Set the text manager to draw a shadow behind everything.
	m_text_manager->set_active_font(m_menu_font);
	m_text_manager->set_shadow_color(0.0, 0.0, 0.0);
	m_text_manager->set_shadow_alpha(0.7);
	m_text_manager->set_shadow_offset(1.0, 1.0);
	m_text_manager->set_shadow(true);
	
	// Initialize all of the menu items.
	m_main_menu_items["Resume Game"] = m_text_manager->place_string("Resume Game", 50, 200, TextManager::LEFT, TextManager::LAYER_HUD);
	m_main_menu_items["Options"] = m_text_manager->place_string("Options", 50, 250, TextManager::LEFT, TextManager::LAYER_HUD);
	m_main_menu_items["Quit"] = m_text_manager->place_string("Quit", 50, 300, TextManager::LEFT, TextManager::LAYER_HUD);
	m_main_menu_items["Thanks"] = m_text_manager->place_string("Thanks for playing! Please visit", 50, 400, TextManager::LEFT, TextManager::LAYER_HUD);
 	m_text_manager->set_active_color(0.4, 1.0, 0.4);
 	m_main_menu_items["Thanks2"] = m_text_manager->place_string("blug.brown.edu/legesmotus", 50, 440, TextManager::LEFT, TextManager::LAYER_HUD);
 	m_text_manager->set_active_color(1.0, 1.0, 1.0);
 	m_main_menu_items["Thanks3"] = m_text_manager->place_string("to leave feedback for us!", 50, 480, TextManager::LEFT, TextManager::LAYER_HUD);
	m_options_menu_items["Back"] = m_text_manager->place_string("Back", 50, 200, TextManager::LEFT, TextManager::LAYER_HUD);
	m_options_menu_items["Enter Name"] = m_text_manager->place_string("Enter Name", 50, 250, TextManager::LEFT, TextManager::LAYER_HUD);
	m_options_menu_items["Toggle Sound"] = m_text_manager->place_string("Toggle Sound", 50, 300, TextManager::LEFT, TextManager::LAYER_HUD);
	
	// Initialize the overlay.
	m_overlay_background = new TableBackground(3, m_screen_width - 300);
	m_overlay_background->set_row_height(0, 80);
	m_overlay_background->set_row_height(1, 43);
	m_overlay_background->set_row_height(2, m_screen_height - 323);
	m_overlay_background->set_priority(-2);
	m_overlay_background->set_border_color(Color(1,1,1,0.8));
	m_overlay_background->set_border_width(2);
	m_overlay_background->set_cell_color(0,Color(0.1,0.1,0.1,0.8));
	m_overlay_background->set_cell_color(1,Color(0.2,0.1,0.1,0.8));
	m_overlay_background->set_cell_color(2,Color(0.1,0.1,0.15,0.8));
	m_overlay_background->set_y(100);
	m_overlay_background->set_x(m_screen_width/2);
	m_window->register_hud_graphic(m_overlay_background);
	m_overlay_items["red label"] = m_text_manager->place_string("Red Team:", m_overlay_background->get_x() - m_overlay_background->get_image_width()/2 + 3, 115, TextManager::LEFT, TextManager::LAYER_HUD);
	m_overlay_items["blue label"] = m_text_manager->place_string("Blue Team:", m_overlay_background->get_x(), 115, TextManager::LEFT, TextManager::LAYER_HUD);
	m_text_manager->set_active_font(m_medium_font);
	m_overlay_items["name label"] = m_text_manager->place_string("Name:", m_overlay_background->get_x() - m_overlay_background->get_image_width()/2 + 3, 190, TextManager::LEFT, TextManager::LAYER_HUD);
	m_overlay_items["score label"] = m_text_manager->place_string("Score:", m_overlay_background->get_x(), 190, TextManager::LEFT, TextManager::LAYER_HUD);
	m_text_manager->set_active_font(m_menu_font);
	change_team_scores(0, 0);
	update_individual_scores();
		
	map<string, Graphic*>::iterator it;
	for ( it=m_overlay_items.begin() ; it != m_overlay_items.end(); it++ ) {
		Graphic* thisitem = (*it).second;
		thisitem->set_priority(-4);
		m_window->unregister_graphic(thisitem);
		m_window->register_hud_graphic(thisitem);
	}
	
	m_text_manager->set_active_color(1.0, 0.4, 0.4);
	m_gate_warning = m_text_manager->place_string("Your gate is going down!", m_screen_width/2, m_screen_height - 200, TextManager::CENTER, TextManager::LAYER_HUD);
	m_gate_warning->set_invisible(true);
	m_gate_warning_time = 0;
	
	m_text_manager->set_active_font(m_font);
	
	// Set up the minimap.
	m_minimap = new Minimap(MINIMAP_SCALE);
	m_minimap->set_x(m_screen_width - 120);
	m_minimap->set_y(120);
	m_minimap->register_with_window(m_window);
}

/*
 * The main game loop.
 */
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
		
		// Check if my player is set to unfreeze.
		if (!m_players.empty() && m_time_to_unfreeze < SDL_GetTicks() && m_time_to_unfreeze != 0) {
			m_sound_controller->play_sound("unfreeze");
			m_players[m_player_id].set_is_frozen(false);
			m_time_to_unfreeze = 0;
		}
		
		// Update movement and graphics if frame rate is correct.
		unsigned long currframe = SDL_GetTicks();
		if((currframe - startframe) >= delay) {
			// Erase messages that are too old.
			for (unsigned int i = 0; i < m_messages.size(); i++) {
				if (m_messages[i].second < currframe) {
					m_text_manager->remove_string(m_messages[i].first);
					m_messages.erase(m_messages.begin() + i);
				}
			}
			
			// Reposition messages that remain after removing.
			for (unsigned int i = 0; i < m_messages.size(); i++) {
				int y = 20 + (m_font->ascent() + m_font->descent() + 5) * i;
				m_text_manager->reposition_string(m_messages[i].first, 20, y, TextManager::LEFT);
			}
			
			// Change shot displays based on time.
			for (unsigned int i = 0; i < m_shots.size(); i++) {
				double shot_time = (double)(SHOT_DISPLAY_TIME-(m_shots[i].second - currframe))/SHOT_DISPLAY_TIME;
				double shot_curve = -4.5*shot_time*(shot_time-1.0)/(shot_time+0.5); //fancy curve
				m_shots[i].first->set_scale_x(shot_curve);
				m_shots[i].first->set_scale_y(shot_curve);
				m_shots[i].first->set_rotation(shot_time*90.0);
				if (m_shots[i].second < currframe) {
					m_window->unregister_graphic(m_shots[i].first);
					delete m_shots[i].first;
					m_shots.erase(m_shots.begin() + i);
				}
			}
			
			// Change the display of the gate warning message based on time.
			if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - GATE_WARNING_FLASH_LENGTH) {
				m_gate_warning_time = 0;
				m_gate_warning->set_invisible(true);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (5*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(false);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (4*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(true);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (3*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(false);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (2*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(true);
			} else if (m_gate_warning_time != 0 && m_gate_warning_time < currframe - (1*GATE_WARNING_FLASH_LENGTH)/6) {
				m_gate_warning->set_invisible(false);
			}
						
			move_objects((SDL_GetTicks() - lastmoveframe) / delay); // scale all position changes to keep game speed constant. 
			
			lastmoveframe = SDL_GetTicks();
			
			// TODO: Uncomment if framerate is needed.
			// the framerate:
			//int framerate = (1000/(currframe - startframe));
			
			if (!m_players.empty()) {
				// Change gun sprite if muzzle flash is done.
				Graphic* frontarm = m_players[m_player_id].get_sprite()->get_graphic("frontarm");
				if (m_last_fired < SDL_GetTicks() - MUZZLE_FLASH_LENGTH && frontarm->get_graphic("gun")->is_invisible()) {
					frontarm->get_graphic("gun")->set_invisible(false);
					send_animation_packet("frontarm/gun", "invisible", false);
					frontarm->get_graphic("gun_fired")->set_invisible(true);
					send_animation_packet("frontarm/gun_fired", "invisible", true);
				}
							
				m_crosshairs->set_x(m_mouse_x);
				m_crosshairs->set_y(m_mouse_y);
				
				// Turn arm of player to face crosshairs.
				if (!m_players[m_player_id].is_frozen()) {
					double x_dist;
					double y_dist;
					double angle;
					x_dist = (m_crosshairs->get_x() + m_offset_x) - m_players[m_player_id].get_x();
					y_dist = (m_crosshairs->get_y() + m_offset_y) - m_players[m_player_id].get_y();
					angle = get_normalized_angle(atan2(y_dist, x_dist) * RADIANS_TO_DEGREES - m_players[m_player_id].get_rotation_degrees());
					
					if (angle < 90 || angle > 270) {
						m_players[m_player_id].get_sprite()->set_scale_x(-1);
						send_animation_packet("all", "scale_x", -1);
						angle *= -1;
						angle += 55;
					} else {
						m_players[m_player_id].get_sprite()->set_scale_x(1);
						send_animation_packet("all", "scale_x", 1);
						angle -= 120;
					}
					m_players[m_player_id].get_sprite()->get_graphic("frontarm")->set_rotation(angle);
					send_animation_packet("frontarm", "rotation", angle);
				}
				send_my_player_update();
				
				// Set the new offset of the window so that the player is centered.
				m_offset_x = m_players[m_player_id].get_x() - (m_screen_width/2.0);
				m_offset_y = m_players[m_player_id].get_y() - (m_screen_height/2.0);
				m_window->set_offset_x(m_offset_x);
				m_window->set_offset_y(m_offset_y);
			}
			
			// Show and hide elements depending on game state (started, menus, etc.)
			if (m_game_state == SHOW_MENUS) {
				if (m_map != NULL) {
					m_map->set_visible(false);
				}
				set_players_visible(false);
				
				m_minimap->set_invisible(true);
				
				m_logo->set_invisible(false);
				map<string, Graphic*>::iterator it;
				for ( it=m_main_menu_items.begin() ; it != m_main_menu_items.end(); it++ ) {
					Graphic* thisitem = (*it).second;
					thisitem->set_invisible(false);
				}
				for ( it=m_options_menu_items.begin() ; it != m_options_menu_items.end(); it++ ) {
					Graphic* thisitem = (*it).second;
					thisitem->set_invisible(true);
				}
			} else if (m_game_state == SHOW_OPTIONS_MENU) {
				if (m_map != NULL) {
					m_map->set_visible(false);
				}
				set_players_visible(false);
				
				m_minimap->set_invisible(true);
				
				m_logo->set_invisible(false);
				map<string, Graphic*>::iterator it;
				for ( it=m_main_menu_items.begin() ; it != m_main_menu_items.end(); it++ ) {
					Graphic* thisitem = (*it).second;
					thisitem->set_invisible(true);
				}
				for ( it=m_options_menu_items.begin() ; it != m_options_menu_items.end(); it++ ) {
					Graphic* thisitem = (*it).second;
					thisitem->set_invisible(false);
				}
			} else {
				if (m_map != NULL) {
					m_map->set_visible(true);
				}
				set_players_visible(true);
				
				m_minimap->set_invisible(false);
				
				m_logo->set_invisible(true);
				map<string, Graphic*>::iterator it;
				for ( it=m_main_menu_items.begin() ; it != m_main_menu_items.end(); it++ ) {
					Graphic* thisitem = (*it).second;
					thisitem->set_invisible(true);
				}
				for ( it=m_options_menu_items.begin() ; it != m_options_menu_items.end(); it++ ) {
					Graphic* thisitem = (*it).second;
					thisitem->set_invisible(true);
				}
			}
			
			m_window->redraw();
			startframe = SDL_GetTicks();
		}
	}
	
	disconnect();
}

/*
 * Set the dimensions of the screen.
 */
void GameController::set_screen_dimensions(int width, int height) {
	m_screen_width = width;
	m_screen_height = height;
}

/*
 * Process the current SDL input state.
 */
void GameController::process_input() {
	SDL_Event event;
	while(SDL_PollEvent(&event) != 0) {
		switch(event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == m_key_bindings.quit || event.key.keysym.sym == m_alt_key_bindings.quit) {
					cerr << "Quit key pressed - quitting." << endl;
					m_quit_game = true;
				}
				
				// If we're typing into the input bar...
				if (m_input_bar != NULL) {
					m_text_manager->set_active_color(Color::WHITE);
					// If we're going back to the menu, remove the input bar.
					if (event.key.keysym.sym == m_key_bindings.show_menu || event.key.keysym.sym == m_alt_key_bindings.show_menu) {
						SDL_EnableUNICODE(0);
						SDL_EnableKeyRepeat(0, 0); // Disable key repeat
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = NULL;
						m_input_text = "> ";
					} else if (event.key.keysym.sym == m_key_bindings.send_chat || event.key.keysym.sym == m_alt_key_bindings.send_chat) {
						// Remove the "> " from the front.
						string message = m_input_text.substr(2);
						
						// Check message for commands.
						if (message.find("/name ") == 0) {
							string new_name(message.substr(6));
							send_name_change_packet(new_name.c_str());
						} else if (message.find("/team ") == 0) {
							string	new_team_string(message.substr(6));
							char	new_team = parse_team_string(new_team_string.c_str());
							if (is_valid_team(new_team)) {
								send_team_change_packet(new_team);
							} else {
								display_message("Please enter '/team blue' or '/team red'");
							}
						} else if (message.find("/tchat ") == 0) {
							send_team_message(message.substr(7));
						} else {
							send_message(message);
						}
					
						// Remove the input bar.
						SDL_EnableUNICODE(0);
						SDL_EnableKeyRepeat(0, 0); // Disable key repeat
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = NULL;
						m_input_text = "> ";
					} else if (event.key.keysym.sym == SDLK_BACKSPACE) {
						// Delete text.
						if (m_input_text.length() <= 2) {
							break;
						}
						m_input_text.erase(m_input_text.length() - 1);
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
					} else {
						// Otherwise, it's a regular character. Type it in.
						if ( (event.key.keysym.unicode & 0xFF80) == 0 && event.key.keysym.unicode != 0) {
							m_input_text.push_back(event.key.keysym.unicode & 0x7F);
						} else {
							// INTERNATIONAL CHARACTER... DO SOMETHING.
						}
						// Replace the text display with the new one.
						m_text_manager->remove_string(m_input_bar);
						m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
					}
				} else {
					//Check which key using: event.key.keysym.sym == SDLK_<SOMETHING>
					if (event.key.keysym.sym == m_key_bindings.jump || event.key.keysym.sym == m_alt_key_bindings.jump) {
						attempt_jump();
					} else if (event.key.keysym.sym == m_key_bindings.show_overlay || event.key.keysym.sym == m_alt_key_bindings.show_overlay) {
						// TODO: Show the overlay.
					} else if (event.key.keysym.sym == m_key_bindings.open_chat || event.key.keysym.sym == m_key_bindings.open_console || event.key.keysym.sym == m_alt_key_bindings.open_chat || event.key.keysym.sym == m_alt_key_bindings.open_console) {
						SDL_EnableUNICODE(1);
						SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
						m_text_manager->set_active_color(Color::WHITE);
						if (m_input_bar == NULL) {
							m_input_bar = m_text_manager->place_string("> ", 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
						}
					} else if (event.key.keysym.sym == m_key_bindings.open_team_chat || event.key.keysym.sym == m_alt_key_bindings.open_team_chat) {
						SDL_EnableUNICODE(1);
						SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
						m_text_manager->set_active_color(Color::WHITE);
						m_input_text = "> /tchat ";
						if (m_input_bar == NULL) {
							m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
						}
					} else if (event.key.keysym.sym == m_key_bindings.show_menu || event.key.keysym.sym == m_alt_key_bindings.show_menu) {
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
				// Use: event.motion.xrel, event.motion.yrel (changes in position), event.motion.x, event.motion.y
				m_mouse_x = event.motion.x;
				m_mouse_y = event.motion.y;
				m_crosshairs->set_x(m_mouse_x);
				m_crosshairs->set_y(m_mouse_y);
				
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
/*
 * Set which keys are used for which functions.
 */
void GameController::initialize_key_bindings() {
	// -1 = unused
	m_key_bindings.quit = -1;
	m_key_bindings.jump = SDLK_SPACE;
	m_key_bindings.show_overlay = SDLK_TAB;
	m_key_bindings.show_menu = SDLK_ESCAPE;
	m_key_bindings.open_chat = SDLK_t;
	m_key_bindings.open_team_chat = SDLK_y;
	m_key_bindings.open_console = SDLK_BACKQUOTE;
	m_key_bindings.send_chat = SDLK_RETURN;

	m_alt_key_bindings.quit = -1;
	m_alt_key_bindings.jump = -1;
	m_alt_key_bindings.show_overlay = -1;
	m_alt_key_bindings.show_menu = -1;
	m_alt_key_bindings.open_chat = -1;
	m_alt_key_bindings.open_team_chat = -1;
	m_alt_key_bindings.open_console = -1;
	m_alt_key_bindings.send_chat = SDLK_KP_ENTER;
}

/*
 * Used to process any keys that are to be held down rather than pressed once.
 */
void GameController::parse_key_input() {
	// For keys that can be held down:
   	m_keys = SDL_GetKeyState(NULL);
   	
   	if (m_game_state == GAME_IN_PROGRESS && m_input_bar == NULL) {
		if ((m_key_bindings.jump != -1 && m_keys[m_key_bindings.jump]) || (m_alt_key_bindings.jump != -1 && m_keys[m_alt_key_bindings.jump])) {
			attempt_jump();
		}
	}
	
	if ((m_key_bindings.show_overlay != -1 && m_keys[m_key_bindings.show_overlay]) || (m_alt_key_bindings.show_overlay != -1 && m_keys[m_alt_key_bindings.show_overlay])) {
		if (m_game_state == GAME_IN_PROGRESS) {
			if (m_overlay_background->is_invisible() == true) {
				toggle_score_overlay(true);
			}
		} else if (m_game_state != GAME_OVER) {
			if (m_overlay_background->is_invisible() == false) {
				toggle_score_overlay(false);
			}
		}
	} else {
		if (m_game_state != GAME_OVER && m_overlay_background->is_invisible() == false) {
			toggle_score_overlay(false);
		}
	}
}

/*
 * Process a mouse click, depending on the game state.
 */
void GameController::process_mouse_click(SDL_Event event) {
	if (m_game_state == SHOW_MENUS) {
		// Check each item in the menu to see if the mouse is clicking on it.
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
					m_game_state = SHOW_OPTIONS_MENU;
				} else if ((*it).first == "Resume Game") {
					if (!m_players.empty()) {
						m_game_state = GAME_IN_PROGRESS;
					}
				}
				m_sound_controller->play_sound("click");
			}
		}
	} else if (m_game_state == SHOW_OPTIONS_MENU) {
		// Check each item in the options menu.
		map<string, Graphic*>::iterator it;
		for ( it=m_options_menu_items.begin() ; it != m_options_menu_items.end(); it++ ) {
			Graphic* thisitem = (*it).second;
			int x = thisitem->get_x();
			int y = thisitem->get_y();
			if (event.button.x >= x && event.button.x <= x + thisitem->get_image_width()
			    && event.button.y >= y && event.button.y <= y + thisitem->get_image_height()) {
				if ((*it).first == "Back") {
					m_game_state = SHOW_MENUS;
				} else if ((*it).first == "Enter Name") {
					// Open the input bar and allow the name to be entered.
					// Should replace later, to use a separate text entry location.
					SDL_EnableUNICODE(1);
					m_text_manager->set_active_color(Color::WHITE);
					m_input_text = "> /name ";
					m_text_manager->remove_string(m_input_bar);
					m_input_bar = m_text_manager->place_string(m_input_text, 20, m_screen_height-100, TextManager::LEFT, TextManager::LAYER_HUD);
				} else if ((*it).first == "Toggle Sound") {
					if (m_sound_controller->is_sound_on()) {
						m_sound_controller->set_sound_on(false);
						display_message("Sound is now OFF.");
					} else {
						m_sound_controller->set_sound_on(true);
						display_message("Sound is now ON.");
					}
				}
				m_sound_controller->play_sound("click");
			}
		}
	} else if (m_game_state == GAME_IN_PROGRESS) {
		if (event.button.button == 1) {
			// Fire the gun if it's ready.
			if (m_last_fired != 0 && m_last_fired > SDL_GetTicks() - FIRING_DELAY) {
				return;
			}
			if (m_players.empty() || m_players[m_player_id].is_frozen()) {
				return;
			}
			double x_dist = (event.button.x + m_offset_x) - m_players[m_player_id].get_x();
			double y_dist = (event.button.y + m_offset_y) - m_players[m_player_id].get_y();
			double direction = atan2(y_dist, x_dist) * RADIANS_TO_DEGREES;
			// Cause recoil if the player is not hanging onto a wall.
			if (m_players[m_player_id].get_x_vel() != 0 || m_players[m_player_id].get_y_vel() != 0) {
				m_players[m_player_id].set_velocity(m_players[m_player_id].get_x_vel() - FIRING_RECOIL * cos((direction) * DEGREES_TO_RADIANS), m_players[m_player_id].get_y_vel() - FIRING_RECOIL * sin((direction) * DEGREES_TO_RADIANS));
			}
			m_last_fired = SDL_GetTicks();
			player_fired(m_player_id, m_players[m_player_id].get_x(), m_players[m_player_id].get_y(), direction);
			m_sound_controller->play_sound("fire");
		}
	}
}

/*
 * Do the movement of objects in a certain time scale.
 */
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
	
	// Check if the player is hitting a map edge.
	if (new_x - half_width < 0) {
		new_x = half_width;
		new_y = m_players[m_player_id].get_y();
		if (m_players[m_player_id].is_frozen() && !m_players[m_player_id].is_invisible()) {
			m_players[m_player_id].set_x_vel(-m_players[m_player_id].get_x_vel() * .9);
			m_players[m_player_id].set_y_vel(m_players[m_player_id].get_y_vel() * .9);
		} else {
			m_players[m_player_id].set_velocity(0, 0);
			m_players[m_player_id].set_rotational_vel(0);
		}
	} else if (new_x + half_width > m_map_width) {
		new_x = m_map_width - half_width;
		new_y = m_players[m_player_id].get_y();
		if (m_players[m_player_id].is_frozen() && !m_players[m_player_id].is_invisible()) {
			m_players[m_player_id].set_x_vel(-m_players[m_player_id].get_x_vel() * .9);
			m_players[m_player_id].set_y_vel(m_players[m_player_id].get_y_vel() * .9);
		} else {
			m_players[m_player_id].set_velocity(0, 0);
			m_players[m_player_id].set_rotational_vel(0);
		}
	}
	
	if (new_y - half_height < 0) {
		new_y = half_height;
		new_x = m_players[m_player_id].get_x();
		if (m_players[m_player_id].is_frozen() && !m_players[m_player_id].is_invisible()) {
			m_players[m_player_id].set_x_vel(m_players[m_player_id].get_x_vel() * .9);
			m_players[m_player_id].set_y_vel(-m_players[m_player_id].get_y_vel() * .9);
		} else {
			m_players[m_player_id].set_velocity(0, 0);
			m_players[m_player_id].set_rotational_vel(0);
		}
	} else if (new_y + half_height > m_map_height) {
		new_y = m_map_height - half_height;
		new_x = m_players[m_player_id].get_x();
		if (m_players[m_player_id].is_frozen() && !m_players[m_player_id].is_invisible()) {
			m_players[m_player_id].set_x_vel(m_players[m_player_id].get_x_vel() * .9);
			m_players[m_player_id].set_y_vel(-m_players[m_player_id].get_y_vel() * .9);
		} else {
			m_players[m_player_id].set_velocity(0, 0);
			m_players[m_player_id].set_rotational_vel(0);
		}
	}
	
	bool holdinggate = false;
	
	const list<MapObject>& map_objects(m_map->get_objects());
	list<MapObject>::const_iterator thisobj;
	int radius = m_players[m_player_id].get_radius();
	Point currpos = Point(new_x, new_y);
	Point oldpos = Point(m_players[m_player_id].get_x(), m_players[m_player_id].get_y());
	
	// Check each object for collisions with the player.
	for (thisobj = map_objects.begin(); thisobj != map_objects.end(); thisobj++) {
		if (thisobj->get_sprite() == NULL) {
			continue;
		}
		const Polygon& poly(thisobj->get_bounding_polygon());
		double angle_of_incidence = 0;
		double newdist = poly.intersects_circle(currpos, radius, &angle_of_incidence);
		angle_of_incidence = get_normalized_angle(get_normalized_angle(angle_of_incidence+180));
		double olddist = poly.intersects_circle(oldpos, radius, NULL);
	
		// REPEL FROZEN PLAYERS AWAY FROM GATES.
		if (thisobj->get_type() == Map::GATE && m_players[m_player_id].is_frozen() && !m_players[m_player_id].is_invisible()) {
			double newdist_repulsion = poly.dist_from_circle(currpos, radius);
			if (newdist_repulsion < 400) {
				double gate_x = thisobj->get_upper_left().x + thisobj->get_sprite()->get_image_width()/2;
				double gate_y = thisobj->get_upper_left().y + thisobj->get_sprite()->get_image_height()/2;
				double angle = atan2(gate_y - new_y, gate_x - new_x);
				m_players[m_player_id].set_velocity(m_players[m_player_id].get_x_vel() - .01 * cos(angle), m_players[m_player_id].get_y_vel() - .01 * sin(angle));
			}
		}
	
		// If we're hitting the object...
		if (newdist != -1) {
			// If we're moving closer to the object, we need to stop.
			if (newdist < olddist) {
				if (thisobj->get_type() == Map::OBSTACLE) {
					// If we're frozen, bounce off the wall.
					if (m_players[m_player_id].is_frozen() && !m_players[m_player_id].is_invisible()) {
						double xvel = m_players[m_player_id].get_x_vel();
						double yvel = m_players[m_player_id].get_y_vel();
						double my_angle = get_normalized_angle(atan2(yvel, xvel) * RADIANS_TO_DEGREES);
						double new_angle = get_normalized_angle(angle_of_incidence + (angle_of_incidence - my_angle) - 180);
						double vel_magnitude = sqrt(xvel * xvel + yvel * yvel);
						m_players[m_player_id].set_velocity(vel_magnitude * cos(new_angle * DEGREES_TO_RADIANS) * .9, vel_magnitude * sin(new_angle * DEGREES_TO_RADIANS) * .9);
						//new_x = m_players[m_player_id].get_x() + m_players[m_player_id].get_x_vel() * timescale;
						//new_y = m_players[m_player_id].get_y() + m_players[m_player_id].get_y_vel() * timescale;
						new_x = m_players[m_player_id].get_x();
						new_y = m_players[m_player_id].get_y();
					} else {
						m_players[m_player_id].set_velocity(0, 0);
						m_players[m_player_id].set_rotational_vel(0);
						new_x = m_players[m_player_id].get_x();
						new_y = m_players[m_player_id].get_y();
					}
				}
			}
			// If it's a gate, we're lowering it if we're unfrozen and it's owned by the enemy team.
			if (thisobj->get_type() == Map::GATE && thisobj->get_team() != m_players[m_player_id].get_team() && !m_players[m_player_id].is_frozen()) {
				if (!m_holding_gate) {
					send_gate_hold(true);
				}
				m_holding_gate = true;
				holdinggate = true;
			}
		}
	}
	
	if (!holdinggate) {
		if (m_holding_gate) {
			send_gate_hold(false);
		}
		m_holding_gate = false;
	}
	
	// Set the player position and minimap position.
	m_players[m_player_id].set_x(new_x);
	m_players[m_player_id].set_y(new_y);
	m_minimap->move_blip(m_player_id, new_x, new_y);
	m_minimap->recenter(new_x, new_y);
	m_players[m_player_id].set_rotation_degrees(m_players[m_player_id].get_rotation_degrees() + m_players[m_player_id].get_rotational_vel() * timescale);
	
	// Set name sprites visible/invisible. and move players.
	map<int, GraphicalPlayer>::iterator it;
	for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
		GraphicalPlayer currplayer = (*it).second;
		if (currplayer.is_invisible()) {
			currplayer.get_name_sprite()->set_invisible(true);
		} else {
			currplayer.get_name_sprite()->set_invisible(false);
			m_text_manager->reposition_string(currplayer.get_name_sprite(), currplayer.get_x(), currplayer.get_y() - (currplayer.get_radius()+30), TextManager::CENTER);
		}
		
		if (currplayer.get_id() == m_player_id) {
			continue;
		}
		
		m_players[currplayer.get_id()].set_x(currplayer.get_x() + currplayer.get_x_vel() * timescale);
		m_players[currplayer.get_id()].set_y(currplayer.get_y() + currplayer.get_y_vel() * timescale);
		m_minimap->move_blip(currplayer.get_id(), m_players[currplayer.get_id()].get_x(), m_players[currplayer.get_id()].get_y());
	}
}

/*
 * Try to jump off of an obstacle.
 */
void GameController::attempt_jump() {
	if (m_players.empty()) {
		return;
	}
	
	if (m_players[m_player_id].is_frozen()) {
		return;
	}
	
	GraphicalPlayer* player = &m_players[m_player_id];
	
	double new_rotation = (((double)rand() / ((double)(RAND_MAX)+1)) - 0.5) * RANDOM_ROTATION_SCALE;
	
	double half_width = m_players[m_player_id].get_radius();
	double half_height = m_players[m_player_id].get_radius();
	
	double x_dist = (m_crosshairs->get_x() + m_offset_x) - player->get_x();
	double y_dist = (m_crosshairs->get_y() + m_offset_y) - player->get_y();
	double x_vel = 6 * cos(atan2(y_dist, x_dist));
	double y_vel = 6 * sin(atan2(y_dist, x_dist));
	
	// Check if we're next to the side of the map.
	if (player->get_x() - (half_width) <= 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
		m_players[m_player_id].set_rotational_vel(new_rotation);
	} else if (player->get_x() + (half_width) >= m_map_width - 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
		m_players[m_player_id].set_rotational_vel(new_rotation);
	}
	
	if (player->get_y() - (half_height) <= 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
		m_players[m_player_id].set_rotational_vel(1);
	} else if (player->get_y() + (half_height) >= m_map_height - 5) {
		player->set_x_vel(x_vel);
		player->set_y_vel(y_vel);
		m_players[m_player_id].set_rotational_vel(new_rotation);
	}

	list<MapObject>::const_iterator thisobj;
	const list<MapObject>& map_objects(m_map->get_objects());
	Point currpos = Point(player->get_x(), player->get_y());
	
	// Check if we're near any obstacles.
	for (thisobj = map_objects.begin(); thisobj != map_objects.end(); thisobj++) {
		if (thisobj->get_sprite() == NULL) {
			continue;
		}
		if (thisobj->get_type() != Map::OBSTACLE) {
			// Only obstacles can be jumped from
			continue;
		}
		const Polygon& poly(thisobj->get_bounding_polygon());
		double newdist = poly.intersects_circle(currpos, player->get_radius()+5, NULL);
		if (newdist != -1) {
			player->set_x_vel(x_vel);
			player->set_y_vel(y_vel);	
			m_players[m_player_id].set_rotational_vel(new_rotation);
		}
	}
}

/*
 * Called when a player (including you) fires.
 */
void GameController::player_fired(unsigned int player_id, double start_x, double start_y, double direction) {
	if (m_players.empty()) {
		return;
	}
	
	const list<MapObject>& map_objects(m_map->get_objects());
	list<MapObject>::const_iterator thisobj;
	Point startpos = Point(start_x, start_y);
	
	double shortestdist = numeric_limits<double>::max();
	Point wallhitpoint = Point(0, 0);
	double end_x = -1;
	double end_y = -1;
	
	// Find the closest object that the shot will hit, if any.
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
	
	// Now check if any players are closer.
	if (player_id == m_player_id) {
		double player_hit = -1;
		
		map<int, GraphicalPlayer>::iterator it;
		for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
			GraphicalPlayer currplayer = (*it).second;
			if (currplayer.get_id() == player_id) {
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
		
		// If we're still not hitting anything, find the nearest map edge where the shot will hit.
		if (end_x == -1 && end_y == -1) {
			double dist_to_obstacle = m_map_width + m_map_height;
			Point endpos = Point(start_x + dist_to_obstacle * cos(direction * DEGREES_TO_RADIANS), start_y + dist_to_obstacle * sin(direction * DEGREES_TO_RADIANS));
			Point newpoint = m_map_polygon.intersects_line(startpos, endpos);
		
			if (newpoint.x != -1 || newpoint.y != -1) {		
				double newdist = dist_between_points(start_x, start_y, newpoint.x, newpoint.y);
		
				if (newdist != -1 && newdist < shortestdist) {
					shortestdist = newdist;
					wallhitpoint = newpoint;
					end_x = newpoint.x;
					end_y = newpoint.y;
				}
			}
		}
		
		// Create the gun fired packet and send it, and display the shot hit point.
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
		
		// Switch to the gun with the muzzle flash.
		GraphicGroup* frontarm = (GraphicGroup*)m_players[m_player_id].get_sprite()->get_graphic("frontarm");
		frontarm->get_graphic("gun")->set_invisible(true);
		send_animation_packet("frontarm/gun", "invisible", true);
		frontarm->get_graphic("gun_fired")->set_invisible(false);
		send_animation_packet("frontarm/gun_fired", "invisible", false);
		
		m_network.send_packet(gun_fired);
		
		if (player_hit != -1 && !m_players[player_hit].is_frozen()) {
			send_player_shot(player_id, player_hit, direction-180);
		}
	}
}

/*
 * Set the player sprites visible or invisible.
 */
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
			m_minimap->set_blip_invisible(it->first,currplayer.is_invisible());
		} else {
			currplayer.get_sprite()->set_invisible(true);
			currplayer.get_name_sprite()->set_invisible(true);
			m_minimap->set_blip_invisible(it->first,true);
		}
	}
}

/*
 * Show or hide the overlay.
 */
void GameController::toggle_score_overlay(bool visible) {
	update_individual_scores();
	m_overlay_background->set_invisible(!visible);
	map<string, Graphic*>::iterator it;
	for ( it=m_overlay_items.begin() ; it != m_overlay_items.end(); it++ ) {
		Graphic* thisitem = (*it).second;
		thisitem->set_invisible(!visible);
	}
}

/*
 * Change team scores.
 */
void GameController::change_team_scores(int bluescore, int redscore) {
	m_text_manager->set_active_font(m_menu_font);
	m_text_manager->set_active_color(1.0, 1.0, 1.0);
	
	if (redscore != -1) {
		if (m_overlay_items.count("red score") != 0) {
			m_text_manager->remove_string(m_overlay_items["red score"]);
		}
		
		stringstream redscoreprinter;
		redscoreprinter << redscore;
	 	m_overlay_items["red score"] = m_text_manager->place_string(redscoreprinter.str(), m_overlay_items["red label"]->get_x() + m_overlay_items["red label"]->get_image_width() + 5, 115, TextManager::LEFT, TextManager::LAYER_HUD);
	
		m_overlay_items["red score"]->set_priority(-4);
		m_window->unregister_graphic(m_overlay_items["red score"]);
		m_window->register_hud_graphic(m_overlay_items["red score"]);
	}
	
	if (bluescore != -1) {
		if (m_overlay_items.count("blue score") != 0) {
			m_text_manager->remove_string(m_overlay_items["blue score"]);
		}
		
		stringstream bluescoreprinter;
		bluescoreprinter << bluescore;
		m_overlay_items["blue score"] = m_text_manager->place_string(bluescoreprinter.str(), m_overlay_items["blue label"]->get_x() + m_overlay_items["blue label"]->get_image_width() + 5, 115, TextManager::LEFT, TextManager::LAYER_HUD);
	
		m_overlay_items["blue score"]->set_priority(-4);
		m_window->unregister_graphic(m_overlay_items["blue score"]);
		m_window->register_hud_graphic(m_overlay_items["blue score"]);
	}

	m_text_manager->set_active_font(m_font);
}

/*
 * Update individual scores.
 */
void GameController::update_individual_scores() {
	m_text_manager->set_active_font(m_medium_font);
	m_text_manager->set_active_color(1.0, 1.0, 1.0);
	
	int count = 0;
	
	map<int, GraphicalPlayer>::iterator it;
	for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
		GraphicalPlayer currplayer = (*it).second;
		if (currplayer.get_team() == 'A') {
			if (currplayer.get_sprite() == NULL) {
				continue;
			}
			string playername = currplayer.get_name();
			playername.append(": ");
			string playernameforscore = currplayer.get_name();
			string playerscore = playernameforscore.append("score");
		
			m_text_manager->set_active_color(BLUE_COLOR);
		
			if (m_overlay_items.count(playerscore) != 0) {
				m_text_manager->remove_string(m_overlay_items[playerscore]);
			}
			if (m_overlay_items.count(playername) != 0) {
				m_text_manager->remove_string(m_overlay_items[playername]);
			}
		
			stringstream scoreprinter;
			scoreprinter << currplayer.get_score();
		 	m_overlay_items[playername] = m_text_manager->place_string(playername, m_overlay_background->get_x() - m_overlay_background->get_image_width()/2 + 5, 230 + count*25, TextManager::LEFT, TextManager::LAYER_HUD);
			m_overlay_items[playerscore] = m_text_manager->place_string(scoreprinter.str(), m_overlay_background->get_x(), 230 + count*25, TextManager::LEFT, TextManager::LAYER_HUD);
		
			m_overlay_items[playername]->set_priority(-4);
			m_window->unregister_graphic(m_overlay_items[playername]);
			m_window->register_hud_graphic(m_overlay_items[playername]);
		
			m_overlay_items[playerscore]->set_priority(-4);
			m_window->unregister_graphic(m_overlay_items[playerscore]);
			m_window->register_hud_graphic(m_overlay_items[playerscore]);
		
			count++;
		}
	}
	
	count += 2;
	
	for ( it=m_players.begin() ; it != m_players.end(); it++ ) {
		GraphicalPlayer currplayer = (*it).second;
		if (currplayer.get_team() == 'B') {
			if (currplayer.get_sprite() == NULL) {
				continue;
			}
			string playername = currplayer.get_name();
			playername.append(": ");
			string playernameforscore = currplayer.get_name();
			string playerscore = playernameforscore.append("score");
		
			m_text_manager->set_active_color(RED_COLOR);
					
			if (m_overlay_items.count(playerscore) != 0) {
				m_text_manager->remove_string(m_overlay_items[playerscore]);
			}
			if (m_overlay_items.count(playername) != 0) {
				m_text_manager->remove_string(m_overlay_items[playername]);
			}
		
			stringstream scoreprinter;
			scoreprinter << currplayer.get_score();
		 	m_overlay_items[playername] = m_text_manager->place_string(playername, m_overlay_background->get_x() - m_overlay_background->get_image_width()/2 + 5, 230 + count*25, TextManager::LEFT, TextManager::LAYER_HUD);
			m_overlay_items[playerscore] = m_text_manager->place_string(scoreprinter.str(), m_overlay_background->get_x(), 230 + count*25, TextManager::LEFT, TextManager::LAYER_HUD);
		
			m_overlay_items[playername]->set_priority(-4);
			m_window->unregister_graphic(m_overlay_items[playername]);
			m_window->register_hud_graphic(m_overlay_items[playername]);
		
			m_overlay_items[playerscore]->set_priority(-4);
			m_window->unregister_graphic(m_overlay_items[playerscore]);
			m_window->register_hud_graphic(m_overlay_items[playerscore]);
		
			count++;
		}
	}

	m_text_manager->set_active_font(m_font);
}

/*
 * Send a player shot packet.
 */
void GameController::send_player_shot(unsigned int shooter_id, unsigned int hit_player_id, double angle) {
	PacketWriter player_shot(PLAYER_SHOT_PACKET);
	player_shot << shooter_id;
	player_shot << hit_player_id;
	player_shot << angle;
	
	m_network.send_packet(player_shot);
}

/*
 * Try to connect to a server.
 */
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

/*
 * Send a disconnect packet.
 */
void GameController::disconnect() {
	PacketWriter leave_request(LEAVE_PACKET);
	leave_request << m_player_id;
	
	m_network.send_packet(leave_request);
	
	m_network.disconnect();
}

/*
 * When we receive a welcome packet.
 */
void GameController::welcome(PacketReader& reader) {
	string serverversion;
	int playerid;
	string playername;
	char team;
	
	reader >> serverversion >> playerid >> playername >> team;
	
	m_player_id = playerid;
	m_name = playername;

	cerr << "Received welcome packet. Version: " << serverversion << ", Player ID: " << playerid << ", Name: " << playername << ", Team: " << team << endl;
	send_ack(reader);
	
	m_players.clear();
	
	// Insert different name colors and sprites depending on team.
	if (team == 'A') {
		m_players.insert(pair<int, GraphicalPlayer>(m_player_id,GraphicalPlayer(m_name.c_str(), m_player_id, team, new GraphicGroup(blue_player), blue_sprite->get_width()/2, blue_sprite->get_height()/2)));
		m_text_manager->set_active_color(BLUE_COLOR);
		m_window->register_graphic(&blue_player);
	} else {
		m_players.insert(pair<int, GraphicalPlayer>(m_player_id,GraphicalPlayer(m_name.c_str(), m_player_id, team, new GraphicGroup(red_player), red_sprite->get_width()/2, red_sprite->get_height()/2)));
		m_text_manager->set_active_color(RED_COLOR);
		m_window->register_graphic(&red_player);
	}
	m_window->register_graphic(m_players[m_player_id].get_sprite());
	m_minimap->add_blip(m_player_id, team, 0, 0);
	
	m_players[m_player_id].set_radius(30);
	m_players[m_player_id].set_name_sprite(m_text_manager->place_string(m_players[m_player_id].get_name(), m_screen_width/2, (m_screen_height/2)-(m_players[m_player_id].get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
	
	send_my_player_update();
}

/*
 * Add a player when we get an announce packet.
 */
void GameController::announce(PacketReader& reader) {
	unsigned int playerid;
	string playername;
	char team;
	
	if (m_players.empty()) {
		// WELCOME packet not received yet
		// do NOT send an ACK for this ANNOUNCE packet, so that the server will resend it, hopefully after the WELCOME has come in.
		return;
	}

	send_ack(reader);
	
	reader >> playerid >> playername >> team;
	
	string joinmsg = "";
	joinmsg.append(playername);
	joinmsg.append(" has joined the game!");
	display_message(joinmsg, team == 'A' ? BLUE_COLOR : RED_COLOR);
	
	// Ignore announce packet for ourself, but still display join message (above)
	if (playerid == m_player_id) {
		return;
	}

	// Add a different sprite and name color depending on team.
	if (team == 'A') {
		m_players.insert(pair<int, GraphicalPlayer>(playerid,GraphicalPlayer((const char*)playername.c_str(), playerid, team, new GraphicGroup(blue_player))));
		m_text_manager->set_active_color(BLUE_COLOR);
	} else {
		m_players.insert(pair<int, GraphicalPlayer>(playerid,GraphicalPlayer((const char*)playername.c_str(), playerid, team, new GraphicGroup(red_player))));
		m_text_manager->set_active_color(RED_COLOR);
	}
	m_minimap->add_blip(playerid,team,0,0);
	
	// Register the player sprite with the window
	m_window->register_graphic(m_players[playerid].get_sprite());
	m_players[playerid].set_name_sprite(m_text_manager->place_string(m_players[playerid].get_name(), m_players[playerid].get_x(), m_players[playerid].get_y()-(m_players[playerid].get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
	m_players[playerid].set_radius(40);
}

/*
 * When we receive a player update.
 */
void GameController::player_update(PacketReader& reader) {
	if (m_players.empty()) {
		return;
	}

	unsigned int player_id;
	long x;
	long y;
	double velocity_x;
	double velocity_y;
	double rotation;
	string flags;
	reader >> player_id >> x >> y >> velocity_x >> velocity_y >> rotation >> flags;

	if (player_id == m_player_id) {
		// If the player update packet is for this player, send an ACK for it
		send_ack(reader);
	}
	
	GraphicalPlayer* currplayer = get_player_by_id(player_id);
	if (currplayer == NULL) {
		cerr << "Error: Received update packet for non-existent player " << player_id << endl;
		return;
	}
	
	currplayer->set_position(x, y);
	m_minimap->move_blip(player_id, x, y);
	currplayer->set_velocity(velocity_x, velocity_y);
	currplayer->set_rotation_degrees(rotation);
	
	// If invisible or frozen, set these things appropriately and show/hide the sprite.
	if (flags.find_first_of('I') == string::npos) {
		currplayer->set_is_invisible(false);
		m_text_manager->reposition_string(m_players[player_id].get_name_sprite(), x, y - (m_players[player_id].get_radius()+30), TextManager::CENTER);
		m_players[player_id].get_name_sprite()->set_invisible(false);
		m_minimap->set_blip_invisible(player_id,false);
	} else {
		currplayer->set_is_invisible(true);
		m_players[player_id].get_name_sprite()->set_invisible(true);
		m_minimap->set_blip_invisible(player_id,true);
	}
	
	if (flags.find_first_of('F') == string::npos) {
		currplayer->set_is_frozen(false);
	} else {
		currplayer->set_is_frozen(true);
	}
	
}

/*
 * Send a player update packet.
 */
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
		<< my_player->get_y_vel() << my_player->get_rotation_degrees() << flags;
		
	m_network.send_packet(player_update);
}

/*
 * Send a message packet.
 */
void GameController::send_message(string message) {

	string::size_type		colon_pos = message.find_first_of(':');
	if (colon_pos != string::npos) {
		// Message to individual player
		string			recipient_name = message.substr(0, colon_pos);
		if (const GraphicalPlayer* player = get_player_by_name(recipient_name.c_str())) {
			PacketWriter	message_writer(MESSAGE_PACKET);
			message_writer << m_player_id << player->get_id() << message.substr(colon_pos + 1);
			m_network.send_packet(message_writer);
			return;
		}
	}

	// Broadcast message to all players
	PacketWriter	message_writer(MESSAGE_PACKET);
	message_writer << m_player_id << "" << message;
	m_network.send_packet(message_writer);
}

/*
 * Send a team message packet.
 */
void GameController::send_team_message(string message) {
	PacketWriter	message_writer(MESSAGE_PACKET);
	message_writer << m_player_id << m_players[m_player_id].get_team() << message;
	m_network.send_packet(message_writer);
}
 

/*
 * Deal with receiving a leave packet.
 */
void GameController::leave(PacketReader& reader) {
	uint32_t	playerid;
	string		leave_message;
	reader >> playerid >> leave_message;
	
	 // If it's for ourselves, we were kicked. Quit with a message.
	if (playerid == m_player_id) {
		string leavemsg = "You were kicked!  Reason: ";
		leavemsg.append(leave_message);
		display_message(leavemsg);
		cerr << "You were kicked!  Reason: " << leave_message << endl;
		m_quit_game = true;
		return;
	}
	
	if (m_players.empty()) {
		return;
	}
	
	string leavemsg = "";
	leavemsg.append(m_players[playerid].get_name());
	leavemsg.append(" has left the game.");
	
	// Display a message based on their team.
	if (m_players[playerid].get_team() == 'A') {
		display_message(leavemsg, BLUE_COLOR);
	} else {
		display_message(leavemsg, RED_COLOR);
	}
	
	m_text_manager->remove_string(m_players[playerid].get_name_sprite());
	m_window->unregister_graphic(m_players[playerid].get_sprite());
	m_minimap->remove_blip(playerid);
	delete m_players[playerid].get_sprite();
	m_players.erase(playerid);
}

/*
 * Called when a gun fired packet is received.
 */
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
	
	// Show a graphic for the shot.
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

/*
 * Called when a player shot packet is received.
 */
void GameController::player_shot(PacketReader& reader) {
	unsigned int shooter_id;
	unsigned int shot_id;
	unsigned long time_to_unfreeze;
	double shot_angle;
	
	reader >> shooter_id >> shot_id >> time_to_unfreeze >> shot_angle;
	
	// If we were frozen, add to our velocity based on the shot, and freeze.
	if (shot_id == m_player_id) {
		m_sound_controller->play_sound("freeze");
		m_players[m_player_id].set_is_frozen(true);
		m_time_to_unfreeze = SDL_GetTicks() + time_to_unfreeze;
		if (shot_angle != 0) {
			m_players[m_player_id].set_velocity(m_players[m_player_id].get_x_vel() - FIRING_RECOIL * cos((shot_angle) * DEGREES_TO_RADIANS), m_players[m_player_id].get_y_vel() - FIRING_RECOIL * sin((shot_angle) * DEGREES_TO_RADIANS));
		}
	}
}

/*
 * Called when a message is receieved.
 */
void GameController::message(PacketReader& reader) {
	uint32_t sender_id;
	string recipient;
	string message_text;
	
	reader >> sender_id >> recipient >> message_text;

	if (sender_id == 0) {
		// sender_id = 0 ==> From the server
		string message("[Server]: ");
		message.append(message_text);

		display_message(message);

	} else if (const GraphicalPlayer* sender = get_player_by_id(sender_id)) {
		string message(sender->get_name());
		message.append(": ");
		if (is_valid_team(recipient[0])) {
			// Team chat
			message.append("[TEAM]: ");
		}
		message.append(message_text);
		
		// Show the message in a color depending on the sender's team.
		if (sender->get_team() == 'A') {
			display_message(message, BLUE_COLOR);
		} else {
			display_message(message, RED_COLOR);
		}
	}
}

/*
 * Called when a gate update packet is received.
 */
void GameController::gate_update(PacketReader& reader) {
	uint32_t	lowering_player_id; 	// Who's lowering the gate?
	char		team;			// Which team's gate is being lowered
	double		progress;		// How much has the gate gone down? 0 == not at all .. 1 == all the way
	int		change_in_status;	// -1 == now closing, 0 == no change, 1 == now opening
	reader >> lowering_player_id >> team >> progress >> change_in_status;
	
	// If it just started opening, play a sound and set the warning visible.
	if (change_in_status > 0) {
		m_sound_controller->play_sound("gatelower");
		if (team == m_players[m_player_id].get_team()) {
			m_gate_warning->set_invisible(false);
			m_gate_warning_time = SDL_GetTicks();
		}
	}
	
	m_map->set_gate_progress(team, progress);

	// TODO: use the player id to display a HUD message or something...
}

/*
 * When a game start packet is received.
 */
void GameController::game_start(PacketReader& reader) {
	if (m_players.empty()) {
		// WELCOME packet not received yet
		// do NOT send an ACK for this GAME_START packet, so that the server will resend it, hopefully after the WELCOME has come in.
		return;
	}

	string 		mapname;
	bool		game_started;
	uint32_t	timeleft;
	reader >> mapname >> game_started >> timeleft;

	send_ack(reader);

	// Load the map
	string		mapfilename(mapname + ".map");

	if (mapname.find_first_of('/') != string::npos || !m_map->load_file(m_path_manager->data_path(mapfilename.c_str(), "maps"))) {
		string	message("You do not have the current map installed: ");
		message += mapname;
		display_message(message.c_str());
	}

	m_map_width = m_map->get_width();
	m_map_height = m_map->get_height();
	m_map_polygon.make_rectangle(m_map_width, m_map_height);

	// Tell the player what's going on
	ostringstream	message;
	if (game_started) {
		if (m_game_state == GAME_OVER) {
			m_game_state = GAME_IN_PROGRESS;
		}
		message << "Game started!";
		if (timeleft > 0) {
			message << " Time until spawn: " << timeleft;
		}
		m_sound_controller->play_sound("begin");
			
		toggle_score_overlay(false);
	} else {
		message << "Game starts in " << timeleft;
	}
	
	display_message(message.str().c_str());
}

/*
 * Called when the game stop packet is received.
 */
void GameController::game_stop(PacketReader& reader) {
	char		winningteam;
	int 		teamascore;
	int		teambscore;
	
	reader >> winningteam >> teamascore >> teambscore;

	send_ack(reader);
	
	m_game_state = GAME_OVER;
	
	if (winningteam == '-') {
		display_message("DRAW");
	} else if (winningteam == m_players[m_player_id].get_team()) {
		display_message("VICTORY!");
		m_sound_controller->play_sound("victory");
	} else {
		display_message("DEFEAT!");
		m_sound_controller->play_sound("defeat");
	}
	
	change_team_scores(teamascore, teambscore);
	toggle_score_overlay(true);

	// Temporary score display
	ostringstream	score_msg;
	score_msg << "Blue: " << teamascore << " / Red: " << teambscore;
	display_message(score_msg.str().c_str());
	// End temporary score display

	// Reset the gates and set yourself invisible and frozen until respawn.
	m_map->reset_gates();
	m_players[m_player_id].set_is_invisible(true);
	m_minimap->set_blip_invisible(m_player_id,true);
	m_players[m_player_id].set_is_frozen(true);
}

/*
 * Called when a score update packet is received.
 */
void GameController::score_update(PacketReader& reader) {
	if (m_players.empty()) {
		// WELCOME packet not received yet
		// do NOT send an ACK for this SCORE_UPDATE packet, so that the server will resend it, hopefully after the WELCOME has come in.
		return;
	}

	std::string	subject;
	int		score;
	reader >> subject >> score;

	send_ack(reader);

	if (is_valid_team(subject[0])) {
		char	team = subject[0];
		if (team == 'A') {
			change_team_scores(score, -1);
		} else {
			change_team_scores(-1, score);
		}
		toggle_score_overlay(!m_overlay_background->is_invisible());
	} else if (GraphicalPlayer* player = get_player_by_id(atoi(subject.c_str()))) {
		player->set_score(score);
	}
}

/*
 * Called when an animation packet is received.
 */
void GameController::animation_packet(PacketReader& reader) {
	uint32_t	player_id;
	string		spritelist;
	string		field;
	int		value;
	
	reader >> player_id >> spritelist >> field >> value;
	
	StringTokenizer tokenizer(spritelist, '/');
	
	if (m_players.count(player_id) == 0) {
		return;
	}
	
	// Get the sprite to modify by going through the path separated by slashes.
	Graphic* the_sprite = NULL;
	while (tokenizer.has_more()) {
		string spritename = tokenizer.get_next();
	
		if (spritename == "all") {
			if (the_sprite == NULL) {
				the_sprite = m_players[player_id].get_sprite();
			}
		} else {
			if (the_sprite == NULL) {
				the_sprite = m_players[player_id].get_sprite()->get_graphic(spritename);
			} else {
				the_sprite = the_sprite->get_graphic(spritename);
			}
		}
	
		if (the_sprite == NULL) {
			return;
		}
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
	} else if (field == "invisible") {
		the_sprite->set_invisible(value);
	}
}

/*
 * Called when a request denied packet is received.
 */
void GameController::request_denied(PacketReader& reader) {
	int		packet_type;
	string		reason;
	reader >> packet_type >> reason;

	if (packet_type == JOIN_PACKET) {
		string message = "Join denied! Reason: ";
		message.append(reason);
		display_message(message);
		cerr << "Join denied!  Reason: " << reason << endl;
		m_quit_game = true;
	}
}

/*
 * Called when a player name change packet is received.
 */
void GameController::name_change(PacketReader& reader) {
	uint32_t	player_id;
	string		new_name;
	reader >> player_id >> new_name;

	if (GraphicalPlayer* player = get_player_by_id(player_id)) {
		ostringstream	msg;
		msg << player->get_name() << " is now known as " << new_name;

		player->set_name(new_name.c_str());
		if (player_id == m_player_id) {
			m_name = new_name;
		}

		// Re-create the name sprite.
		if (player->get_team() == 'A') {
			display_message(msg.str().c_str(), BLUE_COLOR);
			m_text_manager->set_active_color(BLUE_COLOR);
		} else {
			display_message(msg.str().c_str(), RED_COLOR);
			m_text_manager->set_active_color(RED_COLOR);
		}
		m_text_manager->remove_string(player->get_name_sprite());
		player->set_name_sprite(m_text_manager->place_string(player->get_name(), player->get_x(), player->get_y()-(player->get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
	}
}

/*
 * Called when a team change packet is received.
 */
void GameController::team_change(PacketReader& reader) {
	uint32_t	playerid;
	char		team;
	reader >> playerid >> team;

	if (GraphicalPlayer* player = get_player_by_id(playerid)) {
		player->set_team(team);

		ostringstream	msg;
		msg << player->get_name() << " has switched teams";
		
		// Remove the name and sprite.
		m_text_manager->remove_string(m_players[playerid].get_name_sprite());
		m_window->unregister_graphic(m_players[playerid].get_sprite());
		m_minimap->remove_blip(playerid);
		delete m_players[playerid].get_sprite();

		// Generate new graphics for it.
		if (team == 'A') {
			player->set_sprite(new GraphicGroup(blue_player));
			m_text_manager->set_active_color(BLUE_COLOR);
			display_message(msg.str().c_str(), BLUE_COLOR);
		} else {
			player->set_sprite(new GraphicGroup(red_player));
			m_text_manager->set_active_color(RED_COLOR);
			display_message(msg.str().c_str(), RED_COLOR);
		}
		m_minimap->add_blip(playerid,team,0,0);
		
		m_window->register_graphic(m_players[playerid].get_sprite());
		m_players[playerid].set_name_sprite(m_text_manager->place_string(m_players[playerid].get_name(), m_players[playerid].get_x(), m_players[playerid].get_y()-(m_players[playerid].get_radius()+30), TextManager::CENTER, TextManager::LAYER_MAIN));
	}
}

/*
 * Send an animation packet.
 */
void GameController::send_animation_packet(string sprite, string field, int value) {
	PacketWriter animation_packet(PLAYER_ANIMATION_PACKET);
	animation_packet << m_player_id << sprite << field << value;
	
	m_network.send_packet(animation_packet);
}

/*
 * Send a gate hold packet.
 */
void GameController::send_gate_hold(bool holding) {
	PacketWriter gate_hold(GATE_UPDATE_PACKET);
	if (holding) {
		gate_hold << m_player_id << get_other_team(m_players[m_player_id].get_team()) << 1;
	} else {
		gate_hold << m_player_id << get_other_team(m_players[m_player_id].get_team()) << 0;
	}
	m_network.send_packet(gate_hold);
}

/*
 * Send a name change packet.
 */
void GameController::send_name_change_packet(const char* new_name) {
	PacketWriter packet(NAME_CHANGE_PACKET);
	packet << m_player_id << new_name;
	m_network.send_packet(packet);
}

/*
 * Send a team change packet.
 */
void GameController::send_team_change_packet(char new_team) {
	PacketWriter packet(TEAM_CHANGE_PACKET);
	packet << m_player_id << new_team;
	m_network.send_packet(packet);
}

/*
 * Display a message on the screen.
 */
void GameController::display_message(string message, Color color) {
	m_text_manager->set_active_color(color);
	int y = 20 + (m_font->ascent() + m_font->descent() + 5) * m_messages.size();
	Graphic* message_sprite = m_text_manager->place_string(message, 20, y, TextManager::LEFT, TextManager::LAYER_HUD);
	m_messages.push_back(pair<Graphic*, int>(message_sprite, SDL_GetTicks() + MESSAGE_DISPLAY_TIME));
}

/*
 * Get a player by their ID.
 */
GraphicalPlayer* GameController::get_player_by_id(unsigned int player_id) {
	map<int, GraphicalPlayer>::iterator it(m_players.find(player_id));
	return it == m_players.end() ? NULL : &it->second;
}

/*
 * Get a player by name.
 */
GraphicalPlayer* GameController::get_player_by_name(const char* name) {
	for (map<int, GraphicalPlayer>::iterator it(m_players.begin()); it != m_players.end(); ++it) {
		if (it->second.compare_name(name)) {
			return &it->second;
		}
	}
	return NULL;
}

/*
 * Send an ack packet.
 */
void	GameController::send_ack(const PacketReader& packet) {
	PacketWriter		ack_packet(ACK_PACKET);
	ack_packet << m_player_id << packet.packet_type() << packet.packet_id();
	m_network.send_packet(ack_packet);
}
