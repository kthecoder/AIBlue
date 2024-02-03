extends CharacterBody3D

# TODO Design the MoveTo function

func _ready():
	$AIBlueAgent.agent_setup(self.get_path())
func _process(_delta):
	pass
#
#const SPEED = 5.0
#const JUMP_VELOCITY = 4.5
#
## Get the gravity from the project settings to be synced with RigidBody nodes.
#var gravity = ProjectSettings.get_setting("physics/3d/default_gravity")
#
#
#func _physics_process(delta):
	## Add the gravity.
	#if not is_on_floor():
		#velocity.y -= gravity * delta
#
	## Handle jump.
	#if Input.is_action_just_pressed("ui_accept") and is_on_floor():
		#velocity.y = JUMP_VELOCITY
#
	## Get the input direction and handle the movement/deceleration.
	## As good practice, you should replace UI actions with custom gameplay actions.
	#var input_dir = Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	#var direction = (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
	#if direction:
		#velocity.x = direction.x * SPEED
		#velocity.z = direction.z * SPEED
	#else:
		#velocity.x = move_toward(velocity.x, 0, SPEED)
		#velocity.z = move_toward(velocity.z, 0, SPEED)
#
	#move_and_slide()

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


				
		
	
