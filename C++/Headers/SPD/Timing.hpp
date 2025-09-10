 /*
================================================================================
 MIT License

 Copyright (c) 2025 gafoo

 This file is part of the HardView project:
 https://github.com/gafoo173/HardView

 Licensed under the MIT License
 See the LICENSE file in the project root for more details.
================================================================================
*/

/**
 * @version 0.1B
 */


#pragma once
#include <iomanip>
#include <string>
#include <vector>
#include <stdexcept>

struct TimingDDR3 {
  float MTB;     // Medium Timebase in ns
  float tCKmin;  // Clock cycle time in ns
  float tAAmin;  // CAS Latency in ns
  float tWRmin;  // Write Recovery time in ns
  float tRCDmin; // RAS to CAS Delay in ns
  float tRRDmin; // Row to Row Delay in ns
  float tRPmin;  // Row Precharge time in ns
  float tRASmin; // Minimum Row Active time in ns
  float tRCmin;  // Minimum Row Cycle time in ns
  float tRFCmin; // Refresh Recovery time in ns
  float tWTRmin; // Write to Read delay in ns
  float tRTPmin; // Read to Precharge delay in ns
  float tFAWmin; // Four Activate Window time in ns

  TimingDDR3(const std::vector<uint8_t>& spdData) {
    if (spdData.size() < 176) {
      throw std::invalid_argument("SPD data too short for DDR3 timing extraction");
    }

    // Calculate MTB (Medium Timebase) from bytes 10 and 11
    if (spdData[10] == 0x01) {
      if (spdData[11] == 0x08) {
        MTB = 0.125; // 0.125ns
      } else if (spdData[11] == 0x0F) {
        MTB = 0.0625; // 0.0625ns
      } else {
        MTB = -1.0; // Invalid
      }
    } else {
      MTB = -1.0; // Invalid
    }
    if (MTB < 0.00) {throw std::invalid_argument("Invalid MTB value in SPD data");}

    // Byte 12: Minimum SDRAM Cycle Time (tCKmin)
    tCKmin = MTB * spdData[12];

    // Byte 16: Minimum CAS Latency Time (tAAmin)
    switch (spdData[16]) {
    case 0x64: // 12.5ns - DDR3-800D
      tAAmin = 12.5;
      break;
    case 0x78: // 15ns - DDR3-800E, DDR3-1066G, DDR3-1333J
      tAAmin = 15.0;
      break;
    case 0x5A: // 11.25ns - DDR3-1066E, DDR3-1600H
      tAAmin = 11.25;
      break;
    case 0x69: // 13.125ns - DDR3-1066F
      tAAmin = 13.125;
      break;
    case 0x54: // 10.5ns - DDR3-1333F
      tAAmin = 10.5;
      break;
    case 0x60: // 12ns - DDR3-1333G
      tAAmin = 12.0;
      break;
    case 0x6C: // 13.5ns - DDR3-1333H
      tAAmin = 13.5;
      break;
    case 0x50: // 10ns - DDR3-1600G
      tAAmin = 10.0;
      break;
    case 0x6E: // 13.75ns - DDR3-1600K
      tAAmin = 13.75;
      break;
    default:
      tAAmin = MTB * spdData[16]; // Calculate from MTB if not in standard values
      break;
    }

    // Byte 17: Minimum Write Recovery Time (tWRmin)
    switch (spdData[17]) {
    case 0x78: // 15ns
      tWRmin = 15.0;
      break;
    case 0x60: // 12ns
      tWRmin = 12.0;
      break;
    case 0x80: // 16ns
      tWRmin = 16.0;
      break;
    default:
      tWRmin = MTB * spdData[17];
      break;
    }

    // Byte 18: Minimum RAS# to CAS# Delay time (tRCDmin)
    switch (spdData[18]) {
    case 0x64: // 12.5ns - DDR3-800D
      tRCDmin = 12.5;
      break;
    case 0x78: // 15ns - DDR3-800E, DDR3-1066G, DDR3-1333J
      tRCDmin = 15.0;
      break;
    case 0x5A: // 11.25ns - DDR3-1066E, DDR3-1600H
      tRCDmin = 11.25;
      break;
    case 0x69: // 13.125ns - DDR3-1066F
      tRCDmin = 13.125;
      break;
    case 0x54: // 10.5ns - DDR3-1333F
      tRCDmin = 10.5;
      break;
    case 0x60: // 12ns - DDR3-1333G
      tRCDmin = 12.0;
      break;
    case 0x50: // 10ns - DDR3-1600G
      tRCDmin = 10.0;
      break;
    case 0x6E: // 13.75ns - DDR3-1600K
      tRCDmin = 13.75;
      break;
    default:
      tRCDmin = MTB * spdData[18];
      break;
    }

    // Byte 19: Minimum Row Active to Row Active Delay time (tRRDmin)
    switch (spdData[19]) {
    case 0x30: // 6.0ns
      tRRDmin = 6.0;
      break;
    case 0x3C: // 7.5ns
      tRRDmin = 7.5;
      break;
    case 0x50: // 10ns
      tRRDmin = 10.0;
      break;
    default:
      tRRDmin = MTB * spdData[19];
      break;
    }

    // Byte 20: Minimum Row Precharge Delay Time (tRPmin)
    switch (spdData[20]) {
    case 0x64: // 12.5ns - DDR3-800D
      tRPmin = 12.5;
      break;
    case 0x78: // 15ns - DDR3-800E, DDR3-1066G, DDR3-1333J
      tRPmin = 15.0;
      break;
    case 0x69: // 13.125ns - DDR3-1066F
      tRPmin = 13.125;
      break;
    case 0x54: // 10.5ns - DDR3-1333F
      tRPmin = 10.5;
      break;
    case 0x60: // 12ns - DDR3-1333G
      tRPmin = 12.0;
      break;
    case 0x6C: // 13.5ns - DDR3-1333H
      tRPmin = 13.5;
      break;
    case 0x50: // 10ns - DDR3-1600G
      tRPmin = 10.0;
      break;
    case 0x5A: // 11.25ns - DDR3-1600H
      tRPmin = 11.25;
      break;
    case 0x6E: // 13.75ns - DDR3-1600K
      tRPmin = 13.75;
      break;
    default:
      tRPmin = MTB * spdData[20];
      break;
    }

    // Byte 22: Minimum Active to Precharge Delay Time (tRASmin)
    // Combined with upper nibble from byte 21 (bits 3-0)
    uint16_t tRAS_raw = ((spdData[21] & 0x0F) << 8) | spdData[22];
    switch (spdData[22]) {
    case 0x2C: // 37.5ns - DDR3-800D/E, DDR3-1066E/F/G
      tRASmin = 37.5;
      break;
    case 0x20: // 36ns - DDR3-1333F/G/H/J
      tRASmin = 36.0;
      break;
    case 0x18: // 35ns - DDR3-1600G/H/J/K
      tRASmin = 35.0;
      break;
    default:
      tRASmin = MTB * tRAS_raw;
      break;
    }

    // Byte 23: Minimum Active to Active Refresh Delay Time (tRCmin)
    // Combined with upper nibble from byte 21 (bits 7-4)
    uint16_t tRC_raw = (((spdData[21] & 0xF0) >> 4) << 8) | spdData[23];
    switch (spdData[23]) {
    case 0x90: // 50ns - DDR3-800D
      tRCmin = 50.0;
      break;
    case 0xA4: // 52.5ns - DDR3-800E, DDR3-1066G
      tRCmin = 52.5;
      break;
    case 0x86: // 48.75ns - DDR3-1066E
      tRCmin = 48.75;
      break;
    case 0x95: // 50.625ns - DDR3-1066F
      tRCmin = 50.625;
      break;
    case 0x74: // 46.5ns - DDR3-1333F
      tRCmin = 46.5;
      break;
    case 0x80: // 48ns - DDR3-1333G
      tRCmin = 48.0;
      break;
    case 0x8C: // 49.5ns - DDR3-1333H
      tRCmin = 49.5;
      break;
    case 0x98: // 51ns - DDR3-1333J
      tRCmin = 51.0;
      break;
    case 0x68: // 45ns - DDR3-1600G
      tRCmin = 45.0;
      break;
    case 0x72: // 46.25ns - DDR3-1600H
      tRCmin = 46.25;
      break;
    case 0x7C: // 47.5ns - DDR3-1600J
      tRCmin = 47.5;
      break;
    default:
      tRCmin = MTB * tRC_raw;
      break;
    }

    // Bytes 24-25: Minimum Refresh Recovery Delay Time (tRFCmin)
    uint16_t tRFC_raw = (spdData[25] << 8) | spdData[24];
    if (spdData[25] == 0x02 && spdData[24] == 0xD0) {
      tRFCmin = 90.0; // 512Mb chip
    } else if (spdData[25] == 0x03 && spdData[24] == 0x70) {
      tRFCmin = 110.0; // 1Gb chip
    } else if (spdData[25] == 0x05 && spdData[24] == 0x00) {
      tRFCmin = 160.0; // 2Gb chip
    } else {
      tRFCmin = MTB * tRFC_raw;
    }

    // Byte 26: Minimum Internal Write to Read Command Delay time (tWTRmin)
    switch (spdData[26]) {
    case 0x3C: // 7.5ns - standard for all DDR3
      tWTRmin = 7.5;
      break;
    default:
      tWTRmin = MTB * spdData[26];
      break;
    }

    // Byte 27: Minimum Internal Read to Precharge Command Delay time (tRTPmin)
    switch (spdData[27]) {
    case 0x3C: // 7.5ns - standard for all DDR3
      tRTPmin = 7.5;
      break;
    default:
      tRTPmin = MTB * spdData[27];
      break;
    }

    // Bytes 28-29: Minimum Four Activate Window Delay Time (tFAWmin)
    // Combined with upper nibble from byte 28 (bits 3-0)
    uint16_t tFAW_raw = ((spdData[28] & 0x0F) << 8) | spdData[29];
    switch (spdData[29]) {
    case 0x40: // 40.0ns - DDR3-800 1K page, DDR3-1600 2K page
      tFAWmin = 40.0;
      break;
    case 0x90: // 50.0ns - DDR3-800 2K page, DDR3-1066 2K page
      tFAWmin = 50.0;
      break;
    case 0x2C: // 37.5ns - DDR3-1066 1K page
      tFAWmin = 37.5;
      break;
    case 0xF0: // 30.0ns - DDR3-1333 1K page, DDR3-1600 1K page
      tFAWmin = 30.0;
      break;
    case 0x68: // 45.0ns - DDR3-1333 2K page
      tFAWmin = 45.0;
      break;
    default:
      tFAWmin = MTB * tFAW_raw;
      break;
    }
  }
};

