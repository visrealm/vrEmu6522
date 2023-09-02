# vrEmu6522

![build-win64-badge](https://github.com/visrealm/vrEmu6522/actions/workflows/build-win64.yml/badge.svg)

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

## License
This code is licensed under the [MIT](https://opensource.org/licenses/MIT "MIT") license
