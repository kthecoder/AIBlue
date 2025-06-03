extends Node

var grid_origin: Vector3 = Vector3(-100.0, 0.0, -100.0);  # Example for a 200×200 area centered at (0,0,0)
var cell_size: float = 1.0;

var myFlowField: FlowField = FlowField.new();

func _ready():
	var all_occupied_cells := []

	for child in $"../Obstacles".get_children():
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

	myFlowField.compute_flow(200, 200, 0, 0, all_occupied_cells);

func get_movement_vector(world_pos : Vector3) -> Vector3:
	return myFlowField.get_move_direction(world_pos, cell_size);
