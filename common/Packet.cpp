// Auto-generated by parse_idl.py

#include "Packet.hpp"
#include <cstring>

using namespace LM;
using namespace std;

static void marshal_ACK(PacketWriter& w, Packet* p) {
	w << p->ack.packet_type;
	w << p->ack.sequence_no;
}

static void unmarshal_ACK(PacketReader& r, Packet* p) {
	r >> p->ack.packet_type;
	r >> p->ack.sequence_no;
}

static void marshal_PLAYER_UPDATE(PacketWriter& w, Packet* p) {
	w << p->player_update.player_id;
	w << p->player_update.x;
	w << p->player_update.y;
	w << p->player_update.x_vel;
	w << p->player_update.y_vel;
	w << p->player_update.rotation;
	w << p->player_update.energy;
	w << p->player_update.gun_rotation;
	w << p->player_update.current_weapon_id;
	w << p->player_update.flags;
}

static void unmarshal_PLAYER_UPDATE(PacketReader& r, Packet* p) {
	r >> p->player_update.player_id;
	r >> p->player_update.x;
	r >> p->player_update.y;
	r >> p->player_update.x_vel;
	r >> p->player_update.y_vel;
	r >> p->player_update.rotation;
	r >> p->player_update.energy;
	r >> p->player_update.gun_rotation;
	r >> p->player_update.current_weapon_id;
	r >> p->player_update.flags;
}

static void marshal_WEAPON_DISCHARGED(PacketWriter& w, Packet* p) {
	w << p->weapon_discharged.player_id;
	w << p->weapon_discharged.weapon_id;
	w << p->weapon_discharged.direction;
	w << p->weapon_discharged.start_x;
	w << p->weapon_discharged.start_y;
	w << p->weapon_discharged.end_x;
	w << p->weapon_discharged.end_y;
}

static void unmarshal_WEAPON_DISCHARGED(PacketReader& r, Packet* p) {
	r >> p->weapon_discharged.player_id;
	r >> p->weapon_discharged.weapon_id;
	r >> p->weapon_discharged.direction;
	r >> p->weapon_discharged.start_x;
	r >> p->weapon_discharged.start_y;
	r >> p->weapon_discharged.end_x;
	r >> p->weapon_discharged.end_y;
}

static void marshal_PLAYER_HIT(PacketWriter& w, Packet* p) {
	w << p->player_hit.shooter_id;
	w << p->player_hit.weapon_id;
	w << p->player_hit.shot_player_id;
	w << p->player_hit.has_effect;
	w << p->player_hit.extradata;
}

static void unmarshal_PLAYER_HIT(PacketReader& r, Packet* p) {
	r >> p->player_hit.shooter_id;
	r >> p->player_hit.weapon_id;
	r >> p->player_hit.shot_player_id;
	r >> p->player_hit.has_effect;
	r >> p->player_hit.extradata;
}

static void marshal_MESSAGE(PacketWriter& w, Packet* p) {
	w << p->message.sender_id;
	w << p->message.recipient;
	w << p->message.message_text;
}

static void unmarshal_MESSAGE(PacketReader& r, Packet* p) {
	r >> p->message.sender_id;
	r >> p->message.recipient;
	r >> p->message.message_text;
}

static void marshal_NEW_ROUND(PacketWriter& w, Packet* p) {
	w << p->new_round.map_name;
	w << p->new_round.map_revision;
	w << p->new_round.map_width;
	w << p->new_round.map_height;
	w << p->new_round.game_started;
	w << p->new_round.time_until_start;
}

static void unmarshal_NEW_ROUND(PacketReader& r, Packet* p) {
	r >> p->new_round.map_name;
	r >> p->new_round.map_revision;
	r >> p->new_round.map_width;
	r >> p->new_round.map_height;
	r >> p->new_round.game_started;
	r >> p->new_round.time_until_start;
}

static void marshal_ROUND_OVER(PacketWriter& w, Packet* p) {
	w << p->round_over.winning_team;
	w << p->round_over.team_a_score;
	w << p->round_over.team_b_score;
}

static void unmarshal_ROUND_OVER(PacketReader& r, Packet* p) {
	r >> p->round_over.winning_team;
	r >> p->round_over.team_a_score;
	r >> p->round_over.team_b_score;
}

static void marshal_SCORE_UPDATE(PacketWriter& w, Packet* p) {
	w << p->score_update.subject;
	w << p->score_update.score;
}

static void unmarshal_SCORE_UPDATE(PacketReader& r, Packet* p) {
	r >> p->score_update.subject;
	r >> p->score_update.score;
}

static void marshal_WELCOME(PacketWriter& w, Packet* p) {
	w << p->welcome.server_version;
	w << p->welcome.player_id;
	w << p->welcome.player_name;
	w << p->welcome.team;
}

