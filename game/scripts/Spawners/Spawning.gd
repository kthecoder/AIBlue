extends Node

const playerScene = preload("res://controller/player/PlayerController.tscn")

@onready var playerSpawnLocations : Array[Node]
@onready var playerBucket = $SpawnedPlayers

var currentPlayerCount : int = 0

func _ready():
	if multiplayer.is_server():
		$"..".spawn_player.connect(_spawn_player)
		multiplayer.peer_disconnected.connect(_remove_player)
		playerSpawnLocations = $PlayerSpawnPoints.get_children()

func _spawn_player(peer_id : int):
	if multiplayer.is_server():
		var setupPlayer = playerScene.instantiate()
		# Set the Node name to the Peer ID
		setupPlayer.name = str(peer_id)
		# Set the Player's Entity to their Peer ID
		setupPlayer.player_id = peer_id
		setupPlayer.transform = playerSpawnLocations[currentPlayerCount].global_transform
		# Add Player to the Scene Tree
		playerBucket.add_child(setupPlayer)
		# Increment number of spawned players
		currentPlayerCount += 1
	
func _remove_player(peer_id : int):
	if multiplayer.is_server():
		for player in playerBucket.get_children():
			if player.name == str(peer_id):
				var playerNode = playerBucket.get_node(player.path())
				playerNode.queue_free()
				currentPlayerCount -= 1
