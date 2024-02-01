extends CharacterBody3D

# TODO Design the MoveTo function

var agentHTN : RefCounted

func _ready():
	agentHTN = AIBlueAgent.new()
	
func _process(_delta):
	agentHTN.planner_tick();

func moveTo() -> bool:
	print("Move Agent To Location")
	return true;


				
		
	
