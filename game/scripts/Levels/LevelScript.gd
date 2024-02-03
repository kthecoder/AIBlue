extends Node3D

signal spawn_player(playerID : int)

func _ready():
	GlobalSettings.apply_graphics_settings(get_window(), $WorldEnvironment.environment, self)
	if multiplayer.is_server():
		multiplayer.peer_connected.connect(_spawn_player)
		var peers = multiplayer.get_peers()
		# The server is a player
		peers.append(1)
		for player in peers:
			spawn_player.emit(player)

func _spawn_player(playerID : int):
	spawn_player.emit(playerID)
