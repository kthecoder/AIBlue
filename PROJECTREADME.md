# Project Read Me

# Setup

1. Get the Godot CPP Submodules
   1. `git submodule update --remote`
1. Changes made to the .vscode/tasks.json
   1. change `scons` to `python -mSCons` or else you will bang your head against a wall trying to get the pathing to work on windows
1. C++ Configurations
   1. Must include new directories inside `c_cpp_properties.json`
1. SCONS Configuration
   1. Must include extra folders in the `SConstruct`'s `sources`; if you desire a different setup

# ⚙️ Building the extension

`CTRL+SHIFT+P` then type `task build`

### VSCode Compilation

1. For the initial build you can run the vscode task `initial-build-extension`.
   1. This compiles both godot-cpp and the extension.
1. For all subsequent builds, you only need to run the task `build-extension`.

### Debugging via VSCode

1. First ensure you have run the `build-extension [dev build]` vs code task command
   1. Command found in the `CTRL + P` menu, type `task`
   1. To use this command you need to also modify your specific Godot paths inside the `.vscode/tasks.json` file.
   1. You need to set the path to your godot project folder
1. You can then run the debug in VS Code
   1. Use : `Run Demo [FROM GAME] - [DEBUG MODE]`
1. When you get errors that are due to FluidHTN
   1. The errors will show a line in FluidHTN and it will actually be the Fatal Exception in the line of code above that

# Code Changes

1. Some classes are renamed from the original CPP version due to the fact that Godot uses those names for some features like ActionType.

# Godot Specifics

There are two ways of implementing C++ into Godot.

1. Linking (Modules)
   1. Implement Custom DLL's that are called on Runtime
   1. Can make small changes and only have to rebuild DLL's but for each platform
   1. Create a GDExtension Library
   1. Add GDExtension Library to the GDScript's NativeScripts export option in the inspector panel
   1. If done correctly there should be a node called "AIBlueAgent"
      1. Attaching this to the scene connects the GDExtension to your game
   1. Documentation : https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html
   1. Tutorial : https://youtu.be/kn8H2nlx3xY?si=Fyp35O7tR6weiRNk
2. Integration
   1. Implement the C++ code into the engine
      1. Basically you rebuild the engine and include your C++ code
   2. No platform specific DLL's needed
   3. Have to build the engine every time you make a change to the C++ code
   4. Tutorial : https://www.youtube.com/watch?v=81uoyvvSG2A&list=PLZ2NyOcFnfQOko5ZysZeGkH3bfw2UtbOK&index=8

Note : You could develop your extension using GDExtension and then when you are ready to ship your game, you could build an Integration into the actual engine to avoid needing platform specific binaries.
