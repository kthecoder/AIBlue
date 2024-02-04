extends CharacterBody3D

const SPEED = 3.0

@onready var nav_agent = $NavigationAgent3D

func _ready():
	$AIBlueAgent.agent_setup(self.get_path())
func _process(_delta):
	var current_location = global_transform.origin
	var next_location = nav_agent.get_next_location()
	var new_velocity = (next_location - current_location).normalized() * SPEED
	
	velocity = new_velocity
	move_and_slide()

func update_target_loc(target_loc):
	nav_agent.set_target_location(target_loc)

func _on_vision_sensor_timeout():
	var overlaps : Array = $VisionArea.get_overlapping_bodies()
	if overlaps.size() > 0:
		for overlap in overlaps:
			if overlap.name == "Player":
				print('Vision Sensor : Detecting Possible Enemy')
				var playerPos = overlap.global_transform.origin
				$VisionRayCast.look_at(playerPos, Vector3.UP)
				$VisionRayCast.force_raycast_update()
				
				if $VisionRayCast.is_colliding():
					var collider = $VisionRayCast.get_collider()
					
					if collider.name == "Player":
						print('Vision Sensor : Detected Enemy in Sight')

func moveTo() -> bool:
	#print("Move Agent To Location")
	return true;

func closest_player() -> Vector3:
	var players = get_tree().get_nodes_in_group("Player")
	var closest_player = null
	var closest_distance = INF

	for player in players:
		var distance = self.global_transform.origin.distance_to(player.global_transform.origin)
		if distance < closest_distance:
			closest_distance = distance
			closest_player = player
	
	return closest_player.global_transform
