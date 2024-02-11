extends Node3D

const agent = preload("res://controller/agent/AgentController.tscn")

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
	
	for spawner in $Spawning/AgentSpawners.get_children():
		spawner.connect("spawn_agent", Callable(self, "_spawn_agent"))

func _spawn_player(playerID : int):
	spawn_player.emit(playerID)
	
func _spawn_agent(spawnLocation : Vector3):
	var newAgent = agent.instantiate()
	newAgent.position = spawnLocation
	$Spawning/SpawnedAgents.add_child(newAgent, true)
