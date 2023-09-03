# vrEmu6522

<a href="https://github.com/visrealm/vrEmu6522/actions/workflows/cmake-multi-platform.yml"><img src="https://github.com/visrealm/vrEmu6522/actions/workflows/cmake-multi-platform.yml/badge.svg"/></a>

6522/65C22 emulator written in standard C99 with no external dependencies.

Initially created for my [HBC-56 (6502 on a backplane) Emulator](https://github.com/visrealm/hbc-56)

### Development status

Unlike my [vrEmu6502](https://github.com/visrealm/vrEmu6502) and [vrEmuTms9918](https://github.com/visrealm/vrEmuTms9918) libraries, this library is in the very early stages of development. So far, I am only implementing features of the 65C22 that I am actively using in my HBC-56 project.

#### Supported features

* Timer 1 (one-shot and free-running modes)
* Timer 2 (one-shot mode)

#### Unsupported features

* Everything else

Additional features will be added as required.

## Building

vrEmu6522 uses the CMake build system

#### Checkout repository:

```
git clone https://github.com/visrealm/vrEmu6522.git
cd vrEmu6522
```

#### Setup build:

```
mkdir build
cd build
cmake ..
```

#### Build

```
cmake --build .
```
Windows: Optionally, open the generated solution file

#### Run tests
```
ctest
```
Windows: Optionally, build the ALL_TESTS project in the generated solution file

## License
This code is licensed under the [MIT](https://opensource.org/licenses/MIT "MIT") license