static void unmarshal_WELCOME(PacketReader& r, Packet* p) {
	r >> p->welcome.server_version;
	r >> p->welcome.player_id;
	r >> p->welcome.player_name;
	r >> p->welcome.team;
}

static void marshal_ANNOUNCE(PacketWriter& w, Packet* p) {
	w << p->announce.player_id;
	w << p->announce.player_name;
	w << p->announce.team;
}

static void unmarshal_ANNOUNCE(PacketReader& r, Packet* p) {
	r >> p->announce.player_id;
	r >> p->announce.player_name;
	r >> p->announce.team;
}

static void marshal_GATE_UPDATE(PacketWriter& w, Packet* p) {
	w << p->gate_update.acting_player_id;
	w << p->gate_update.team;
	w << p->gate_update.progress;
	w << p->gate_update.change_in_players;
	w << p->gate_update.new_nbr_players;
	w << p->gate_update.sequence_no;
}

static void unmarshal_GATE_UPDATE(PacketReader& r, Packet* p) {
	r >> p->gate_update.acting_player_id;
	r >> p->gate_update.team;
	r >> p->gate_update.progress;
	r >> p->gate_update.change_in_players;
	r >> p->gate_update.new_nbr_players;
	r >> p->gate_update.sequence_no;
}

static void marshal_JOIN(PacketWriter& w, Packet* p) {
	w << p->join.protocol_number;
	w << p->join.compat_version;
	w << p->join.name;
	w << p->join.team;
}

static void unmarshal_JOIN(PacketReader& r, Packet* p) {
	r >> p->join.protocol_number;
	r >> p->join.compat_version;
	r >> p->join.name;
	r >> p->join.team;
}

static void marshal_INFO_server(PacketWriter& w, Packet* p) {
	w << p->info_server.request_packet_id;
	w << p->info_server.scan_start_time;
	w << p->info_server.server_address;
	w << p->info_server.server_protocol_version;
	w << p->info_server.server_compat_version;
	w << p->info_server.current_map_name;
	w << p->info_server.team_count_a;
	w << p->info_server.team_count_b;
	w << p->info_server.max_players;
	w << p->info_server.uptime;
	w << p->info_server.time_left_in_game;
	w << p->info_server.server_name;
	w << p->info_server.server_location;
}

static void unmarshal_INFO_server(PacketReader& r, Packet* p) {
	r >> p->info_server.request_packet_id;
	r >> p->info_server.scan_start_time;
	r >> p->info_server.server_address;
	r >> p->info_server.server_protocol_version;
	r >> p->info_server.server_compat_version;
	r >> p->info_server.current_map_name;
	r >> p->info_server.team_count_a;
	r >> p->info_server.team_count_b;
	r >> p->info_server.max_players;
	r >> p->info_server.uptime;
	r >> p->info_server.time_left_in_game;
	r >> p->info_server.server_name;
	r >> p->info_server.server_location;
}

static void marshal_INFO_client(PacketWriter& w, Packet* p) {
	w << p->info_client.client_proto_version;
	w << p->info_client.scan_id;
	w << p->info_client.scan_start_time;
	w << p->info_client.client_version;
}

static void unmarshal_INFO_client(PacketReader& r, Packet* p) {
	r >> p->info_client.client_proto_version;
	r >> p->info_client.scan_id;
	r >> p->info_client.scan_start_time;
	r >> p->info_client.client_version;
}

static void marshal_LEAVE(PacketWriter& w, Packet* p) {
	w << p->leave.player_id;
	w << p->leave.message;
}

static void unmarshal_LEAVE(PacketReader& r, Packet* p) {
	r >> p->leave.player_id;
	r >> p->leave.message;
}

static void marshal_PLAYER_ANIMATION(PacketWriter& w, Packet* p) {
	w << p->player_animation.player_id;
	w << p->player_animation.sprite_list;
	w << p->player_animation.field;
	w << p->player_animation.value;
}

static void unmarshal_PLAYER_ANIMATION(PacketReader& r, Packet* p) {
	r >> p->player_animation.player_id;
	r >> p->player_animation.sprite_list;
	r >> p->player_animation.field;
	r >> p->player_animation.value;
}

static void marshal_REQUEST_DENIED(PacketWriter& w, Packet* p) {
	w << p->request_denied.packet_type;
	w << p->request_denied.message;
}

static void unmarshal_REQUEST_DENIED(PacketReader& r, Packet* p) {
	r >> p->request_denied.packet_type;
	r >> p->request_denied.message;
}

static void marshal_NAME_CHANGE(PacketWriter& w, Packet* p) {
	w << p->name_change.player_id;
	w << p->name_change.name;
}

