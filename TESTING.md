# C++ Testing Setup

1. Installing Bazel
   1. Windows : `choco install bazelisk`
   1. Npm : `npm install -g @bazel/bazelisk`
1. Setting up
   1. Requires that you symlink the `extension/src/FluidHTN` into `test/cpp/FluidHTN`
1. Running
   1. `bazel test --test_output=all //:test`
