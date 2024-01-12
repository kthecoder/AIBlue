# Project Read Me

# Setup

1. Changes made to the .vscode/tasks.json
   1. change `scons` to `python -mSCons` or else you will bang your head against a wall trying to get the pathing to work on windows

# ⚙️ Building the extension

### VSCode Compilation

1. For the initial build you can run the vscode task `initial-build-extension`.
   1. This compiles both godot-cpp and the extension.
1. For all subsequent builds, you only need to run the task `build-extension`.

### Debugging via VSCode

1. Run the `build-extension [dev build]` command.
   1. To use this command you need to also modify your specific Godot paths inside the `.vscode/tasks.json` file.
   1. For further information see [here](https://github.com/paddy-exe/GDExtensionSummator/issues/50).

# Godot Specifics

1. Some classes are renamed from the original CPP version due to the fact that Godot uses those names for some features like ActionType.