static void unmarshal_NAME_CHANGE(PacketReader& r, Packet* p) {
	r >> p->name_change.player_id;
	r >> p->name_change.name;
}

static void marshal_TEAM_CHANGE(PacketWriter& w, Packet* p) {
	w << p->team_change.player_id;
	w << p->team_change.name;
}

static void unmarshal_TEAM_CHANGE(PacketReader& r, Packet* p) {
	r >> p->team_change.player_id;
	r >> p->team_change.name;
}

static void marshal_REGISTER_SERVER_server(PacketWriter& w, Packet* p) {
	w << p->register_server_server.server_protocol_version;
	w << p->register_server_server.server_version;
	w << p->register_server_server.server_listen_address;
}

static void unmarshal_REGISTER_SERVER_server(PacketReader& r, Packet* p) {
	r >> p->register_server_server.server_protocol_version;
	r >> p->register_server_server.server_version;
	r >> p->register_server_server.server_listen_address;
}

static void marshal_REGISTER_SERVER_metaserver(PacketWriter& w, Packet* p) {
	w << p->register_server_metaserver.metaserver_token;
	w << p->register_server_metaserver.metaserver_contact_frequency;
}

static void unmarshal_REGISTER_SERVER_metaserver(PacketReader& r, Packet* p) {
	r >> p->register_server_metaserver.metaserver_token;
	r >> p->register_server_metaserver.metaserver_contact_frequency;
}

static void marshal_UNREGISTER_SERVER(PacketWriter& w, Packet* p) {
	w << p->unregister_server.server_listen_address;
	w << p->unregister_server.metaserver_token;
}

static void unmarshal_UNREGISTER_SERVER(PacketReader& r, Packet* p) {
	r >> p->unregister_server.server_listen_address;
	r >> p->unregister_server.metaserver_token;
}

static void marshal_UPGRADE_AVAILABLE(PacketWriter& w, Packet* p) {
	w << p->upgrade_available.latest_version;
}

static void unmarshal_UPGRADE_AVAILABLE(PacketReader& r, Packet* p) {
	r >> p->upgrade_available.latest_version;
}

static void marshal_MAP_INFO(PacketWriter& w, Packet* p) {
	w << p->map_info.transmission_id;
	w << p->map_info.map;
	w << p->map_info.num_expected_objects;
}

static void unmarshal_MAP_INFO(PacketReader& r, Packet* p) {
	r >> p->map_info.transmission_id;
	r >> p->map_info.map;
	r >> p->map_info.num_expected_objects;
}

static void marshal_MAP_OBJECT(PacketWriter& w, Packet* p) {
	w << p->map_object.transmission_id;
}

static void unmarshal_MAP_OBJECT(PacketReader& r, Packet* p) {
	r >> p->map_object.transmission_id;
}

static void marshal_GAME_PARAM(PacketWriter& w, Packet* p) {
	w << p->game_param.param_name;
	w << p->game_param.param_value;
}

static void unmarshal_GAME_PARAM(PacketReader& r, Packet* p) {
	r >> p->game_param.param_name;
	r >> p->game_param.param_value;
}

static void marshal_HOLE_PUNCH(PacketWriter& w, Packet* p) {
	w << p->hole_punch.client_address;
	w << p->hole_punch.scan_id;
}

static void unmarshal_HOLE_PUNCH(PacketReader& r, Packet* p) {
	r >> p->hole_punch.client_address;
	r >> p->hole_punch.scan_id;
}

static void marshal_PLAYER_DIED(PacketWriter& w, Packet* p) {
	w << p->player_died.killed_player_id;
	w << p->player_died.killer_id;
	w << p->player_died.freeze_time;
	w << p->player_died.killer_type;
}

static void unmarshal_PLAYER_DIED(PacketReader& r, Packet* p) {
	r >> p->player_died.killed_player_id;
	r >> p->player_died.killer_id;
	r >> p->player_died.freeze_time;
	r >> p->player_died.killer_type;
}

static void marshal_WEAPON_INFO(PacketWriter& w, Packet* p) {
	w << p->weapon_info.index;
	w << p->weapon_info.weapon_data;
}

static void unmarshal_WEAPON_INFO(PacketReader& r, Packet* p) {
	r >> p->weapon_info.index;
	r >> p->weapon_info.weapon_data;
}

static void marshal_ROUND_START(PacketWriter& w, Packet* p) {
	w << p->round_start.time_left_in_round;
}

static void unmarshal_ROUND_START(PacketReader& r, Packet* p) {
	r >> p->round_start.time_left_in_round;
}

static void marshal_SPAWN(PacketWriter& w, Packet* p) {
	w << p->spawn.position;
	w << p->spawn.velocity;
	w << p->spawn.is_grabbing_obstacle;
	w << p->spawn.is_alive;
	w << p->spawn.freeze_time;
}

