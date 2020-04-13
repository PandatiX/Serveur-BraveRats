#Serveur BraveRats
Serveur BraveRats is a C/C++11 server prototype to play BraveRats.

## Dependencies
- [JSON for Modern C++][1]

[1]: https://github.com/nlohmann/json

## How to compile
After cloning the repo, run:
```shell
pushd <repoDir>
cmake -S . -B cmake-build-debug
pushd cmake-build-debug
```
Execute `./ServeurBraveRats` to run the program.
