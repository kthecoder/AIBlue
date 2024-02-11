extends CharacterBody3D

enum TaskStatus
{
	Continue,
	Success,
	Failure
};

enum WsAgent
{
	wsEnemyRange,
	wsAgentMovement,
	wsHealth,
};

enum AgentMovement
{
	NewPosition,
	Arrived,
};

const SPEED = 4.0
var gravity: float = ProjectSettings.get_setting("physics/3d/default_gravity")

@onready var aiBrain = $AIBlueAgent
@onready var nav_agent = $NavigationAgent3D
@onready var agent_tick_rate = $AgentTickRate

var initial_position : Vector3
var last_position : Vector3
var moveToLocation : Vector3 = Vector3.ZERO

var isRotationQuick : bool = false

var agentDelta : float = 0.0

func _ready():
	if multiplayer.is_server():
		# Hook Up GDExtension HTN to the Agent
		aiBrain.agent_setup(self.get_path())
		# Increase Performance by restricting the total Slides
		set_max_slides(2)
		# Requires that NavigationAgent3D -> Avoidance Enabled == True
		nav_agent.velocity_computed.connect(_move_agent)
		nav_agent.target_reached.connect(_cancel_navigation)
		agent_tick_rate.timeout.connect(_tick_rate_complete)
		
		var playerLoc = closest_player()
		if playerLoc:
			update_target_loc(playerLoc)

func _physics_process(delta :  float):
	agentDelta = delta
	
	if not is_on_floor():
		velocity.y -= gravity * delta
	# If we're below -40, respawn (teleport to the initial position).
	# We have fallen off the map
	if transform.origin.y < -40:
		transform.origin = initial_position
		
func _tick_rate_complete():
	
	last_position = global_position
	var playerLoc = closest_player()
	if playerLoc:
		update_target_loc(playerLoc)
	
	
func _cancel_navigation() -> void:
	nav_agent.emit_signal("navigation_finished")
	nav_agent.set_target_position(global_position)
	aiBrain.movement_update(WsAgent.wsAgentMovement, AgentMovement.Arrived)
	
func _move_agent(safe_velocity : Vector3):
	velocity = safe_velocity
	
	var collision : KinematicCollision3D = move_and_collide(safe_velocity * get_physics_process_delta_time())
	if collision:
		var collider : Object = collision.get_collider()
		if collider is CharacterBody3D:
			velocity = safe_velocity.slide(collision.get_normal())
		elif collider is StaticBody3D:
			move_and_slide()

func update_target_loc(target_loc):
	
	moveToLocation = target_loc + Vector3(randf_range(-1.5,1.5), 0, randf_range(-1.5,1.5))
	nav_agent.set_target_position(moveToLocation)
	# Update AI Brain State
	aiBrain.movement_update(WsAgent.wsAgentMovement, AgentMovement.NewPosition)

func moveTo() -> int:
	if nav_agent.is_navigation_finished():
		return TaskStatus.Success
	if(global_position != moveToLocation):
		var next_location = nav_agent.get_next_path_position()
		var new_direction : Vector3 = global_position.direction_to(next_location) * nav_agent.max_speed
		# Ensure Units Rotate correctly 
		_rotate_agent(new_direction)
		var steering_behavior_velocity : Vector3 = (new_direction - velocity) * agentDelta * SPEED
		var new_velocity : Vector3 = velocity + steering_behavior_velocity
		if nav_agent.avoidance_enabled:
			nav_agent.set_velocity(new_velocity)
		else:
			_move_agent(new_velocity)
	
		return TaskStatus.Continue;
	elif(global_position == moveToLocation):
		print("No where to move")
		return TaskStatus.Success;
	else:
		print("Failure")
		return TaskStatus.Failure;

func _rotate_agent(direction : Vector3):
	if isRotationQuick:
		rotation.y = atan2(velocity.x, velocity.y)
	else:
		var pos : Vector2 = Vector2(-transform.basis.z.x, -transform.basis.z.z)
		var outcome : Vector2 = Vector2(direction.x, direction.z)
		rotation.y -= pos.angle_to(outcome) * agentDelta * SPEED

func closest_player() -> Vector3:
	var players = get_tree().get_nodes_in_group("Player")
	var closest_player_location = null
	var closest_distance = INF

	for player in players:
		var distance = self.global_transform.origin.distance_to(player.global_transform.origin)
		if distance < closest_distance:
			closest_distance = distance
			closest_player_location = player
	if closest_player_location:
		return closest_player_location.global_transform.origin
	else:
		return Vector3.ZERO

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
