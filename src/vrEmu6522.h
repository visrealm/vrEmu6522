/*
 * Troy's 6522 Emulator
 *
 * Copyright (c) 2023 Troy Schrapel
 *
 * This code is licensed under the MIT license
 *
 * https://github.com/visrealm/vrEmu6522
 *
 */

#ifndef _VR_EMU_6522_H_
#define _VR_EMU_6522_H_

/* ------------------------------------------------------------------
 * LINKAGE MODES:
 * 
 * Default (nothing defined):    When your executable is using vrEmuTms9918 as a DLL
 * VR_6522_EMU_COMPILING_DLL:    When compiling vrEmuTms9918 as a DLL
 * VR_6522_EMU_STATIC:           When linking vrEmu6522 statically in your executable
 */

#if VR_6522_EMU_COMPILING_DLL
  #define VR_EMU_6522_DLLEXPORT __declspec(dllexport)
#elif VR_6522_EMU_STATIC
  #ifdef __cplusplus
    #define VR_EMU_6522_DLLEXPORT extern "C"
  #else
    #define VR_EMU_6522_DLLEXPORT extern
  #endif
#elif __EMSCRIPTEN__
  #include <emscripten.h>
  #define VR_EMU_6522_DLLEXPORT EMSCRIPTEN_KEEPALIVE
#else
  #define VR_EMU_6522_DLLEXPORT __declspec(dllimport)
#endif

#include <stdint.h>
#include <stdbool.h>  

/* ------------------------------------------------------------------
 * PRIVATE DATA STRUCTURE
 */
struct vrEmu6522_s;
typedef struct vrEmu6522_s VrEmu6522;

/* ------------------------------------------------------------------
 * CONSTANTS
 */
typedef enum
{
  VIA_6522,     /* NMOS 6522 */
  VIA_65C22,    /* Standard CMOS 65C22 */
} vrEmu6522Model;

typedef enum
{
  IntRequested,
  IntCleared,
  IntLow = IntRequested,
  IntHigh = IntCleared
} vrEmu6522Interrupt;


/*
 * create a new 6522
 */
VR_EMU_6522_DLLEXPORT VrEmu6522* vrEmu6522New(vrEmu6522Model model);

/*
 * destroy a 6522
 */
VR_EMU_6522_DLLEXPORT void vrEmu6522Destroy(VrEmu6522* vr6522);

/*
 * reset the 6522
 */
VR_EMU_6522_DLLEXPORT void vrEmu6522Reset(VrEmu6522* vr6522);

/*
 * write to the via
 */
VR_EMU_6522_DLLEXPORT void vrEmu6522Write(VrEmu6522* vr6522, uint8_t addr, uint8_t data);

/*
 * read from the via
 */
VR_EMU_6522_DLLEXPORT uint8_t vrEmu6522Read(VrEmu6522* vr6522, uint8_t addr);

/*
 * read from the via (for debug - don't change the state)
 */
VR_EMU_6522_DLLEXPORT uint8_t vrEmu6522ReadDbg(VrEmu6522* vr6522, uint8_t addr);


/*
 * a single clock tick
 */
VR_EMU_6522_DLLEXPORT void vrEmu6522Tick(VrEmu6522* vr6522);

/*
 * returns a pointer to the interrupt signal.
 * externally, you can modify it to set/reset the interrupt signal
 */
VR_EMU_6522_DLLEXPORT vrEmu6522Interrupt *vrEmu6522Int(VrEmu6522* vr6522);



#endif // _VR_EMU_6522__H_
