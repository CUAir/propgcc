/* gmp-mparam.h -- Compiler/machine parameter header file.

Copyright 1991, 1993, 1994, 1999, 2000, 2001, 2002, 2004, 2005, 2008, 2009,
2010 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#define GMP_LIMB_BITS 64
#define BYTES_PER_MP_LIMB 8

#define DIVEXACT_BY3_METHOD 0	/* override ../diveby3.asm */

/* 500 MHz 21164 (agnesi.math.su.se) */

/* Generated by tuneup.c, 2009-11-29, gcc 3.3 */

#define DIVREM_1_NORM_THRESHOLD              0  /* preinv always */
#define DIVREM_1_UNNORM_THRESHOLD            0  /* always */
#define MOD_1_NORM_THRESHOLD                 0  /* always */
#define MOD_1_UNNORM_THRESHOLD               0  /* always */
#define MOD_1N_TO_MOD_1_1_THRESHOLD          4
#define MOD_1U_TO_MOD_1_1_THRESHOLD          2
#define MOD_1_1_TO_MOD_1_2_THRESHOLD         6
#define MOD_1_2_TO_MOD_1_4_THRESHOLD        30
#define PREINV_MOD_1_TO_MOD_1_THRESHOLD     10
#define USE_PREINV_DIVREM_1                  1  /* preinv always */
#define DIVEXACT_1_THRESHOLD                 0  /* always */
#define BMOD_1_TO_MOD_1_THRESHOLD           16

#define MUL_TOOM22_THRESHOLD                35
#define MUL_TOOM33_THRESHOLD                74
#define MUL_TOOM44_THRESHOLD               178
#define MUL_TOOM6H_THRESHOLD               288
#define MUL_TOOM8H_THRESHOLD               333

#define MUL_TOOM32_TO_TOOM43_THRESHOLD      75
#define MUL_TOOM32_TO_TOOM53_THRESHOLD     101
#define MUL_TOOM42_TO_TOOM53_THRESHOLD     105
#define MUL_TOOM42_TO_TOOM63_THRESHOLD     105

#define SQR_BASECASE_THRESHOLD               5
#define SQR_TOOM2_THRESHOLD                 61
#define SQR_TOOM3_THRESHOLD                107
#define SQR_TOOM4_THRESHOLD                170
#define SQR_TOOM6_THRESHOLD                309
#define SQR_TOOM8_THRESHOLD                360

#define MULMOD_BNM1_THRESHOLD               20
#define SQRMOD_BNM1_THRESHOLD               23

#define MUL_FFT_MODF_THRESHOLD             480  /* k = 5 */
#define MUL_FFT_TABLE3                                      \
  { {    480, 5}, {     18, 6}, {     10, 5}, {     21, 6}, \
    {     11, 5}, {     23, 6}, {     12, 5}, {     25, 6}, \
    {     19, 7}, {     10, 6}, {     25, 7}, {     13, 6}, \
    {     27, 7}, {     14, 6}, {     29, 7}, {     25, 8}, \
    {     13, 7}, {     29, 8}, {     15, 7}, {     32, 8}, \
    {     17, 7}, {     35, 8}, {     19, 7}, {     39, 8}, \
    {     29, 9}, {     15, 8}, {     37, 9}, {     19, 8}, \
    {     41, 9}, {     23, 8}, {     51, 9}, {     27, 8}, \
    {     55, 9}, {     31, 8}, {     63, 9}, {     35, 8}, \
    {     71, 9}, {     39,10}, {     23, 9}, {     55,10}, \
    {     31, 9}, {     67,10}, {     39, 9}, {     83,10}, \
    {     47, 9}, {     99,10}, {     55,11}, {     31,10}, \
    {     79,11}, {     47,10}, {    103,12}, {     31,11}, \
    {     63,10}, {    135,11}, {     79,10}, {    167,11}, \
    {     95,10}, {    191,11}, {    111,12}, {     63,11}, \
    {    127,10}, {    255,11}, {    143,10}, {    287, 9}, \
    {    575,11}, {    159,10}, {    319,12}, {     95,11}, \
    {    191,10}, {    383,11}, {    207,13}, {     63,12}, \
    {    127,11}, {    255,10}, {    511,11}, {    271,10}, \
    {    543,11}, {    287,10}, {    575,12}, {    159,11}, \
    {    319,10}, {    639,11}, {    351,10}, {    703,12}, \
    {    191,11}, {    383,10}, {    767,11}, {    415,10}, \
    {    831,11}, {    447,13}, {    127,12}, {    255,11}, \
    {    543,12}, {    287,11}, {    575,10}, {   1151,12}, \
    {    319,11}, {    639,12}, {    351,11}, {    703,13}, \
    {    191,12}, {    383,11}, {    767,12}, {    415,11}, \
    {    831,12}, {    447,11}, {    895,14}, {    127,13}, \
    {    255,12}, {    543,11}, {   1087,12}, {    575,11}, \
    {   1151,12}, {    607,13}, {    319,12}, {    671,11}, \
    {   1343,12}, {    703,13}, {    383,12}, {    831,13}, \
    {    447,12}, {    927,14}, {    255,13}, {    511,12}, \
    {   1087,13}, {    575,12}, {   1151,13}, {    639,12}, \
    {   1279,13}, {    703,12}, {   1407,14}, {    383,13}, \
    {    767,15}, {    255,14}, {    511,13}, {   1215,14}, \
    {    639,13}, {   1407,14}, {    767,13}, {   1663,14}, \
    {    895,13}, {   1791,15}, {  32768,16}, {  65536,17}, \
    { 131072,18}, { 262144,19}, { 524288,20}, {1048576,21}, \
    {2097152,22}, {4194304,23}, {8388608,24} }
