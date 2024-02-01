extends Node3D

const PlayerScene = preload("res://controller/player/PlayerController.tscn")
var lightmap_gi = null

var peer : MultiplayerPeer = OfflineMultiplayerPeer.new()

signal quit

@onready var world_environment = $WorldEnvironment
@onready var player_spawn_points = $PlayerSpawnPoints
@onready var spawn_node = $SpawnedNodes

func _ready():
	peer = ENetMultiplayerPeer.new()
	peer.create_server(7000)
	
	multiplayer.multiplayer_peer = peer

	if multiplayer.is_server():

		# Then spawn already connected players at random location
		randomize()
		var spawn_points = player_spawn_points.get_children()
		spawn_points.shuffle()
		add_player(1, spawn_points.pop_front())
		for id in multiplayer.get_peers():
			add_player(id, spawn_points.pop_front())

		# Then spawn/despawn players as they connect/disconnect
		multiplayer.peer_connected.connect(add_player)
		multiplayer.peer_disconnected.connect(del_player)

func del_player(id: int):
	if not spawn_node.has_node(str(id)):
		return
	spawn_node.get_node(str(id)).queue_free()


func add_player(id: int, spawn_point: Marker3D = null):
	if spawn_point == null:
		spawn_point = player_spawn_points.get_child(randi() % player_spawn_points.get_child_count())
	var player = PlayerScene.instantiate()
	player.name = str(id)
	player.player_id = id
	player.transform = spawn_point.transform
	spawn_node.add_child(player)


func _input(_event):
	pass
	#if event.is_action_pressed("quit"):
		#Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
		#emit_signal("quit")