struct TimingDDR4 {
  float MTB;      // Medium Timebase in ns (always 0.125ns for DDR4)
  float FTB;      // Fine Timebase in ps (always 1ps for DDR4)
  float tCKmin;   // Clock cycle time in ns
  float tCKmax;   // Maximum clock cycle time in ns
  float tAAmin;   // CAS Latency in ns
  float tRCDmin;  // RAS to CAS Delay in ns
  float tRPmin;   // Row Precharge time in ns
  float tRASmin;  // Minimum Row Active time in ns
  float tRCmin;   // Minimum Row Cycle time in ns
  float tRFC1min; // Refresh Recovery time (normal) in ns
  float tRFC2min; // Refresh Recovery time (2x) in ns
  float tRFC4min; // Refresh Recovery time (4x) in ns
  float tFAWmin;  // Four Activate Window time in ns
  float tRRD_Smin; // Row to Row Delay (different bank group) in ns
  float tRRD_Lmin; // Row to Row Delay (same bank group) in ns
  float tCCD_Lmin; // CAS to CAS Delay (same bank group) in ns
  
  // Fine timing corrections (applied as ps corrections)
  float tAAmin_fine;
  float tRCDmin_fine;
  float tRPmin_fine;
  float tRCmin_fine;
  float tRRD_Smin_fine;
  float tRRD_Lmin_fine;
  float tCCD_Lmin_fine;
  float tCKmin_fine;
  float tCKmax_fine;

