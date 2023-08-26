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

#include "vrEmu6522.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#if PICO_BUILD
#include "pico/stdlib.h"
#else
#define __not_in_flash(x)
#define __time_critical_func(fn) fn
#endif


#pragma warning(disable : 4100)


/* ------------------------------------------------------------------
 * PRIVATE DATA STRUCTURE
 */
struct vrEmu6522_s
{
  vrEmu6522Model model;

  uint8_t reg[16];

  bool t1Active;
  bool t2Active;

  vrEmu6522Interrupt intPin;


};

/* ------------------------------------------------------------------
 * PRIVATE CONSTANTS
 */

#define VIA_REG_PORT_B  0x00
#define VIA_REG_PORT_A  0x01
#define VIA_REG_DDR_B   0x02
#define VIA_REG_DDR_A   0x03
#define VIA_REG_T1C_L   0x04
#define VIA_REG_T1C_H   0x05
#define VIA_REG_T1L_L   0x06
#define VIA_REG_T1L_H   0x07
#define VIA_REG_T2C_L   0x08
#define VIA_REG_T2C_H   0x09
#define VIA_REG_SR      0x0a
#define VIA_REG_ACR     0x0b
#define VIA_REG_PCR     0x0c
#define VIA_REG_IFR     0x0d
#define VIA_REG_IER     0x0e
#define VIA_REG_PORT_Ax 0x0f


#define VIA_IFR_CA2 (1 << 0)
#define VIA_IFR_CA1 (1 << 1)
#define VIA_IFR_SR  (1 << 2)
#define VIA_IFR_CB2 (1 << 3)
#define VIA_IFR_CB1 (1 << 4)
#define VIA_IFR_T2  (1 << 5)
#define VIA_IFR_T1  (1 << 6)
#define VIA_IFR_IRQ (1 << 7)

/* ------------------------------------------------------------------
 *  HELPER FUNCTIONS
 * ----------------------------------------------------------------*/
inline static void viaIfrSetIrq(VrEmu6522* vr6522)
{
  uint8_t ier = vr6522->reg[VIA_REG_IER] & 0x7f;
  uint8_t ifr = vr6522->reg[VIA_REG_IFR] & 0x7f;

  if (ier & ifr) ifr |= 0x80;

  vr6522->reg[VIA_REG_IFR] = ifr;
  vr6522->intPin = vr6522->reg[VIA_REG_IFR] & 0x80 ? IntRequested : IntCleared;
}

inline static void viaIfrSetBit(VrEmu6522* vr6522, uint8_t bit)
{
  vr6522->reg[VIA_REG_IFR] |= bit & 0x7f;
  viaIfrSetIrq(vr6522);
}

inline static void viaIfrResetBit(VrEmu6522* vr6522, uint8_t bit)
{
  vr6522->reg[VIA_REG_IFR] &= ~(bit & 0x7f);
  viaIfrSetIrq(vr6522);
}

inline static void viaIerSet(VrEmu6522* vr6522, uint8_t bit)
{
  if (bit & 0x80)
  {
    vr6522->reg[VIA_REG_IER] |= 0x80 | bit;
  }
  else
  {
    vr6522->reg[VIA_REG_IER] &= 0x80 | ~bit;
  }

}


/* ------------------------------------------------------------------
 *
 * create a new 6522
 */
VR_EMU_6522_DLLEXPORT VrEmu6522* vrEmu6522New(
  vrEmu6522Model model)
{
  VrEmu6522* vr6522 = (VrEmu6522*)malloc(sizeof(VrEmu6522));
  if (vr6522 != NULL)
  {
    vr6522->model = model;

    vrEmu6522Reset(vr6522);
 }

  return vr6522;
}

/* 
 * destroy a 6522
 */
VR_EMU_6522_DLLEXPORT void vrEmu6522Destroy(VrEmu6522* vr6522)
{
  if (vr6522)
  {
    /* destruction */
    free(vr6522);
  }
}

/* 
 * reset the 6522
 */
VR_EMU_6522_DLLEXPORT void vrEmu6522Reset(VrEmu6522* vr6522)
{
  if (vr6522)
  {
    for (int i = 0; i < 4; ++i)
    {
      vr6522->reg[i] = 0;
    }

    for (int i = 11; i < 16; ++i)
    {
      vr6522->reg[i] = 0;
    }

    vr6522->t1Active = false;
    vr6522->t2Active = false;

    /* initialization */
    vr6522->intPin = IntCleared;
  }
}

