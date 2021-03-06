/*
 * ai/main.cpp
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

#ifdef __APPLE__
extern "C" {
// FIXME port over mac_bridge
//#include "mac_bridge.h"
}
#elif defined(__WIN32)
#include <Windows.h>
#endif

#include "client/Client.hpp"
#include "common/network.hpp"
#include "common/Configuration.hpp"
#include "ai/ReactiveAIController.hpp"

using namespace LM;
using namespace std;

extern "C" int main(int argc, char* argv[]) {
	Client game;
	ReactiveAIController controller;
	Configuration config("ai.ini");
	game.set_controller(&controller);
	game.set_config(&config);
	IPAddress host;
	const char* server = config.get_string("Network", "server", "endrift.com:16876");
	resolve_hostname(host, server);
	game.connect(host);
	game.run();

	return 0;
}