  TimingDDR4(const std::vector<uint8_t>& spdData) {
    if (spdData.size() < 384) {
      throw std::invalid_argument("SPD data too short for DDR4 timing extraction");
    }

    // Verify this is DDR4 (Byte 2 should be 0x0C)
    if (spdData[2] != 0x0C) {
      throw std::invalid_argument("SPD data is not DDR4");
    }

    // Byte 17: Time bases - DDR4 uses fixed values
    MTB = 0.125; // 125ps = 0.125ns
    FTB = 0.001; // 1ps = 0.001ns

    // Byte 18: SDRAM Minimum Cycle Time (tCKmin)
    switch (spdData[18]) {
    case 0x0A: // DDR4-1600 (1250ps)
      tCKmin = 1.250;
      break;
    case 0x09: // DDR4-1866 (1071ps)
      tCKmin = 1.071;
      break;
    case 0x08: // DDR4-2133 (938ps)
      tCKmin = 0.938;
      break;
    case 0x07: // DDR4-2400 (833ps)
      tCKmin = 0.833;
      break;
    case 0x06: // DDR4-2666 (750ps)
      tCKmin = 0.750;
      break;
    case 0x05: // DDR4-3200 (625ps)
      tCKmin = 0.625;
      break;
    default:
      tCKmin = MTB * spdData[18];
      break;
    }

    // Byte 19: SDRAM Maximum Cycle Time (tCKmax)
    switch (spdData[19]) {
    case 0x0C: // DDR4-1600/1866/2133/2400 (1500ps)
      tCKmax = 1.500;
      break;
    default:
      tCKmax = MTB * spdData[19];
      break;
    }

    // Byte 24: Minimum CAS Latency Time (tAAmin)
    switch (spdData[24]) {
    case 0x64: // 12.5ns - DDR4-1600J, DDR4-2133P downbin
      tAAmin = 12.5;
      break;
    case 0x6E: // 13.75ns - DDR4-1600K
      tAAmin = 13.75;
      break;
    case 0x6C: // 13.5ns - DDR4-1600K downbin, DDR4-1866M downbin
      tAAmin = 13.5;
      break;
    case 0x78: // 15.0ns - DDR4-1600JL, DDR4-1866N, DDR4-2133R, DDR4-2400U
      tAAmin = 15.0;
      break;
    case 0x67: // 12.85ns - DDR4-1866L
      tAAmin = 12.85;
      break;
    case 0x70: // 13.92ns - DDR4-1866M
      tAAmin = 13.92;
      break;
    case 0x6A: // 13.13ns - DDR4-2133N
      tAAmin = 13.13;
      break;
    case 0x71: // 14.06ns - DDR4-2133P
      tAAmin = 14.06;
      break;
    case 0x6B: // 13.32ns - DDR4-2400R
      tAAmin = 13.32;
      break;
    default:
      tAAmin = MTB * spdData[24];
      break;
    }

    // Byte 25: Minimum RAS to CAS Delay Time (tRCDmin) - Same values as tAAmin
    switch (spdData[25]) {
    case 0x64:
      tRCDmin = 12.5;
      break;
    case 0x6E:
      tRCDmin = 13.75;
      break;
    case 0x6C:
      tRCDmin = 13.5;
      break;
    case 0x78:
      tRCDmin = 15.0;
      break;
    case 0x67:
      tRCDmin = 12.85;
      break;
    case 0x70:
      tRCDmin = 13.92;
      break;
    case 0x6A:
      tRCDmin = 13.13;
      break;
    case 0x71:
      tRCDmin = 14.06;
      break;
    case 0x6B:
      tRCDmin = 13.32;
      break;
    default:
      tRCDmin = MTB * spdData[25];
      break;
    }

    // Byte 26: Minimum Row Precharge Delay Time (tRPmin) - Same values as tAAmin
    switch (spdData[26]) {
    case 0x64:
      tRPmin = 12.5;
      break;
    case 0x6E:
      tRPmin = 13.75;
      break;
    case 0x6C:
      tRPmin = 13.5;
      break;
    case 0x78:
      tRPmin = 15.0;
      break;
    case 0x67:
      tRPmin = 12.85;
      break;
    case 0x70:
      tRPmin = 13.92;
      break;
    case 0x6A:
      tRPmin = 13.13;
      break;
    case 0x71:
      tRPmin = 14.06;
      break;
    case 0x6B:
      tRPmin = 13.32;
      break;
    default:
      tRPmin = MTB * spdData[26];
      break;
    }

    // Byte 28: Minimum Active to Precharge Delay Time (tRASmin), LSB
    // Combined with lower nibble of byte 27
    uint16_t tRAS_raw = ((spdData[27] & 0x0F) << 8) | spdData[28];
    switch (spdData[28]) {
    case 0x18: // 35ns - DDR4-1600
      tRASmin = 35.0;
      break;
    case 0x10: // 34ns - DDR4-1866
      tRASmin = 34.0;
      break;
    case 0x08: // 33ns - DDR4-2133
      tRASmin = 33.0;
      break;
    case 0x00: // 32ns - DDR4-2400
      tRASmin = 32.0;
      break;
    default:
      tRASmin = MTB * tRAS_raw;
      break;
    }

    // Byte 29: Minimum Active to Active/Refresh Delay Time (tRCmin), LSB
    // Combined with upper nibble of byte 27
    uint16_t tRC_raw = ((spdData[27] & 0xF0) << 4) | spdData[29];
    switch (spdData[29]) {
    case 0x7C: // 47.5ns - DDR4-1600J
      tRCmin = 47.5;
      break;
    case 0x86: // 48.75ns - DDR4-1600K
      tRCmin = 48.75;
      break;
    case 0x84: // 48.5ns - DDR4-1600K downbin
      tRCmin = 48.5;
      break;
    case 0x90: // 50ns - DDR4-1600L
      tRCmin = 50.0;
      break;
    case 0x77: // 46.85ns - DDR4-1866L
      tRCmin = 46.85;
      break;
    case 0x80: // 47.92ns - DDR4-1866M
      tRCmin = 47.92;
      break;
    case 0x88: // 49ns - DDR4-1866N
      tRCmin = 49.0;
      break;
    case 0x72: // 46.13ns - DDR4-2133N
      tRCmin = 46.13;
      break;
    case 0x79: // 47.06ns - DDR4-2133P
      tRCmin = 47.06;
      break;
    case 0x74: // 46.50ns - DDR4-2133P downbin
      tRCmin = 46.50;
      break;
    case 0x64: // 44.50ns - DDR4-2400P
      tRCmin = 44.50;
      break;
    case 0x6B: // 45.32ns - DDR4-2400R
      tRCmin = 45.32;
      break;
    case 0x78: // 47.00ns - DDR4-2400U
      tRCmin = 47.00;
      break;
    default:
      tRCmin = MTB * tRC_raw;
      break;
    }

    // Bytes 30-31: Minimum Refresh Recovery Delay time (tRFC1min)
    uint16_t tRFC1_raw = (spdData[31] << 8) | spdData[30];
    if (spdData[31] == 0x08 && spdData[30] == 0x20) {
      tRFC1min = 260.0; // 4Gb DDR4
    } else if (spdData[31] == 0x0A && spdData[30] == 0xF0) {
      tRFC1min = 350.0; // 8Gb DDR4
    } else {
      tRFC1min = MTB * tRFC1_raw;
    }

    // Bytes 32-33: Minimum Refresh Recovery Delay time (tRFC2min)
    uint16_t tRFC2_raw = (spdData[33] << 8) | spdData[32];
    if (spdData[33] == 0x05 && spdData[32] == 0x00) {
      tRFC2min = 160.0; // 4Gb DDR4
    } else if (spdData[33] == 0x08 && spdData[32] == 0x20) {
      tRFC2min = 260.0; // 8Gb DDR4
    } else {
      tRFC2min = MTB * tRFC2_raw;
    }

    // Bytes 34-35: Minimum Refresh Recovery Delay time (tRFC4min)
    uint16_t tRFC4_raw = (spdData[35] << 8) | spdData[34];
    if (spdData[35] == 0x03 && spdData[34] == 0x70) {
      tRFC4min = 110.0; // 4Gb DDR4
    } else if (spdData[35] == 0x05 && spdData[34] == 0x00) {
      tRFC4min = 160.0; // 8Gb DDR4
    } else {
      tRFC4min = MTB * tRFC4_raw;
    }

    // Bytes 36-37: Minimum Four Activate Window Delay Time (tFAWmin)
    uint16_t tFAW_raw = ((spdData[36] & 0x0F) << 8) | spdData[37];
    tFAWmin = MTB * tFAW_raw;

    // Byte 38: Minimum Activate to Activate Delay Time (tRRD_Smin), different bank group
    tRRD_Smin = MTB * spdData[38];

    // Byte 39: Minimum Activate to Activate Delay Time (tRRD_Lmin), same bank group
    tRRD_Lmin = MTB * spdData[39];

    // Byte 40: Minimum CAS to CAS Delay Time (tCCD_Lmin), same bank group
    tCCD_Lmin = MTB * spdData[40];

    // Fine timing corrections (bytes 117-125)
    // Convert two's complement signed bytes to corrections in ps
    auto convertFineOffset = [](uint8_t value) -> float {
      return (value & 0x80) ? -(256 - value) * 0.001f : value * 0.001f;
    };

    tCCD_Lmin_fine = convertFineOffset(spdData[117]);
    tRRD_Lmin_fine = convertFineOffset(spdData[118]);
    tRRD_Smin_fine = convertFineOffset(spdData[119]);
    tRCmin_fine = convertFineOffset(spdData[120]);
    tRPmin_fine = convertFineOffset(spdData[121]);
    tRCDmin_fine = convertFineOffset(spdData[122]);
    tAAmin_fine = convertFineOffset(spdData[123]);
    tCKmax_fine = convertFineOffset(spdData[124]);
    tCKmin_fine = convertFineOffset(spdData[125]);

    // Apply fine corrections
    tCCD_Lmin += tCCD_Lmin_fine;
    tRRD_Lmin += tRRD_Lmin_fine;
    tRRD_Smin += tRRD_Smin_fine;
    tRCmin += tRCmin_fine;
    tRPmin += tRPmin_fine;
    tRCDmin += tRCDmin_fine;
    tAAmin += tAAmin_fine;
    tCKmax += tCKmax_fine;
    tCKmin += tCKmin_fine;
  }
};
