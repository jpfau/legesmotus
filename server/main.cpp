/*
 * Server/main.cpp
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

#include "Server.hpp"
#include "common/LMException.hpp"
#include "common/network.hpp"
#include "common/PathManager.hpp"
#include "common/misc.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

extern "C" void clean_exit() {
	// TODO write
}

static void display_usage(const char* progname) {
	cout << "Usage: " << progname << " [OPTION]" << endl;
	cout << "Options:" << endl;
	cout << "  -m MAPNAME	set the map name [default: alpha1]" << endl;
	cout << "  -P PASSWORD	set the admin password [default: DISABLED]" << endl;
	cout << "  -p PORTNO	set the port number to listen on [default: " << DEFAULT_PORTNO << "]" << endl;
	cout << "  -i PORTNO	address of interface to listen on [default: all interfaces]" << endl;
	cout << "  -d		daemonize the server (not on Windows)" << endl;
	cout << "  -l		(local server) do not register with the meta server" << endl;
	cout << "  -?, --help	display this help, and exit" << endl;
	cout << "      --version\tdisplay version information and exit" << endl;
}

static void display_version() {
	cout << "Leges Motus Server " << Server::SERVER_VERSION << endl;
	cout << "A networked, 2D shooter set in zero gravity" << endl;
	cout << endl;
	cout << "Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau" << endl;
	cout << "Leges Motus is free and open source software; see the source for copying conditions." << endl;
	cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
}

extern "C" int main(int argc, char* argv[]) try {
	srand(time(0));

	string			password;
	string			map_name("alpha1");
	const char*		interface_ip_address = NULL;
	unsigned int		portno = DEFAULT_PORTNO;
	bool			daemonize = false;
	bool			local_server = false;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0 && argc > i+1) {
			interface_ip_address = argv[i+1];
			++i;
		} else if (strcmp(argv[i], "-p") == 0 && argc > i+1) {
			portno = atoi(argv[i+1]);
			++i;
		} else if (strcmp(argv[i], "-P") == 0 && argc > i+1) {
			password = argv[i+1];
			++i;
		} else if (strcmp(argv[i], "-m") == 0 && argc > i+1) {
			map_name = argv[i+1];
			++i;
		} else if (strcmp(argv[i], "-d") == 0) {
			daemonize = true;
		} else if (strcmp(argv[i], "-l") == 0) {
			local_server = true;
		} else if (strcmp(argv[i], "--version") == 0) {
			display_version();
			return 0;
		} else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-?") == 0) {
			display_usage(argv[0]);
			return 0;
		} else if (strncmp(argv[i], "-psn", 4) == 0) {
			//Ignore -psn for Macs
		} else {
			cerr << argv[0] << ": Unrecognized option `" << argv[i] << "'" << endl;
			display_usage(argv[0]);
			return 2;
		}
	}

	PathManager		path_manager(argv[0]);

	Server			server(path_manager);

	server.set_password(password.c_str());
	server.set_register_with_metaserver(!local_server);
	server.start(interface_ip_address, portno, map_name.c_str());

	if (daemonize) {
		::daemonize();
	}

	server.run();

	return 0;

} catch (const LMException& e) {
	cerr << "Error: " << e.what() << endl;
	cerr << "1. Make sure that you are running the server from the top-level source directory, OR that your $LM_DATA_DIR environment variable is set to the directory containing the game resources." << endl;
	cerr << "2. Make sure that you are not already running the server." << endl;
	return 1;
}
