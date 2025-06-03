extends Node3D

const agent = preload("res://controller/agent/AgentController_FlowField.tscn")

signal spawn_player(playerID : int)

var grid_origin: Vector3 = Vector3(-100.0, 0.0, -100.0);  # Example for a 200×200 area centered at (0,0,0)
var cell_size: float = 2.0;

var myFlowField: FlowField;

var grid_size = 100  # Grid cells
var label_spacing = 20  # Display labels every 20 cells


func _ready():
	GlobalSettings.apply_graphics_settings(get_window(), $WorldEnvironment.environment, self)
	if multiplayer.is_server():
		myFlowField = FlowField.new();
		var all_occupied_cells := []

		for child in $"Obstacles".get_children():
			if child is MeshInstance3D:
				var mesh_instance: MeshInstance3D = child
				var local_aabb: AABB = mesh_instance.get_aabb()
				var global_aabb: AABB = mesh_instance.global_transform * mesh_instance.get_aabb()
				
				# Shift the global AABB to grid-local coordinates by offsetting with grid_origin.
				var aabb_min: Vector3 = global_aabb.position - grid_origin
				var aabb_max: Vector3 = (global_aabb.position + global_aabb.size) - grid_origin
				
				# Convert the X and Z coordinates to grid cell indices (using floor() for proper rounding).
				var min_cell_x: int = int(floor(aabb_min.x / cell_size))
				var min_cell_y: int = int(floor(aabb_min.z / cell_size))  # We use Z as our grid’s "y" index.
				var max_cell_x: int = int(floor(aabb_max.x / cell_size))
				var max_cell_y: int = int(floor(aabb_max.z / cell_size))
				
				# Loop over the cells covered by this AABB and add them.
				for x in range(min_cell_x, max_cell_x + 1):
					for y in range(min_cell_y, max_cell_y + 1):
						all_occupied_cells.append(Vector2(x, y))

			myFlowField.compute_flow(grid_size, grid_size, 0, 0, all_occupied_cells);
	
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
	
func get_movement_vector(world_pos : Vector3) -> Vector3:
	return myFlowField.get_move_direction(world_pos, cell_size);