static void unmarshal_SPAWN(PacketReader& r, Packet* p) {
	r >> p->spawn.position;
	r >> p->spawn.velocity;
	r >> p->spawn.is_grabbing_obstacle;
	r >> p->spawn.is_alive;
	r >> p->spawn.freeze_time;
}

static void marshal_PLAYER_JUMPED(PacketWriter& w, Packet* p) {
	w << p->player_jumped.player_id;
	w << p->player_jumped.direction;
}

static void unmarshal_PLAYER_JUMPED(PacketReader& r, Packet* p) {
	r >> p->player_jumped.player_id;
	r >> p->player_jumped.direction;
}

Packet::Packet() {
	clear();
	type = (PacketEnum) 0;
}

Packet::Packet(PacketEnum type) {
	clear();
	this->type = type;
}

Packet::Packet(const Packet& other) {
	clear();
	raw = other.raw;
	header = other.header;
	type = other.type;
	switch(type) {
	case ACK_PACKET:
		ack.packet_type = other.ack.packet_type;
		ack.sequence_no = other.ack.sequence_no;
		break;

	case PLAYER_UPDATE_PACKET:
		player_update.player_id = other.player_update.player_id;
		player_update.x = other.player_update.x;
		player_update.y = other.player_update.y;
		player_update.x_vel = other.player_update.x_vel;
		player_update.y_vel = other.player_update.y_vel;
		player_update.rotation = other.player_update.rotation;
		player_update.energy = other.player_update.energy;
		player_update.gun_rotation = other.player_update.gun_rotation;
		player_update.current_weapon_id = other.player_update.current_weapon_id;
		player_update.flags = *other.player_update.flags;
		break;

	case WEAPON_DISCHARGED_PACKET:
		weapon_discharged.player_id = other.weapon_discharged.player_id;
		weapon_discharged.weapon_id = other.weapon_discharged.weapon_id;
		weapon_discharged.direction = other.weapon_discharged.direction;
		weapon_discharged.start_x = other.weapon_discharged.start_x;
		weapon_discharged.start_y = other.weapon_discharged.start_y;
		weapon_discharged.end_x = other.weapon_discharged.end_x;
		weapon_discharged.end_y = other.weapon_discharged.end_y;
		break;

	case PLAYER_HIT_PACKET:
		player_hit.shooter_id = other.player_hit.shooter_id;
		player_hit.weapon_id = other.player_hit.weapon_id;
		player_hit.shot_player_id = other.player_hit.shot_player_id;
		player_hit.has_effect = other.player_hit.has_effect;
		player_hit.extradata = *other.player_hit.extradata;
		break;

	case MESSAGE_PACKET:
		message.sender_id = other.message.sender_id;
		message.recipient = *other.message.recipient;
		message.message_text = *other.message.message_text;
		break;

	case NEW_ROUND_PACKET:
		new_round.map_name = *other.new_round.map_name;
		new_round.map_revision = other.new_round.map_revision;
		new_round.map_width = other.new_round.map_width;
		new_round.map_height = other.new_round.map_height;
		new_round.game_started = other.new_round.game_started;
		new_round.time_until_start = other.new_round.time_until_start;
		break;

	case ROUND_OVER_PACKET:
		round_over.winning_team = other.round_over.winning_team;
		round_over.team_a_score = other.round_over.team_a_score;
		round_over.team_b_score = other.round_over.team_b_score;
		break;

	case SCORE_UPDATE_PACKET:
		score_update.subject = *other.score_update.subject;
		score_update.score = other.score_update.score;
		break;

	case WELCOME_PACKET:
		welcome.server_version = other.welcome.server_version;
		welcome.player_id = other.welcome.player_id;
		welcome.player_name = *other.welcome.player_name;
		welcome.team = other.welcome.team;
		break;

	case ANNOUNCE_PACKET:
		announce.player_id = other.announce.player_id;
		announce.player_name = *other.announce.player_name;
		announce.team = other.announce.team;
		break;

	case GATE_UPDATE_PACKET:
		gate_update.acting_player_id = other.gate_update.acting_player_id;
		gate_update.team = other.gate_update.team;
		gate_update.progress = other.gate_update.progress;
		gate_update.change_in_players = other.gate_update.change_in_players;
		gate_update.new_nbr_players = other.gate_update.new_nbr_players;
		gate_update.sequence_no = other.gate_update.sequence_no;
		break;

	case JOIN_PACKET:
		join.protocol_number = other.join.protocol_number;
		join.compat_version = *other.join.compat_version;
		join.name = *other.join.name;
		join.team = other.join.team;
		break;

	case INFO_server_PACKET:
		info_server.request_packet_id = other.info_server.request_packet_id;
		info_server.scan_start_time = other.info_server.scan_start_time;
		info_server.server_address = *other.info_server.server_address;
		info_server.server_protocol_version = other.info_server.server_protocol_version;
		info_server.server_compat_version = *other.info_server.server_compat_version;
		info_server.current_map_name = *other.info_server.current_map_name;
		info_server.team_count_a = other.info_server.team_count_a;
		info_server.team_count_b = other.info_server.team_count_b;
		info_server.max_players = other.info_server.max_players;
		info_server.uptime = other.info_server.uptime;
		info_server.time_left_in_game = other.info_server.time_left_in_game;
		info_server.server_name = *other.info_server.server_name;
		info_server.server_location = *other.info_server.server_location;
		break;

	case INFO_client_PACKET:
		info_client.client_proto_version = other.info_client.client_proto_version;
		info_client.scan_id = other.info_client.scan_id;
		info_client.scan_start_time = other.info_client.scan_start_time;
		info_client.client_version = *other.info_client.client_version;
		break;

	case LEAVE_PACKET:
		leave.player_id = other.leave.player_id;
		leave.message = *other.leave.message;
		break;

	case PLAYER_ANIMATION_PACKET:
		player_animation.player_id = other.player_animation.player_id;
		player_animation.sprite_list = *other.player_animation.sprite_list;
		player_animation.field = *other.player_animation.field;
		player_animation.value = other.player_animation.value;
		break;

	case REQUEST_DENIED_PACKET:
		request_denied.packet_type = other.request_denied.packet_type;
		request_denied.message = *other.request_denied.message;
		break;

	case NAME_CHANGE_PACKET:
		name_change.player_id = other.name_change.player_id;
		name_change.name = *other.name_change.name;
		break;

	case TEAM_CHANGE_PACKET:
		team_change.player_id = other.team_change.player_id;
		team_change.name = other.team_change.name;
		break;

	case REGISTER_SERVER_server_PACKET:
		register_server_server.server_protocol_version = other.register_server_server.server_protocol_version;
		register_server_server.server_version = *other.register_server_server.server_version;
		register_server_server.server_listen_address = *other.register_server_server.server_listen_address;
		break;

	case REGISTER_SERVER_metaserver_PACKET:
		register_server_metaserver.metaserver_token = other.register_server_metaserver.metaserver_token;
		register_server_metaserver.metaserver_contact_frequency = other.register_server_metaserver.metaserver_contact_frequency;
		break;

	case UNREGISTER_SERVER_PACKET:
		unregister_server.server_listen_address = *other.unregister_server.server_listen_address;
		unregister_server.metaserver_token = other.unregister_server.metaserver_token;
		break;

	case UPGRADE_AVAILABLE_PACKET:
		upgrade_available.latest_version = *other.upgrade_available.latest_version;
		break;

	case MAP_INFO_PACKET:
		map_info.transmission_id = other.map_info.transmission_id;
		map_info.map = *other.map_info.map;
		map_info.num_expected_objects = other.map_info.num_expected_objects;
		break;

	case MAP_OBJECT_PACKET:
		map_object.transmission_id = other.map_object.transmission_id;
		break;

	case GAME_PARAM_PACKET:
		game_param.param_name = *other.game_param.param_name;
		game_param.param_value = *other.game_param.param_value;
		break;

	case HOLE_PUNCH_PACKET:
		hole_punch.client_address = *other.hole_punch.client_address;
		hole_punch.scan_id = other.hole_punch.scan_id;
		break;

	case PLAYER_DIED_PACKET:
		player_died.killed_player_id = other.player_died.killed_player_id;
		player_died.killer_id = other.player_died.killer_id;
		player_died.freeze_time = other.player_died.freeze_time;
		player_died.killer_type = other.player_died.killer_type;
		break;

	case WEAPON_INFO_PACKET:
		weapon_info.index = other.weapon_info.index;
		weapon_info.weapon_data = *other.weapon_info.weapon_data;
		break;

	case ROUND_START_PACKET:
		round_start.time_left_in_round = other.round_start.time_left_in_round;
		break;

	case SPAWN_PACKET:
		spawn.position = *other.spawn.position;
		spawn.velocity = *other.spawn.velocity;
		spawn.is_grabbing_obstacle = other.spawn.is_grabbing_obstacle;
		spawn.is_alive = other.spawn.is_alive;
		spawn.freeze_time = other.spawn.freeze_time;
		break;

	case PLAYER_JUMPED_PACKET:
		player_jumped.player_id = other.player_jumped.player_id;
		player_jumped.direction = other.player_jumped.direction;
		break;

	}
}

