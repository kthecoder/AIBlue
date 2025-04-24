extends Node3D

signal spawn_agent(spawnLocation : Vector3)

@onready var rayCastSpawner = $RayCastSpawner
@onready var spawner_tick : Timer = $SpawnerTick

var spawn_point_created : bool = false
var spawnLocation : Vector3

var agentsSpawned : int = 0

func _ready():
	spawner_tick.timeout.connect(_spawner_tick_timeout)

func _process(_delta):
	if !spawn_point_created:
		# Check if the raycast hit something
		if rayCastSpawner.is_colliding():
			var collision_point = rayCastSpawner.get_collision_point()
			var collider = rayCastSpawner.get_collider()
			if collider is StaticBody3D and collider.is_in_group("Ground"):
				spawnLocation = collision_point
				spawn_point_created = true
				rayCastSpawner.enabled = false
				rayCastSpawner.queue_free()
				set_process(false)

func _spawner_tick_timeout():
	if agentsSpawned < 2 and spawn_point_created:
		spawn_agent.emit(spawnLocation)
		agentsSpawned += 1
