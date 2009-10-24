/*
 * common/PacketHeader.cpp
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

#include "PacketHeader.hpp"
#include "StringTokenizer.hpp"
#include "network.hpp"
#include <sstream>

using namespace LM;
using namespace std;

string	PacketHeader::make_string() const {
	ostringstream	str;
	str << packet_type << PACKET_FIELD_SEPARATOR << sequence_no;
	if (connection_id) {
		str << ':' << connection_id;
	}
	return str.str();
}
void	PacketHeader::read(StringTokenizer& tok) {
	string		packet_id_string;
	tok >> packet_type >> packet_id_string;
	StringTokenizer(packet_id_string, ':') >> sequence_no >> connection_id;
}