Packet::~Packet() {
	free();
}

void Packet::free() {
	switch(type) {
	case ACK_PACKET:
		break;

	case PLAYER_UPDATE_PACKET:
		delete player_update.flags.item;
		player_update.flags.item = NULL;
		break;

	case WEAPON_DISCHARGED_PACKET:
		break;

	case PLAYER_HIT_PACKET:
		delete player_hit.extradata.item;
		player_hit.extradata.item = NULL;
		break;

	case MESSAGE_PACKET:
		delete message.recipient.item;
		message.recipient.item = NULL;
		delete message.message_text.item;
		message.message_text.item = NULL;
		break;

	case NEW_ROUND_PACKET:
		delete new_round.map_name.item;
		new_round.map_name.item = NULL;
		break;

	case ROUND_OVER_PACKET:
		break;

	case SCORE_UPDATE_PACKET:
		delete score_update.subject.item;
		score_update.subject.item = NULL;
		break;

	case WELCOME_PACKET:
		delete welcome.player_name.item;
		welcome.player_name.item = NULL;
		break;

	case ANNOUNCE_PACKET:
		delete announce.player_name.item;
		announce.player_name.item = NULL;
		break;

	case GATE_UPDATE_PACKET:
		break;

	case JOIN_PACKET:
		delete join.compat_version.item;
		join.compat_version.item = NULL;
		delete join.name.item;
		join.name.item = NULL;
		break;

	case INFO_server_PACKET:
		delete info_server.server_address.item;
		info_server.server_address.item = NULL;
		delete info_server.server_compat_version.item;
		info_server.server_compat_version.item = NULL;
		delete info_server.current_map_name.item;
		info_server.current_map_name.item = NULL;
		delete info_server.server_name.item;
		info_server.server_name.item = NULL;
		delete info_server.server_location.item;
		info_server.server_location.item = NULL;
		break;

	case INFO_client_PACKET:
		delete info_client.client_version.item;
		info_client.client_version.item = NULL;
		break;

	case LEAVE_PACKET:
		delete leave.message.item;
		leave.message.item = NULL;
		break;

	case PLAYER_ANIMATION_PACKET:
		delete player_animation.sprite_list.item;
		player_animation.sprite_list.item = NULL;
		delete player_animation.field.item;
		player_animation.field.item = NULL;
		break;

	case REQUEST_DENIED_PACKET:
		delete request_denied.message.item;
		request_denied.message.item = NULL;
		break;

	case NAME_CHANGE_PACKET:
		delete name_change.name.item;
		name_change.name.item = NULL;
		break;

	case TEAM_CHANGE_PACKET:
		break;

	case REGISTER_SERVER_server_PACKET:
		delete register_server_server.server_version.item;
		register_server_server.server_version.item = NULL;
		delete register_server_server.server_listen_address.item;
		register_server_server.server_listen_address.item = NULL;
		break;

	case REGISTER_SERVER_metaserver_PACKET:
		break;

	case UNREGISTER_SERVER_PACKET:
		delete unregister_server.server_listen_address.item;
		unregister_server.server_listen_address.item = NULL;
		break;

	case UPGRADE_AVAILABLE_PACKET:
		delete upgrade_available.latest_version.item;
		upgrade_available.latest_version.item = NULL;
		break;

	case MAP_INFO_PACKET:
		delete map_info.map.item;
		map_info.map.item = NULL;
		break;

	case MAP_OBJECT_PACKET:
		break;

	case GAME_PARAM_PACKET:
		delete game_param.param_name.item;
		game_param.param_name.item = NULL;
		delete game_param.param_value.item;
		game_param.param_value.item = NULL;
		break;

	case HOLE_PUNCH_PACKET:
		delete hole_punch.client_address.item;
		hole_punch.client_address.item = NULL;
		break;

	case PLAYER_DIED_PACKET:
		break;

	case WEAPON_INFO_PACKET:
		delete weapon_info.weapon_data.item;
		weapon_info.weapon_data.item = NULL;
		break;

	case ROUND_START_PACKET:
		break;

	case SPAWN_PACKET:
		delete spawn.position.item;
		spawn.position.item = NULL;
		delete spawn.velocity.item;
		spawn.velocity.item = NULL;
		break;

	case PLAYER_JUMPED_PACKET:
		break;

	}
}

