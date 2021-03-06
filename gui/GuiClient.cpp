/*
 * gui/GuiClient.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#include "GuiClient.hpp"
#include "SDLInputDriver.hpp"
#include "SDLWindow.hpp"
#include "ResourceCache.hpp"
#include "GraphicalPlayer.hpp"
#include "HumanController.hpp"
#include "common/Weapon.hpp"
#include "common/timer.hpp"
#include "common/Configuration.hpp"
#include "common/file.hpp"
#include "Window.hpp"
#include "Bone.hpp"
#include "common/math.hpp"
#include "Font.hpp"
#include "Label.hpp"
#include "Hud.hpp"
#include "ParticleManager.hpp"
#include "SimpleLineEmitter.hpp"

using namespace LM;
using namespace std;

GuiClient::GuiClient() {
	// TODO move elsewhere
	m_config = new Configuration("gui.ini");
	set_config(m_config);
	int width = m_config->get_int("GameWindow", "width", 800);
	int height = m_config->get_int("GameWindow", "height", 600);
	int depth = m_config->get_int("GameWindow", "depth", 24);
	int flags = 0;
	flags |= m_config->get_bool("GameWindow", "vsync", true)?Window::FLAG_VSYNC:0;
	flags |= m_config->get_bool("GameWindow", "fullscreen")?Window::FLAG_FULLSCREEN:0;
	m_window = SDLWindow::get_instance(width, height, depth, flags);
	m_cache = new ResourceCache(resource_dir(), m_window->get_context());
	m_input = new SDLInputDriver;
	m_input->set_sink(this);
	m_gcontrol = new HumanController; // XXX we don't necessarily want one
	m_gcontrol->set_viewport_size(width, height);
	set_sink(m_gcontrol);
	set_controller(m_gcontrol);

	memset(m_fonts, 0, sizeof(m_fonts));

	preload();

	m_window->set_root_widget(&m_root);
	m_view = new GameView("gv", m_cache, width, height, 128, &m_root);
	m_view->set_scale_base(1024);

	m_debugdraw = new PhysicsDraw;
	if (m_config->get_bool("Debug", "physics_overlay")) {
		add_extra_draw(m_debugdraw);
	}
	
	m_particle_manager = new ParticleManager(&m_root, 50000, true);
	add_extra_draw(m_particle_manager);

	m_map = NULL;
	m_player = NULL;

	m_hud = new Hud(m_cache, &m_root);
	m_hud->set_width(width);
	m_hud->set_height(height);
	set_font(load_font("DustHomeMedium.ttf", 12, m_hud->get_shadow_kernel()), FONT_BADGE_SHADOW);
}

GuiClient::~GuiClient() {
	for (vector<string>::iterator iter = m_preloaded_images.begin(); iter != m_preloaded_images.end(); ++iter) {
		m_cache->decrement<Image>(*iter);
	}

	for (vector<string>::iterator iter = m_preloaded_fonts.begin(); iter != m_preloaded_fonts.end(); ++iter) {
		m_cache->decrement<Font>(*iter);
	}

	delete m_cache;
	delete m_input;
	delete m_debugdraw;
	delete m_particle_manager;
	delete m_view;
	delete m_config;
	
	m_graphical_weapons.clear();

	// Clean up associations to prevent deletion failures
	m_root.set_parent(NULL);

	// TODO destroy window
}

void GuiClient::preload() {
	preload_image("red_head.png");
	preload_image("red_torso.png");
	preload_image("red_frontarm.png");
	preload_image("red_backarm.png");
	preload_image("red_frontleg.png");
	preload_image("red_backleg.png");
	preload_image("blue_head.png");
	preload_image("blue_torso.png");
	preload_image("blue_frontarm.png");
	preload_image("blue_backarm.png");
	preload_image("blue_frontleg.png");
	preload_image("blue_backleg.png");
	preload_image("aim.png");
	preload_image("blue_particle.png");
	preload_image("red_particle.png");
	preload_image("large_blue_particle.png");
	preload_image("large_red_particle.png");
	preload_image("shot.png");
	preload_image("shot_red.png");

	// XXX un-hardcode
	set_font(load_font("DustHomeMedium.ttf", 12), FONT_BADGE);
}

void GuiClient::preload_image(const char* filename) {
	Image img(filename, m_cache, true);
	m_cache->increment<Image>(filename);
	m_preloaded_images.push_back(filename);
}

void GuiClient::set_font(Font* font, FontUse fontuse) {
	Font* oldfont = m_fonts[fontuse];
	if (font != NULL) {
		string newid = font->get_id();
		m_cache->increment<Font>(newid);
	}
	m_fonts[fontuse] = font;
	if (oldfont != NULL) {
		m_cache->decrement<Font>(oldfont->get_id());
	}
}

Font* GuiClient::get_font(FontUse font) {
	return m_fonts[font];
}

Font* GuiClient::load_font(const char* filename, int size, const ConvolveKernel* kernel) {
	Font* font = m_cache->load_font(filename, size, kernel);
	m_preloaded_fonts.push_back(font->get_id());
	return font;
}

void GuiClient::set_sink(InputSink* input_sink) {
	m_input_sink = input_sink;
}

void GuiClient::add_badge(Player* player) {
	Label* badge = new Label(player->get_name(), get_font(FONT_BADGE), &m_root);
	Label* shadow = new Label(player->get_name(), get_font(FONT_BADGE_SHADOW));
	m_badges[player->get_id()] = badge;
	badge->set_color(Hud::get_team_color(player->get_team(), Hud::COLOR_DARK));
	badge->set_align(Label::ALIGN_CENTER);
	shadow->set_color(Hud::get_team_color(player->get_team(), Hud::COLOR_SHADOW));
	shadow->set_blend(DrawContext::BLEND_SUBTRACT);
	badge->set_shadow(shadow);
}

void GuiClient::remove_badge(Player* player) {
	map<int, Label*>::iterator iter = m_badges.find(player->get_id());
	delete iter->second;
	m_badges.erase(iter);
}

void GuiClient::realign_badges() {
	for (map<int, Label*>::iterator iter = m_badges.begin(); iter != m_badges.end(); ++iter) {
		Player* player = get_game()->get_player(iter->first);
		if (player == NULL) {
			continue;
		}
		Point new_point = m_view->world_to_view(Point(player->get_x(), player->get_y()));
		iter->second->set_x(new_point.x);
		iter->second->set_y(new_point.y - 64.0f*m_view->get_scale());

		iter->second->set_drawable(!player->is_invisible());
	}
}

void GuiClient::add_player(Player* player) {
	Client::add_player(player);
	GraphicalPlayer *gp = static_cast<GraphicalPlayer*>(player);
	m_view->add_child(gp->get_graphic(), GameView::PLAYERS);
	add_badge(player);
}

void GuiClient::set_own_player(uint32_t id) {
	Client::set_own_player(id);
	m_player = static_cast<GraphicalPlayer*>(get_player(id));
	if (m_hud != NULL) {
		m_hud->set_team(m_player->get_team());

		if (round_in_progress()) {
			m_hud->set_player(m_player);
		}
	}
}

void GuiClient::remove_player(uint32_t id, const std::string& reason) {
	Player *p = get_player(id);
	if (p == NULL) {
		WARN("Can't remove a non-existent player");
		return;
	}

	GraphicalPlayer *gp = static_cast<GraphicalPlayer*>(p);
	m_view->remove_child(gp->get_graphic());
	remove_badge(p);

	if (m_player != NULL && m_player->get_id() == id) {
		m_player = NULL;
	}

	Client::remove_player(id, reason);
}

void GuiClient::set_map(Map* map) {
	Client::set_map(map);
	if (m_map != NULL) {
		m_view->remove_child(m_map->get_background());
		m_debugdraw->set_world(NULL);
	}
	if (map != NULL) {
		m_map = static_cast<GraphicalMap*>(map);
		m_view->add_child(m_map->get_background(), GameView::BACKGROUND);
		if (get_game() != NULL) {
			m_debugdraw->set_world(get_game()->get_world());
		}
	}
}

void GuiClient::round_init(Map* map) {
	// For now, do nothing.
}

void GuiClient::round_started() {
	if (m_hud != NULL && m_player != NULL) {
		m_hud->set_player(m_player);
	}
}

void GuiClient::round_cleanup() {
	for (map<int, Label*>::iterator iter = m_badges.begin(); iter != m_badges.end(); ++iter) {
		delete iter->second;
	}
	m_badges.clear();
	
	if (m_hud != NULL) {
		m_hud->set_player(NULL);
	}

	Client::round_cleanup();
}

GraphicalPlayer* GuiClient::make_player(const char* name, uint32_t id, char team) {
	return new GraphicalPlayer(name, id, team, m_cache);
}

GraphicalMap* GuiClient::make_map() {
	return new GraphicalMap(m_cache);
}

Weapon* GuiClient::make_weapon(uint32_t index, WeaponReader& weapon_data) {
	GraphicalWeapon* gw = new GraphicalWeapon(m_cache);
	Weapon* w = Weapon::new_weapon(weapon_data, gw);
	
	if (w == NULL) {
		delete gw;
	}
	
	while(index > m_graphical_weapons.size()) {
		m_graphical_weapons.push_back(NULL);
	}
	
	if (index == m_graphical_weapons.size()) {
		m_graphical_weapons.push_back(gw);
	} else {
		m_graphical_weapons.erase(m_graphical_weapons.begin() + index);
		m_graphical_weapons.insert(m_graphical_weapons.begin() + index, gw);
	}
	
	return w;
}

void GuiClient::name_change(Player* player, const std::string& new_name) {
	Client::name_change(player, new_name);
	Label* badge = m_badges[player->get_id()];
	badge->set_string(new_name);
}

void GuiClient::team_change(Player* player, char new_team) {
	Client::team_change(player, new_team);
	Label* badge = m_badges[player->get_id()];
	Label* shadow = badge->get_shadow();
	badge->set_color(Hud::get_team_color(player->get_team(), Hud::COLOR_DARK));
	shadow->set_color(Hud::get_team_color(player->get_team(), Hud::COLOR_SHADOW));
}

void GuiClient::set_param(const string& param_name, const string& param_value) {
	Client::set_param(param_name, param_value);
	
	if (param_name == "radar_mode") {
		Hud::RadarMode mode = (Hud::RadarMode) atoi(param_value.c_str());
		if (mode >= Hud::RADAR_MAX) {
			WARN("Unknown radar mode: " << mode);
		} else {
			m_hud->set_radar_mode(mode);
		}
	} else if (param_name == "radar_scale") {
		m_hud->set_radar_scale(m_view->get_scale() * atof(param_value.c_str()));
	} else if (param_name == "radar_blip_duration") {
		m_hud->set_radar_blip_duration(strtoull(param_value.c_str(), NULL, 10));
	}
}

void GuiClient::run() {
	INFO("Beginning running GuiClient...");
	set_running(true);
	// XXX testing code
	// XXX move crosshairs to HUD?
	Bone crosshair_bone;
	GraphicContainer aim(false, &m_root);
	Sprite crosshair(m_cache->get<Image>("aim.png"));
	crosshair.set_center_x(128);
	crosshair.set_center_y(128);
	crosshair.set_x(512);
	crosshair.get_bone()->set_parent(&crosshair_bone);
	aim.add_graphic(&crosshair);
	aim.set_x(m_window->get_width()/2);
	aim.set_y(m_window->get_height()/2);
	crosshair_bone.set_scale_x(m_view->get_scale()/4.0f);
	crosshair_bone.set_scale_y(m_view->get_scale()/4.0f);

	IPAddress host;
	const char* server = m_config->get_string("Debug", "autoconnect_server", "endrift.com:16876");
	resolve_hostname(host, server);
	INFO("Connecting to " << server);
	connect(host);
	// XXX end testing code

	uint64_t last_time = get_ticks();
	while (running()) {
		uint64_t current_time = get_ticks();
		uint64_t diff = current_time - last_time;

		m_input->update();

		// Fudge the current time so that the remaining time between steps is accounted for
		current_time -= step(diff);
		
		if (!running()) {
			break;
		}

		// XXX move to HUD?
		crosshair_bone.set_rotation(m_gcontrol->get_aim() * RADIANS_TO_DEGREES);

		update_gui();
		
		m_particle_manager->update(current_time - last_time);

		m_window->redraw();
		last_time = current_time;
	}

	round_cleanup();
}

void GuiClient::update_gui() {
	// Recenter player
	if (m_player != NULL) {
		m_view->set_offset_x(m_player->get_x() * m_view->get_scale());
		m_view->set_offset_y(m_player->get_y() * m_view->get_scale());
	}

	// Move badges
	realign_badges();

	m_hud->update(get_game());
}

void GuiClient::add_extra_draw(Widget* draw) {
	m_view->add_child(draw, GameView::OVERLAY);
}

GraphicalWeapon* GuiClient::get_weapon(uint32_t id) {
	if (id >= m_graphical_weapons.size()) {
		return NULL;
	}
	return m_graphical_weapons.at(id);
}

void GuiClient::key_pressed(const KeyEvent& event) {
	m_input_sink->key_pressed(event);

	if (event.type == KEY_ESCAPE) {
		disconnect();
	}
}

void GuiClient::mouse_moved(const MouseMotionEvent& event) {
	m_input_sink->mouse_moved(event);
}

void GuiClient::mouse_clicked(const MouseButtonEvent& event) {
	m_input_sink->mouse_clicked(event);
}

void GuiClient::system_event(const SystemEvent& event) {
	m_input_sink->system_event(event);

	if (event.type == SYSTEM_QUIT) {
		disconnect();
	}
}

void GuiClient::disconnect() {
	for (int i = 0; i < FONT_MAX; ++i) {
		set_font(NULL, (FontUse)i);
	}

	delete m_hud;
	m_hud = NULL;

	round_cleanup();

	Client::disconnect();
	
	INFO("Disconnected.");
}

void GuiClient::round_over(const Packet& p) {
	round_cleanup();
	
	Client::round_over(p);
}

void GuiClient::weapon_discharged(const Packet& p) {
	Client::weapon_discharged(p);

	Player* player = get_player(p.weapon_discharged.player_id);
	if (player == NULL) {
		WARN("Player " << p.weapon_discharged.player_id << " fired, but does not exist.");
		return;
	}
	
	GraphicalPlayer* g_player = static_cast<GraphicalPlayer*>(player);
	if (g_player == NULL) {
		WARN("Player " << p.weapon_discharged.player_id << " could not be converted to a graphical player.");
		return;
	}
	
	Graphic* weapon_graphic = g_player->get_curr_visible_weapon_graphic();
	if (weapon_graphic == NULL) {
		return;
	}
	
	add_weapon_fired_emitter(g_player->get_current_weapon_id(), g_player, player->get_x(), player->get_y(), p.weapon_discharged.end_x, p.weapon_discharged.end_y, p.weapon_discharged.direction);
}

Packet* GuiClient::attempt_firing() {
	Packet* packet = Client::attempt_firing();
	
	if (packet == NULL) {
		return packet;
	}
	
	if (m_player == NULL) {
		WARN("Attempted to fire without having a player.");
		return packet;
	}
	
	Graphic* weapon_graphic = m_player->get_curr_visible_weapon_graphic();
	if (weapon_graphic == NULL) {
		return packet;
	}
	
	add_weapon_fired_emitter(m_player->get_current_weapon_id(), m_player, m_player->get_x(), m_player->get_y(), packet->weapon_discharged.end_x, packet->weapon_discharged.end_y, packet->weapon_discharged.direction);
	
	return packet;
}

void GuiClient::add_weapon_fired_emitter(int weapon_id, GraphicalPlayer* player, float player_x, float player_y, float end_x, float end_y, float rotation_rads) {
	float start_x = player_x;
	float start_y = player_y;
	// Move the start position to be closer to the gun muzzle
	Point offset_to_shoulder(0, -16);
	offset_to_shoulder.rotate(player->get_rotation_radians());
	Point offset_to_gun(50, 0);
	offset_to_gun.rotate(player->get_rotation_radians() + player->get_gun_rotation_radians());
	start_x += offset_to_shoulder.x + offset_to_gun.x;
	start_y += offset_to_shoulder.y + offset_to_gun.y;

	GraphicalWeapon* weapon = get_weapon(weapon_id);
	if (weapon == NULL) {
		DEBUG("No weapon found for generating a particle emitter.");
		return;
	}
	weapon->generate_fired_emitter(m_particle_manager, m_cache, start_x, start_y, end_x, end_y, rotation_rads, player->get_team());
}