/*
 * write to the via
 */
VR_EMU_6522_DLLEXPORT void vrEmu6522Write(VrEmu6522* vr6522, uint8_t addr, uint8_t data)
{
  uint8_t reg = addr & 0x0f;

  switch (reg)
  {
    case VIA_REG_T1C_L:
      reg = VIA_REG_T1L_L;
      break;

    case VIA_REG_T1C_H:
      reg = VIA_REG_T1L_H;
      vr6522->reg[VIA_REG_T1C_H] = data;
      vr6522->reg[VIA_REG_T1C_L] = vr6522->reg[VIA_REG_T1L_L];
      viaIfrResetBit(vr6522, VIA_IFR_T1);
      vr6522->t1Active = true;
      break;

    case VIA_REG_T2C_H:
      viaIfrResetBit(vr6522, VIA_IFR_T2);
      vr6522->t2Active = true;
      break;

    case VIA_REG_IFR:
      viaIfrResetBit(vr6522, data);
      data = vr6522->reg[reg];
      break;

    case VIA_REG_IER:
      viaIerSet(vr6522, data);
      data = vr6522->reg[reg];
      break;
  }

  vr6522->reg[reg] = data;
}

/*
 * read from the via
 */
VR_EMU_6522_DLLEXPORT uint8_t vrEmu6522Read(VrEmu6522* vr6522, uint8_t addr)
{
  uint8_t reg = addr & 0x0f;
  uint8_t value = vr6522->reg[addr & 0x0f];

  switch (reg)
  {
    case VIA_REG_T1C_L:
      viaIfrResetBit(vr6522, VIA_IFR_T1);
      break;

    case VIA_REG_T2C_L:
      vr6522->t2Active = false;
      viaIfrResetBit(vr6522, VIA_IFR_T2);
      break;

  }

  return value;
}

VR_EMU_6522_DLLEXPORT uint8_t vrEmu6522ReadDbg(VrEmu6522* vr6522, uint8_t addr)
{
  return vr6522->reg[addr & 0x0f];
}

/* 
 * a single clock tick
 */
VR_EMU_6522_DLLEXPORT void __time_critical_func(vrEmu6522Tick)(VrEmu6522* vr6522)
{
  if (vr6522->t1Active)
  {
    uint16_t t1 = (vr6522->reg[VIA_REG_T1C_H] << 8) | vr6522->reg[VIA_REG_T1C_L];
    --t1;
    vr6522->reg[VIA_REG_T1C_H] = (t1 & 0xff00) >> 8;
    vr6522->reg[VIA_REG_T1C_L] = (t1 & 0x00ff);

    if (t1 == 0)
    {
      viaIfrSetBit(vr6522, VIA_IFR_T1);
      if (vr6522->reg[VIA_REG_ACR] & 0x40)
      {
        vr6522->reg[VIA_REG_T1C_L] = vr6522->reg[VIA_REG_T1L_L];
        vr6522->reg[VIA_REG_T1C_H] = vr6522->reg[VIA_REG_T1L_H];
      }
    }
  }

  if (vr6522->t2Active)
  {
    uint16_t t2 = (vr6522->reg[VIA_REG_T2C_H] << 8) | vr6522->reg[VIA_REG_T2C_L];
    --t2;
    vr6522->reg[VIA_REG_T2C_H] = (t2 & 0xff00) >> 8;
    vr6522->reg[VIA_REG_T2C_L] = (t2 & 0x00ff);

    if (t2 == 0)
    {
      viaIfrSetBit(vr6522, VIA_IFR_T2);
      // one-shot just continues counting (from 0xffff)
    }
  }
}



/* ------------------------------------------------------------------
 *
 * returns a pointer to the interrupt signal.
 * externally, you can modify it to set/reset the interrupt signal
 */
VR_EMU_6522_DLLEXPORT vrEmu6522Interrupt* vrEmu6522Int(VrEmu6522* vr6522)
{
  if (vr6522)
  {
    return &vr6522->intPin;
  }
  return NULL;
}
