class_name Player
extends CharacterBody3D

const DIRECTION_INTERPOLATE_SPEED = 1
const MOTION_INTERPOLATE_SPEED = 10
const ROTATION_INTERPOLATE_SPEED = 10

# How fast the player moves in meters per second.
@export var simple_speed = 14

const MIN_AIRBORNE_TIME = 0.1
const JUMP_SPEED = 5

var airborne_time = 100

var orientation = Transform3D()
var root_motion = Transform3D()
var motion = Vector2()

@onready var initial_position = transform.origin
@onready var gravity = ProjectSettings.get_setting("physics/3d/default_gravity") * ProjectSettings.get_setting("physics/3d/default_gravity_vector")

@onready var player_input = $InputSynchronizer
@onready var player_model = $PlayerModel

@export var player_id := 1 :
	set(value):
		player_id = value
		$InputSynchronizer.set_multiplayer_authority(value)

func _ready():
	
	"""
	Ref : https://forum.godotengine.org/t/what-is-the-difference-between-global-and-local-coordinates/25891
	
	Local co-ordinates are relative to the parent that the node is a child of. 
	Global co-ordinates are an exact position on the screen (including off-screen space).
	
	For example if you have a Node2D positioned at x:50, y:100 in the main scene. 
	If you add a child node (say a Sprite), that Sprite will have a local 
	position of 0,0 but a global position of 50, 100. 
	Let’s imagine you want to move the Sprite 10 pixels to the right. 
	You set it’s position to 10,0 which is relative to it’s parent. 
	Now it has global co-ordinates of 60, 100 and local co-ordinates of 10, 0.
	"""
	# Pre-initialize orientation transform.
	orientation = player_model.global_transform
	#Transform.origin : The translation offset of the transform (column 3, the fourth column). Equivalent to array index 3.
	#Translation gives the node's position relative to its parent node aka Local Space.
	# Origin is the vector between object and parent
	# Local Coordinates
	orientation.origin = Vector3()
	if not multiplayer.is_server():
		set_process(false)


func _physics_process(delta: float):
	if multiplayer.is_server():
		apply_input(delta)


func apply_input(delta: float):
	motion = motion.lerp(player_input.motion, MOTION_INTERPOLATE_SPEED * delta)

	var camera_basis : Basis = player_input.get_camera_rotation_basis()
	var camera_z := camera_basis.z
	var camera_x := camera_basis.x

	camera_z.y = 0
	camera_z = camera_z.normalized()
	camera_x.y = 0
	camera_x = camera_x.normalized()

	
	"""
		Quaternions
		https://www.youtube.com/watch?v=1yoFjjJRnLY
	"""
	# Convert orientation to quaternions for interpolating rotation.
	var target = camera_x * motion.x + camera_z * motion.y

	if target.length() > 0.001:
		var q_from = orientation.basis.get_rotation_quaternion()
		var q_to = Transform3D().looking_at(target, Vector3.UP).basis.get_rotation_quaternion()
		# Interpolate current rotation with desired one.
		orientation.basis = Basis(q_from.slerp(q_to, delta * ROTATION_INTERPOLATE_SPEED))

	var h_velocity = orientation.origin / delta
	
	"""
	Velocity is a vector quantity that represents the rate of change of an object’s 
	position with respect to time. It includes both the speed (magnitude) and the direction of movement.
	"""
	#Horizontal Velocity
	# The player’s input should be applied to the horizontal motion only (X and Z axes), since gravity is acting on the Y axis. 
	velocity.x = h_velocity.x + target.x * simple_speed
	velocity.z = h_velocity.z + target.z * simple_speed
	velocity += gravity * delta 
	
	
	set_velocity(velocity)
	set_up_direction(Vector3.UP)
	
	"""
	Move & Slide
	
	Actually moves the CharacterBody3D
	"""
	move_and_slide()

	orientation.origin = Vector3() # Clear accumulated root motion displacement (was applied to speed).
	orientation = orientation.orthonormalized() # Orthonormalize orientation.

	player_model.global_transform.basis = orientation.basis

	# If we're below -40, respawn (teleport to the initial position).
	# We have fallen off the map
	if transform.origin.y < -40:
		transform.origin = initial_position


@rpc("call_local")
func shoot():
	pass


@rpc("call_local")
func hit():
	pass