void Packet::clear() {
	void* base = &header;
	memset(base, 0, sizeof(*this) - (long(base) - long(this)));
}
void Packet::marshal() {
	PacketWriter w(type, header);
	switch(type) {
	case ACK_PACKET:
		marshal_ACK(w, this);
		break;

	case PLAYER_UPDATE_PACKET:
		marshal_PLAYER_UPDATE(w, this);
		break;

	case WEAPON_DISCHARGED_PACKET:
		marshal_WEAPON_DISCHARGED(w, this);
		break;

	case PLAYER_HIT_PACKET:
		marshal_PLAYER_HIT(w, this);
		break;

	case MESSAGE_PACKET:
		marshal_MESSAGE(w, this);
		break;

	case NEW_ROUND_PACKET:
		marshal_NEW_ROUND(w, this);
		break;

	case ROUND_OVER_PACKET:
		marshal_ROUND_OVER(w, this);
		break;

	case SCORE_UPDATE_PACKET:
		marshal_SCORE_UPDATE(w, this);
		break;

	case WELCOME_PACKET:
		marshal_WELCOME(w, this);
		break;

	case ANNOUNCE_PACKET:
		marshal_ANNOUNCE(w, this);
		break;

	case GATE_UPDATE_PACKET:
		marshal_GATE_UPDATE(w, this);
		break;

	case JOIN_PACKET:
		marshal_JOIN(w, this);
		break;

	case INFO_server_PACKET:
		marshal_INFO_server(w, this);
		break;

	case INFO_client_PACKET:
		marshal_INFO_client(w, this);
		break;

	case LEAVE_PACKET:
		marshal_LEAVE(w, this);
		break;

	case PLAYER_ANIMATION_PACKET:
		marshal_PLAYER_ANIMATION(w, this);
		break;

	case REQUEST_DENIED_PACKET:
		marshal_REQUEST_DENIED(w, this);
		break;

	case NAME_CHANGE_PACKET:
		marshal_NAME_CHANGE(w, this);
		break;

	case TEAM_CHANGE_PACKET:
		marshal_TEAM_CHANGE(w, this);
		break;

	case REGISTER_SERVER_server_PACKET:
		marshal_REGISTER_SERVER_server(w, this);
		break;

	case REGISTER_SERVER_metaserver_PACKET:
		marshal_REGISTER_SERVER_metaserver(w, this);
		break;

	case UNREGISTER_SERVER_PACKET:
		marshal_UNREGISTER_SERVER(w, this);
		break;

	case UPGRADE_AVAILABLE_PACKET:
		marshal_UPGRADE_AVAILABLE(w, this);
		break;

	case MAP_INFO_PACKET:
		marshal_MAP_INFO(w, this);
		break;

	case MAP_OBJECT_PACKET:
		marshal_MAP_OBJECT(w, this);
		break;

	case GAME_PARAM_PACKET:
		marshal_GAME_PARAM(w, this);
		break;

	case HOLE_PUNCH_PACKET:
		marshal_HOLE_PUNCH(w, this);
		break;

	case PLAYER_DIED_PACKET:
		marshal_PLAYER_DIED(w, this);
		break;

	case WEAPON_INFO_PACKET:
		marshal_WEAPON_INFO(w, this);
		break;

	case ROUND_START_PACKET:
		marshal_ROUND_START(w, this);
		break;

	case SPAWN_PACKET:
		marshal_SPAWN(w, this);
		break;

	case PLAYER_JUMPED_PACKET:
		marshal_PLAYER_JUMPED(w, this);
		break;

	default:
		break;
	}

	raw.fill(w.get_header().make_string());
	raw.append(w.packet_data());
}

