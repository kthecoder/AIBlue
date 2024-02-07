# Project Read Me

# Usage

1. Setup the Project with the setup instructions below
1. Build the Project
   1. Frist time : `CTRL + P` -> `task initial-build-extension`
   1. In VSCode : `CTRL + P` -> `task build-extension`
   1. For use in Development Game & Debugging : `CTRL + P` -> `task build-extension [dev build]`
1. Ensure that the bin folder contains `ai_blue.gdextension`
   1. If not you can find one in the Godot Docs: https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html#using-the-gdextension-module
1. Modify the Agents Folder && AI_Blue_Agent.cpp/.h to fit your project needs
   1. Write your HTN Domains and World States in C++
   1. Write your Operators to call functions in GDScript, to perform actual actions in your game
1. Build the project when you finalize your C++ code
1. Copy the bin folder `game/bin` to your project
1. Use your Agent Nodes
   1. In the included example the Agent Brains are setup as Node3D, that can be found in the search of the add node menu in Godot engine
1. Call functions on the Agents inside GDScript
   1. Get a path to your Agent 3D node, and call the functions you defined for sensors in your Agent-C++

# Setup

This setup assumes you are using Windows.

1. Clone the repo
   1. `git clone --recurse-submodules https://github.com/kthecoder/AIBlue.git`
1. Get the Godot CPP Submodules if you didn't use --recurse-submodules
   1. `git submodule update --init --recursive`
1. Install Python and Scons
1. Changes were already made to the .vscode/tasks.json to make pathing easier
   1. Change `scons` to `python -mSCons` or else you will bang your head against a wall trying to get the pathing to work on windows
1. `.vscode`
   1. `.vscode/launch.json`
      1. Replace `"program": "C:/Program Files (x86)/Godot_4.2.1/Godot_v4.2.1.exe",` with the location of your Godot exe
   1. `.vscode/c_cpp_properties.json`
      1. Replace `"compilerPath": "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.38.33130/bin/Hostx64/x64/cl.exe",` with the location fo your `cl.exe`
      1. This is for windows only, compiler pathing will differ on mac and linux
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
