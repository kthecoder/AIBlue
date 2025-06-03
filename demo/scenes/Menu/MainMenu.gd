extends Control

@onready var hostButton = $VBoxContainer/Host
@onready var joinButton = $VBoxContainer/Join

@onready var playerName = 'Unknown'

func _on_host_pressed():
	MultiplayerManager.update_player_info.rpc(playerName)
	var gameType : int = $VBoxContainer2/AIGameType.selected;
	MultiplayerManager.create_game(gameType)

func _on_join_pressed():
	MultiplayerManager.update_player_info.rpc(playerName)
	var gameType : int = $VBoxContainer2/AIGameType.selected;
	MultiplayerManager.join_game()
