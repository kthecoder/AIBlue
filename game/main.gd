extends Node

func _ready() -> void:
	var s = AIBlue.new()
	s.add(10)
	s.add(20)
	s.add(30)
	print(s.get_total())
	s.reset()