void Packet::unmarshal() {
	PacketReader r(raw);
	type = (PacketEnum) r.packet_type();
	header = r.get_header();
	switch(type) {
	case ACK_PACKET:
		unmarshal_ACK(r, this);
		break;

	case PLAYER_UPDATE_PACKET:
		unmarshal_PLAYER_UPDATE(r, this);
		break;

	case WEAPON_DISCHARGED_PACKET:
		unmarshal_WEAPON_DISCHARGED(r, this);
		break;

	case PLAYER_HIT_PACKET:
		unmarshal_PLAYER_HIT(r, this);
		break;

	case MESSAGE_PACKET:
		unmarshal_MESSAGE(r, this);
		break;

	case NEW_ROUND_PACKET:
		unmarshal_NEW_ROUND(r, this);
		break;

	case ROUND_OVER_PACKET:
		unmarshal_ROUND_OVER(r, this);
		break;

	case SCORE_UPDATE_PACKET:
		unmarshal_SCORE_UPDATE(r, this);
		break;

	case WELCOME_PACKET:
		unmarshal_WELCOME(r, this);
		break;

	case ANNOUNCE_PACKET:
		unmarshal_ANNOUNCE(r, this);
		break;

	case GATE_UPDATE_PACKET:
		unmarshal_GATE_UPDATE(r, this);
		break;

	case JOIN_PACKET:
		unmarshal_JOIN(r, this);
		break;

	case INFO_server_PACKET:
		unmarshal_INFO_server(r, this);
		break;

	case INFO_client_PACKET:
		unmarshal_INFO_client(r, this);
		break;

	case LEAVE_PACKET:
		unmarshal_LEAVE(r, this);
		break;

	case PLAYER_ANIMATION_PACKET:
		unmarshal_PLAYER_ANIMATION(r, this);
		break;

	case REQUEST_DENIED_PACKET:
		unmarshal_REQUEST_DENIED(r, this);
		break;

	case NAME_CHANGE_PACKET:
		unmarshal_NAME_CHANGE(r, this);
		break;

	case TEAM_CHANGE_PACKET:
		unmarshal_TEAM_CHANGE(r, this);
		break;

	case REGISTER_SERVER_server_PACKET:
		unmarshal_REGISTER_SERVER_server(r, this);
		break;

	case REGISTER_SERVER_metaserver_PACKET:
		unmarshal_REGISTER_SERVER_metaserver(r, this);
		break;

	case UNREGISTER_SERVER_PACKET:
		unmarshal_UNREGISTER_SERVER(r, this);
		break;

	case UPGRADE_AVAILABLE_PACKET:
		unmarshal_UPGRADE_AVAILABLE(r, this);
		break;

	case MAP_INFO_PACKET:
		unmarshal_MAP_INFO(r, this);
		break;

	case MAP_OBJECT_PACKET:
		unmarshal_MAP_OBJECT(r, this);
		break;

	case GAME_PARAM_PACKET:
		unmarshal_GAME_PARAM(r, this);
		break;

	case HOLE_PUNCH_PACKET:
		unmarshal_HOLE_PUNCH(r, this);
		break;

	case PLAYER_DIED_PACKET:
		unmarshal_PLAYER_DIED(r, this);
		break;

	case WEAPON_INFO_PACKET:
		unmarshal_WEAPON_INFO(r, this);
		break;

	case ROUND_START_PACKET:
		unmarshal_ROUND_START(r, this);
		break;

	case SPAWN_PACKET:
		unmarshal_SPAWN(r, this);
		break;

	case PLAYER_JUMPED_PACKET:
		unmarshal_PLAYER_JUMPED(r, this);
		break;

	default:
		break;
	}
}
void Packet::dispatch(PacketReceiver* r) {
	switch(type) {
	case ACK_PACKET:
		r->ack(*this);
		break;

	case PLAYER_UPDATE_PACKET:
		r->player_update(*this);
		break;

	case WEAPON_DISCHARGED_PACKET:
		r->weapon_discharged(*this);
		break;

	case PLAYER_HIT_PACKET:
		r->player_hit(*this);
		break;

	case MESSAGE_PACKET:
		r->message(*this);
		break;

	case NEW_ROUND_PACKET:
		r->new_round(*this);
		break;

	case ROUND_OVER_PACKET:
		r->round_over(*this);
		break;

	case SCORE_UPDATE_PACKET:
		r->score_update(*this);
		break;

	case WELCOME_PACKET:
		r->welcome(*this);
		break;

	case ANNOUNCE_PACKET:
		r->announce(*this);
		break;

	case GATE_UPDATE_PACKET:
		r->gate_update(*this);
		break;

	case JOIN_PACKET:
		r->join(*this);
		break;

	case INFO_server_PACKET:
		r->info_server(*this);
		break;

	case INFO_client_PACKET:
		r->info_client(*this);
		break;

	case LEAVE_PACKET:
		r->leave(*this);
		break;

	case PLAYER_ANIMATION_PACKET:
		r->player_animation(*this);
		break;

	case REQUEST_DENIED_PACKET:
		r->request_denied(*this);
		break;

	case NAME_CHANGE_PACKET:
		r->name_change(*this);
		break;

	case TEAM_CHANGE_PACKET:
		r->team_change(*this);
		break;

	case REGISTER_SERVER_server_PACKET:
		r->register_server_server(*this);
		break;

	case REGISTER_SERVER_metaserver_PACKET:
		r->register_server_metaserver(*this);
		break;

	case UNREGISTER_SERVER_PACKET:
		r->unregister_server(*this);
		break;

	case UPGRADE_AVAILABLE_PACKET:
		r->upgrade_available(*this);
		break;

	case MAP_INFO_PACKET:
		r->map_info(*this);
		break;

	case MAP_OBJECT_PACKET:
		r->map_object(*this);
		break;

	case GAME_PARAM_PACKET:
		r->game_param(*this);
		break;

	case HOLE_PUNCH_PACKET:
		r->hole_punch(*this);
		break;

	case PLAYER_DIED_PACKET:
		r->player_died(*this);
		break;

	case WEAPON_INFO_PACKET:
		r->weapon_info(*this);
		break;

	case ROUND_START_PACKET:
		r->round_start(*this);
		break;

	case SPAWN_PACKET:
		r->spawn(*this);
		break;

	case PLAYER_JUMPED_PACKET:
		r->player_jumped(*this);
		break;

	default:
		break;
	}
}