#define MUL_FFT_TABLE3_SIZE 151
#define MUL_FFT_THRESHOLD                 5760

#define SQR_FFT_MODF_THRESHOLD             476  /* k = 5 */
#define SQR_FFT_TABLE3                                      \
  { {    476, 5}, {     19, 6}, {     10, 5}, {     23, 6}, \
    {     12, 5}, {     25, 6}, {     27, 7}, {     14, 6}, \
    {     29, 7}, {     28, 8}, {     15, 7}, {     31, 8}, \
    {     29, 9}, {     15, 8}, {     35, 9}, {     19, 8}, \
    {     41, 9}, {     23, 8}, {     49, 9}, {     27,10}, \
    {     15, 9}, {     35, 8}, {     71, 9}, {     39,10}, \
    {     23, 9}, {     51,11}, {     15,10}, {     31, 9}, \
    {     67,10}, {     39, 9}, {     79,10}, {     47, 9}, \
    {     95,10}, {     55,11}, {     31,10}, {     79,11}, \
    {     47,10}, {    103,12}, {     31,11}, {     63,10}, \
    {    135,11}, {     79,10}, {    159, 9}, {    319,11}, \
    {     95,10}, {    191, 9}, {    383,11}, {    111,12}, \
    {     63,11}, {    127,10}, {    255, 9}, {    511,10}, \
    {    271,11}, {    143,10}, {    287, 9}, {    575,10}, \
    {    303,11}, {    159,10}, {    319,12}, {     95,11}, \
    {    191,10}, {    383, 9}, {    767,13}, {     63,12}, \
    {    127,11}, {    255,10}, {    511,11}, {    271,10}, \
    {    543,11}, {    287,10}, {    575,11}, {    303,12}, \
    {    159,11}, {    319,10}, {    639,11}, {    335,10}, \
    {    671,11}, {    351,10}, {    703,11}, {    367,10}, \
    {    735,12}, {    191,11}, {    383,10}, {    767,11}, \
    {    415,10}, {    831,11}, {    447,10}, {    895,13}, \
    {    127,12}, {    255,11}, {    543,12}, {    287,11}, \
    {    575,10}, {   1151,11}, {    607,12}, {    319,11}, \
    {    671,12}, {    351,11}, {    735,13}, {    191,12}, \
    {    383,11}, {    767,12}, {    415,11}, {    831,12}, \
    {    447,11}, {    895,12}, {    479,14}, {    127,13}, \
    {    255,12}, {    575,11}, {   1151,12}, {    607,13}, \
    {    319,12}, {    735,13}, {    383,12}, {    831,13}, \
    {    447,12}, {    959,14}, {    255,13}, {    511,12}, \
    {   1023,13}, {    575,12}, {   1215,13}, {    639,12}, \
    {   1279,13}, {    703,12}, {   1407,14}, {    383,13}, \
    {    767,12}, {   1535,13}, {    831,12}, {   1663,13}, \
    {    895,12}, {   1791,15}, {    255,14}, {    511,13}, \
    {   1215,14}, {    639,13}, {   1407,14}, {    767,13}, \
    {   1663,14}, {    895,13}, {   1791,15}, {  32768,16}, \
    {  65536,17}, { 131072,18}, { 262144,19}, { 524288,20}, \
    {1048576,21}, {2097152,22}, {4194304,23}, {8388608,24} }
#define SQR_FFT_TABLE3_SIZE 152
#define SQR_FFT_THRESHOLD                 3136

#define MULLO_BASECASE_THRESHOLD             0  /* always */
#define MULLO_DC_THRESHOLD                 130
#define MULLO_MUL_N_THRESHOLD            15604

#define DC_DIV_QR_THRESHOLD                119
#define DC_DIVAPPR_Q_THRESHOLD             390
#define DC_BDIV_QR_THRESHOLD               110
#define DC_BDIV_Q_THRESHOLD                318

#define INV_MULMOD_BNM1_THRESHOLD          109
#define INV_NEWTON_THRESHOLD               390
#define INV_APPR_THRESHOLD                 372

#define BINV_NEWTON_THRESHOLD              393
#define REDC_1_TO_REDC_N_THRESHOLD         110

#define MU_DIV_QR_THRESHOLD               1718
#define MU_DIVAPPR_Q_THRESHOLD            1895
#define MUPI_DIV_QR_THRESHOLD              180
#define MU_BDIV_QR_THRESHOLD              1387
#define MU_BDIV_Q_THRESHOLD               1652

#define MATRIX22_STRASSEN_THRESHOLD         17
#define HGCD_THRESHOLD                     282
#define GCD_DC_THRESHOLD                  1138
#define GCDEXT_DC_THRESHOLD                773
#define JACOBI_BASE_METHOD                   3

#define GET_STR_DC_THRESHOLD                14
#define GET_STR_PRECOMPUTE_THRESHOLD        19
#define SET_STR_DC_THRESHOLD              3754
#define SET_STR_PRECOMPUTE_THRESHOLD      8097
