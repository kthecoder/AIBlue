'''
==================
Godot TPS Demo
Link : https://github.com/godotengine/tps-demo
==================

All code Copyright (c) 2018-2021 Juan Linietsky, Godot Engine contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons 
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

==================
'''

extends Node

enum GIType {
	SDFGI = 0,
	VOXEL_GI = 1,
	LIGHTMAP_GI = 2,
}

enum GIQuality {
	DISABLED = 0,
	LOW = 1,
	HIGH = 2,
}

const CONFIG_FILE_PATH = "user://settings.cfg"
var config_file := ConfigFile.new()

const DEFAULTS = {
	gameSettings = {
		display_name = '',
		music_volume = 50, # int 0 to 100
		sfx_volume = 50, # int 0 to 100
	},
	video = {
		display_mode = Window.MODE_MAXIMIZED,
		vsync = DisplayServer.VSYNC_ENABLED,
		max_fps = 0,
		resolution_scale = 1.0,
		scale_filter = Viewport.SCALING_3D_MODE_FSR2,
	},
	rendering = {
		taa = false,
		msaa = Viewport.MSAA_DISABLED,
		fxaa = false,
		shadow_mapping = true,
		gi_type = GIType.VOXEL_GI,
		gi_quality = GIQuality.LOW,
		ssao_quality = RenderingServer.ENV_SSAO_QUALITY_MEDIUM,
		ssil_quality = -1,  # Disabled
		bloom = true,
		volumetric_fog = false,
	},
}

func _ready():
	load_settings()


func _input(event):
	if event.is_action_pressed("toggle_fullscreen"):
		get_window().mode = Window.MODE_EXCLUSIVE_FULLSCREEN if (!((get_window().mode == Window.MODE_EXCLUSIVE_FULLSCREEN) or (get_window().mode == Window.MODE_FULLSCREEN))) else Window.MODE_WINDOWED
		get_viewport().set_input_as_handled()


func load_settings():
	config_file.load(CONFIG_FILE_PATH)
	# Initialize defaults for values not found in the existing configuration file,
	# so we don't have to specify them every time we use `ConfigFile.get_value()`.
	for section in DEFAULTS:
		for key in DEFAULTS[section]:
			if not config_file.has_section_key(section, key):
				config_file.set_value(section, key, DEFAULTS[section][key])


func save_settings():
	config_file.save(CONFIG_FILE_PATH)
	
	
func set_graphics_low():
	# TODO
	config_file.set_value("rendering", "ssao_quality", -1)
	config_file.set_value("rendering", "ssil_quality", -1)
	
	save_settings()
	
func set_graphics_normal():
	# TODO
	config_file.set_value("rendering", "ssao_quality", RenderingServer.ENV_SSAO_QUALITY_MEDIUM)
	config_file.set_value("rendering", "ssil_quality", RenderingServer.ENV_SSIL_QUALITY_MEDIUM)
	
	save_settings()
	
func set_graphics_high():
	# TODO
	config_file.set_value("rendering", "ssao_quality", RenderingServer.ENV_SSAO_QUALITY_MEDIUM)
	config_file.set_value("rendering", "ssil_quality", RenderingServer.ENV_SSIL_QUALITY_HIGH)
	
	save_settings()
	
func set_vsync():
	DisplayServer.window_set_vsync_mode(config_file.get_value("video", "vsync"))

# Call this fuction in the ready function of your level script
func apply_graphics_settings(window: Window, environment: Environment, sceneRoot: Node):
	get_window().mode = config_file.get_value("video", "display_mode")
	DisplayServer.window_set_vsync_mode(config_file.get_value("video", "vsync"))
	Engine.max_fps = config_file.get_value("video", "max_fps")
	window.scaling_3d_scale = config_file.get_value("video", "resolution_scale")
	window.scaling_3d_mode = config_file.get_value("video", "scale_filter")

	window.use_taa = config_file.get_value("rendering", "taa")
	window.msaa_3d = config_file.get_value("rendering", "msaa")
	window.screen_space_aa = Viewport.SCREEN_SPACE_AA_FXAA if config_file.get_value("rendering", "fxaa") else Viewport.SCREEN_SPACE_AA_DISABLED

	if not config_file.get_value("rendering", "shadow_mapping"):
		# Disable shadows for all lights present during level load,
		# reducing the number of draw calls significantly.
		# FIXME: In the main menu, shadows aren't enabled again after enabling shadows
		# if they were previously disabled. We can't enable shadows on all lights unconditionally,
		# as this would negatively affect the level's performance.
		sceneRoot.propagate_call("set", ["shadow_enabled", false])

	if config_file.get_value("rendering", "ssao_quality") == -1:
		environment.ssao_enabled = false
	if config_file.get_value("rendering", "ssao_quality") == RenderingServer.ENV_SSAO_QUALITY_MEDIUM:
		environment.ssao_enabled = true
		RenderingServer.environment_set_ssao_quality(RenderingServer.ENV_SSAO_QUALITY_HIGH, false, 0.5, 2, 50, 300)
	else:
		environment.ssao_enabled = true
		RenderingServer.environment_set_ssao_quality(RenderingServer.ENV_SSAO_QUALITY_MEDIUM, true, 0.5, 2, 50, 300)

	if config_file.get_value("rendering", "ssil_quality") == -1:
		environment.ssil_enabled = false
	elif config_file.get_value("rendering", "ssil_quality") == RenderingServer.ENV_SSIL_QUALITY_MEDIUM:
		environment.ssil_enabled = true
		RenderingServer.environment_set_ssil_quality(RenderingServer.ENV_SSIL_QUALITY_MEDIUM, false, 0.5, 2, 50, 300)
	else:
		environment.ssil_enabled = true
		RenderingServer.environment_set_ssil_quality(RenderingServer.ENV_SSIL_QUALITY_HIGH, true, 0.5, 2, 50, 300)

	environment.glow_enabled = config_file.get_value("rendering", "bloom")
	environment.volumetric_fog_enabled = config_file.get_value("rendering", "volumetric_fog")
