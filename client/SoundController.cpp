/*
 * SoundController.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "SoundController.hpp"
#include "GameController.hpp"
#include "SDL_mixer.h"
#include "common/PathManager.hpp"
#include <stdio.h>
#include <iostream>

using namespace LM;
using namespace std;

SoundController* SoundController::m_instance = NULL;

SoundController::SoundController(GameController& parent, PathManager& path_manager) : m_path_manager(path_manager), m_parent(parent) {
	m_sound_on = true;
	
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		cerr << "Error calling Mix_OpenAudio" << endl;
	}
	
	Mix_ChannelFinished(&channel_finished);

	m_gunshot_sound = Mix_LoadWAV(m_path_manager.data_path("LMGunshot.ogg", "sounds"));
	if(!m_gunshot_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_unfreeze_sound = Mix_LoadWAV(m_path_manager.data_path("enchant.ogg", "sounds"));
	if(!m_unfreeze_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_freeze_sound = Mix_LoadWAV(m_path_manager.data_path("disenchant.ogg", "sounds"));
	if(!m_freeze_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_gate_siren_sound = Mix_LoadWAV(m_path_manager.data_path("LMGateSiren.ogg", "sounds"));
	if(!m_gate_siren_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}
	
	m_positive_gate_siren_sound = Mix_LoadWAV(m_path_manager.data_path("positive_gate_siren.ogg", "sounds"));
	if(!m_positive_gate_siren_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}
	
	m_victory_sound = Mix_LoadWAV(m_path_manager.data_path("victory_fanfare.ogg", "sounds"));
	if(!m_victory_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_defeat_sound = Mix_LoadWAV(m_path_manager.data_path("defeatsound.ogg", "sounds"));
	if(!m_victory_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}

	m_begin_sound = Mix_LoadWAV(m_path_manager.data_path("clockchime1.ogg", "sounds"));
	if(!m_begin_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}
	
	m_click_sound = Mix_LoadWAV(m_path_manager.data_path("button_click.ogg", "sounds"));
	if(!m_click_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}
	
	m_hit_sound = Mix_LoadWAV(m_path_manager.data_path("laserhit.ogg", "sounds"));
	if(!m_hit_sound) {
		fprintf(stderr, "Mix_LoadWAV: %s\n", Mix_GetError());
	}
}

SoundController::~SoundController() {
	Mix_FreeChunk(m_gunshot_sound);
	Mix_FreeChunk(m_freeze_sound);
	Mix_FreeChunk(m_unfreeze_sound);
	Mix_FreeChunk(m_positive_gate_siren_sound);
	Mix_FreeChunk(m_gate_siren_sound);
	Mix_FreeChunk(m_victory_sound);
	Mix_FreeChunk(m_defeat_sound);
	Mix_FreeChunk(m_begin_sound);
	Mix_FreeChunk(m_click_sound);
	Mix_FreeChunk(m_hit_sound);
}

SoundController* SoundController::get_instance(GameController& parent, PathManager& path_manager) {
	if (m_instance == NULL) {
		m_instance = new SoundController(parent, path_manager);
	}
	return m_instance;
}

SoundController* SoundController::get_instance() {
	return m_instance;
}

void SoundController::destroy_instance() {
	delete m_instance;
	m_instance = NULL;
}

int SoundController::play_sound (string sound) {
	if (!m_sound_on) {
		return -1;
	}
	
	int result = -1;
	
	if(sound == "fire") {
		result = Mix_PlayChannel(-1, m_gunshot_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "freeze") {
		result = Mix_PlayChannel(-1, m_freeze_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "unfreeze") {
		result = Mix_PlayChannel(-1, m_unfreeze_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "gatelower") {
		result = Mix_PlayChannel(-1, m_gate_siren_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "positivegatelower") {
		result = Mix_PlayChannel(-1, m_positive_gate_siren_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "victory") {
		result = Mix_PlayChannel(-1, m_victory_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "begin") {
		result = Mix_PlayChannel(-1, m_begin_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "defeat") {
		result = Mix_PlayChannel(-1, m_defeat_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "click") {
		result = Mix_PlayChannel(-1, m_click_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	} else if(sound == "hit") {
		result = Mix_PlayChannel(-1, m_hit_sound, 0);
		if(result == -1) {
			fprintf(stderr, "Mix_PlayChannel: %s\n", Mix_GetError());
		}
	}
	
	return result;
}

void SoundController::halt_sound(int channel) {
	Mix_HaltChannel(channel);
}

void SoundController::set_sound_on(bool on) {
	m_sound_on = on;
}

void SoundController::channel_done(int channel) {
	m_parent.sound_finished(channel);
}

void SoundController::channel_finished(int channel) {
	m_instance->channel_done(channel);
}
