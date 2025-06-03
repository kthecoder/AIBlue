extends Node

var nav_agent = "res://scenes/Level/main.tscn"
var flow_field = "res://scenes/Level/flow_field.tscn"

signal player_connected(peer_id, player_info)
signal player_disconnected(peer_id)
signal server_disconnected
signal spawn_player(peer_id : int)
signal remove_player(peer_id : int)

const PORT = 7000
const DEFAULT_SERVER_IP = "127.0.0.1" # IPv4 localhost
const MAX_CONNECTIONS = 20
const MIN_PLAYER_COUNT = 2

# This will contain player info for every player
var players = {}

var gameSceneID : int = 0;

# This is the local player info.
var player_info = {
	"name": "No Name", # Max 16 Char
}

func _ready():	
	multiplayer.peer_connected.connect(_on_player_connected)
	multiplayer.peer_disconnected.connect(_on_player_disconnected)
	multiplayer.connected_to_server.connect(_on_connected_ok)
	multiplayer.connection_failed.connect(_on_connected_fail)
	multiplayer.server_disconnected.connect(_on_server_disconnected)

#========================================================================================#
# Multiplayer Setup
#========================================================================================#

func join_game(address = ""):
	if address.is_empty():
		address = DEFAULT_SERVER_IP
	var peer = ENetMultiplayerPeer.new()
	var error = peer.create_client(address, PORT)
	if error:
		return error
	multiplayer.multiplayer_peer = peer

func create_game(gameType : int):
	var peer = ENetMultiplayerPeer.new()
	var error = peer.create_server(PORT, MAX_CONNECTIONS)
	print('Create Server Error: ', error)
	if error:
		return error
	multiplayer.multiplayer_peer = peer

	player_connected.emit(1, player_info)
	if(gameType == 0):
		gameSceneID = gameType;
		load_scene.rpc_id(1, nav_agent)
	elif (gameType == 1):
		gameSceneID = gameType;
		load_scene.rpc_id(1, flow_field)
	else:
		load_scene.rpc_id(1, nav_agent)
		
@rpc("call_local", "reliable")
func load_scene(game_scene_path):
	var gameLoaded = get_tree().change_scene_to_file(game_scene_path)
	
		
	if gameLoaded != OK:
		print('Failure')
	else:
		print('Success')

#========================================================================================#
# Connection, Registration, & Disconnection
#========================================================================================#

func remove_multiplayer_peer():
	print("Removing Multiplayer Peer")
	multiplayer.multiplayer_peer = null

func _on_player_connected(id : int):
	_register_player.rpc_id(id, player_info)
	if multiplayer.is_server():
		if (gameSceneID == 0):
			load_scene.rpc_id(id, nav_agent)
		elif (gameSceneID == 1):
			load_scene.rpc_id(id, flow_field)
		else:
			load_scene.rpc_id(id, nav_agent)

@rpc("any_peer", "reliable")
func _register_player(new_player_info):
	var new_player_id = multiplayer.get_remote_sender_id()
	players[new_player_id] = new_player_info
	player_connected.emit(new_player_id, new_player_info)

func _on_player_disconnected(id : int):
	print("Player Dissconnected: ", id)
	remove_player.emit(id)
	player_disconnected.emit(id)

func _on_connected_ok():
	var peer_id = multiplayer.get_unique_id()
	players[peer_id] = player_info
	player_connected.emit(peer_id, player_info)

func _on_connected_fail():
	print("Connection Failed: ", multiplayer.get_unique_id())
	multiplayer.multiplayer_peer = null

func _on_server_disconnected():
	print("Server Disconnected")
	multiplayer.multiplayer_peer = null
	players.clear()
	server_disconnected.emit()

#========================================================================================#
# Player Info Changes
#========================================================================================#

@rpc("any_peer", "call_local", "reliable")
func update_player_info(playerName):
	var sender = multiplayer.get_remote_sender_id()
	if !players.has(sender):
		player_info = {
			"name": playerName,
		}
	else:
		player_info = {
			"name": playerName,
		}
