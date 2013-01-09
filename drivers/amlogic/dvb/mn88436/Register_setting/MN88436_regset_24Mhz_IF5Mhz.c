/* **************************************************** */
/*!
   @file	MN88436_reg.c
   @brief	Reg. settings of MN88436 for 24MHz IF 5MHz
   @author	Simon
   @date	2012/7/24
   @param
		(c)	Panasonic
   */
/* **************************************************** */

#include "MN_DMD_driver.h"
#include "MN88436_reg.h"
DMD_u32_t DMD_RegSet_Rev = 1;


// Mode : ATSC
DMD_u8_t DMD_REG_ATSC[]={
0		,0x0		,0x50		,
0		,0x1		,0x0		,
0		,0x2		,0xC0		,
0		,0x3		,0x0		,
0		,0x4		,0x0		,
0		,0x5		,0x0		,
0		,0x6		,0x0		,
0		,0x7		,0x0		,
0		,0x8		,0x0		,
0		,0x9		,0x0		,
0		,0xA		,0x1E		,
0		,0xB		,0x35		,
0		,0xC		,0x56		,
0		,0xD		,0x8E		,
0		,0xE		,0xB9		,
0		,0xF		,0x72		,
0		,0x10		,0x0		,
0		,0x11		,0x14		,
0		,0x12		,0xE3		,
0		,0x13		,0x4E		,
0		,0x14		,0x8		,
0		,0x15		,0x5		,
0		,0x16		,0x63		,
0		,0x17		,0x0		,
0		,0x18		,0x0		,
0		,0x19		,0x0		,
0		,0x1A		,0x0		,
0		,0x1B		,0x0		,
0		,0x1C		,0x0		,
0		,0x1D		,0x0		,
0		,0x1E		,0x0		,
0		,0x1F		,0x0		,
0		,0x20		,0x0		,
0		,0x21		,0x50		,
0		,0x22		,0x60		,
0		,0x23		,0x36		,
0		,0x24		,0x2		,
0		,0x25		,0x0		,
0		,0x26		,0x0		,
0		,0x27		,0x0		,
0		,0x28		,0x0		,
0		,0x29		,0x0		,
0		,0x2A		,0x20		,
0		,0x2B		,0xC0		,
0		,0x2C		,0x23		,
0		,0x2D		,0x3F		,
0		,0x2E		,0x3F		,
0		,0x2F		,0x0		,
0		,0x30		,0x2E		,
0		,0x31		,0x0		,
0		,0x32		,0x0		,
0		,0x33		,0x81		,
0		,0x34		,0x0		,
0		,0x35		,0x26		,
0		,0x36		,0x21		,
0		,0x37		,0x88		,
0		,0x38		,0x3		,
0		,0x39		,0x19		,
0		,0x3A		,0x85		,
0		,0x3B		,0x5		,
0		,0x3C		,0xC9		,
0		,0x3D		,0x2		,
0		,0x3E		,0x30		,
0		,0x3F		,0x69		,
0		,0x40		,0x1F		,
0		,0x41		,0xF0		,
0		,0x42		,0x0		,
0		,0x43		,0x96		,
0		,0x44		,0x72		,
0		,0x45		,0x1B		,
0		,0x46		,0x2D		,
0		,0x47		,0x1A		,
0		,0x48		,0x31		,
0		,0x49		,0xFE		,
0		,0x4A		,0x96		,
0		,0x4B		,0x7		,
0		,0x4C		,0x26		,
0		,0x4D		,0xE3		,
0		,0x4E		,0x22		,
0		,0x4F		,0x1B		,
0		,0x50		,0x26		,
0		,0x51		,0x12		,
0		,0x52		,0x40		,
0		,0x53		,0x50		,
0		,0x54		,0x60		,
0		,0x55		,0x40		,
0		,0x56		,0xED		,
0		,0x57		,0x96		,
0		,0x58		,0x4A		,
0		,0x59		,0xDE		,
0		,0x5A		,0x88		,
0		,0x5B		,0xC8		,
0		,0x5C		,0x2D		,
0		,0x5D		,0x1		,
0		,0x5E		,0x0		,
0		,0x5F		,0xB8		,
0		,0x60		,0x40		,
0		,0x61		,0x14		,
0		,0x62		,0x13		,
0		,0x63		,0x30		,
0		,0x64		,0x59		,
0		,0x65		,0x7		,
0		,0x66		,0xF0		,
0		,0x67		,0x47		,
0		,0x68		,0xA1		,
0		,0x69		,0x80		,
0		,0x6A		,0x0		,
0		,0x6B		,0x0		,
0		,0x6C		,0xC0		,
0		,0x6D		,0x0		,
0		,0x6E		,0x0		,
0		,0x6F		,0x0		,
0		,0x70		,0x0		,
0		,0x71		,0x0		,
0		,0x72		,0x0		,
0		,0x73		,0x0		,
0		,0x74		,0x2F		,
0		,0x75		,0x0		,
0		,0x76		,0x0		,
0		,0x77		,0x0		,
0		,0x78		,0x0		,
0		,0x79		,0x40		,
0		,0x7A		,0x20		,
0		,0x7B		,0x40		,
0		,0x7C		,0x80		,
0		,0x7D		,0xFF		,
0		,0x7E		,0x20		,
0		,0x7F		,0x40		,
0		,0x80		,0x10		,
0		,0x81		,0x0		,
0		,0x82		,0xE5		,
0		,0x83		,0xC4		,
0		,0x84		,0xE9		,
0		,0x85		,0x52		,
0		,0x86		,0x5F		,
0		,0x87		,0x53		,
0		,0x88		,0x20		,
0		,0x89		,0x43		,
0		,0x8A		,0x11		,
0		,0x8B		,0x8		,
0		,0x8C		,0x43		,
0		,0x8D		,0x11		,
0		,0x8E		,0x8		,
0		,0x8F		,0x0		,
0		,0x90		,0x60		,
0		,0x91		,0x5B		,
0		,0x92		,0x80		,
0		,0x93		,0xE4		,
0		,0x94		,0x34		,
0		,0x95		,0x0		,
0		,0x96		,0x0		,
0		,0x97		,0x7		,
0		,0x98		,0x0		,
0		,0x99		,0xB0		,
0		,0x9A		,0x68		,
0		,0x9B		,0x50		,
0		,0x9C		,0x84		,
0		,0x9D		,0xA9		,
0		,0x9E		,0x11		,
0		,0x9F		,0x4		,
0		,0xA0		,0x44		,
0		,0xA1		,0x14		,
0		,0xA2		,0x9C		,
0		,0xA3		,0x15		,
0		,0xA4		,0xF6		,
0		,0xA5		,0x36		,
0		,0xA6		,0x16		,
0		,0xA7		,0x3F		,
0		,0xA8		,0x1		,
0		,0xA9		,0x8		,
0		,0xAA		,0x0		,
0		,0xAB		,0x0		,
0		,0xAC		,0x0		,
0		,0xAD		,0x65		,
0		,0xAE		,0x87		,
0		,0xAF		,0x56		,
0		,0xB0		,0x52		,
0		,0xB1		,0x55		,
0		,0xB2		,0x35		,
0		,0xB3		,0x13		,
0		,0xB4		,0x88		,
0		,0xB5		,0x23		,
0		,0xB6		,0x28		,
0		,0xB7		,0x3C		,
0		,0xB8		,0x1		,
0		,0xB9		,0x5B		,
0		,0xBA		,0x0		,
0		,0xBB		,0x0		,
0		,0xBC		,0x0		,
0		,0xBD		,0x0		,
0		,0xBE		,0x0		,
0		,0xBF		,0x0		,
0		,0xC0		,0x0		,
0		,0xC1		,0x0		,
0		,0xC2		,0x0		,
0		,0xC3		,0x0		,
0		,0xC4		,0x0		,
0		,0xC5		,0x0		,
0		,0xC6		,0x0		,
0		,0xC7		,0x0		,
0		,0xC8		,0x0		,
0		,0xC9		,0x0		,
0		,0xCA		,0x0		,
0		,0xCB		,0x0		,
0		,0xCC		,0x0		,
0		,0xCD		,0x0		,
0		,0xCE		,0x0		,
0		,0xCF		,0x0		,
0		,0xD0		,0x0		,
0		,0xD1		,0x0		,
0		,0xD2		,0x0		,
0		,0xD3		,0x0		,
0		,0xD4		,0x0		,
0		,0xD5		,0x0		,
0		,0xD6		,0x0		,
0		,0xD7		,0x0		,
0		,0xD8		,0x0		,
0		,0xD9		,0xA0		,
0		,0xDA		,0x0		,
0		,0xDB		,0x0		,
0		,0xDC		,0x0		,
0		,0xDD		,0x0		,
0		,0xDE		,0x0		,
0		,0xDF		,0x9		,
0		,0xE0		,0x8		,
0		,0xE1		,0x8		,
0		,0xE2		,0xA8		,
0		,0xE3		,0x0		,
0		,0xE4		,0x0		,
0		,0xE5		,0x0		,
0		,0xE6		,0x0		,
0		,0xE7		,0xB4		,
0		,0xE8		,0xA		,
0		,0xE9		,0x0		,
0		,0xEA		,0x0		,
0		,0xEB		,0x0		,
0		,0xEC		,0x0		,
0		,0xED		,0x0		,
0		,0xEE		,0x0		,
0		,0xEF		,0xB4		,
0		,0xF0		,0x91		,
0		,0xF1		,0x0		,
0		,0xF2		,0x0		,
0		,0xF3		,0x0		,
0		,0xF4		,0x0		,
0		,0xF5		,0x0		,
0		,0xF6		,0x0		,
0		,0xF7		,0x0		,
0		,0xF8		,0x0		,
0		,0xF9		,0x0		,
0		,0xFA		,0x0		,
0		,0xFB		,0x80		,
0		,0xFC		,0x10		,
0		,0xFD		,0x2		,
0		,0xFE		,0x20		,
0		,0xFF		,0xAA		,
1		,0x9		,0x0		,
1		,0xA		,0x0		,
1		,0xB		,0x0		,
1		,0xC		,0x8		,
1		,0xD		,0x0		,
1		,0x14		,0x8		,
1		,0x5A		,0x3C		,
1		,0x5B		,0x0		,
1		,0x5C		,0x0		,
1		,0x5D		,0x0		,
1		,0x5E		,0x0		,
1		,0x5F		,0x0		,
1		,0x60		,0x0		,
1		,0x61		,0xAC		,
1		,0x62		,0x24		,
1		,0x63		,0x0		,
1		,0x64		,0x0		,
1		,0x65		,0x0		,
1		,0x66		,0x0		,
1		,0x67		,0x0		,
1		,0x68		,0x0		,
1		,0x69		,0x0		,
1		,0x6A		,0x0		,
1		,0x6B		,0x0		,
1		,0x74		,0x0		,
1		,0x79		,0x0		,
1		,0x80		,0x5		,
1		,0x81		,0x0		,
1		,0x82		,0x40		,
1		,0x83		,0x0		,
1		,0x84		,0x40		,
1		,0x85		,0x37		,
1		,0x86		,0x2		,
1		,0x87		,0x20		,
1		,0x88		,0x0		,
1		,0x89		,0x20		,
1		,0x8A		,0x0		,
1		,0x8B		,0x3		,
1		,0x8C		,0x60		,
1		,0x8D		,0x30		,
1		,0x8E		,0x88		,
1		,0x8F		,0x11		,
1		,0x90		,0x0		,
1		,0x91		,0x0		,
1		,0x92		,0x76		,
1		,0x93		,0xA6		,
1		,0x94		,0x19		,
1		,0x95		,0x52		,
1		,0x96		,0x73		,
1		,0x97		,0x96		,
1		,0x98		,0x9E		,
1		,0x99		,0x69		,
1		,0x9A		,0xFA		,
1		,0x9B		,0x8F		,
1		,0x9C		,0x2E		,
1		,0x9D		,0x8E		,
1		,0x9E		,0x0		,
1		,0x9F		,0x0		,
1		,0xA0		,0xFD		,
1		,0xA1		,0x6D		,
1		,0xA2		,0x63		,
1		,0xA3		,0x52		,
1		,0xFF		,0x55		,
0xff,0xff,0xff};


// Mode : QAM_B_64QAM
DMD_u8_t DMD_REG_QAM_B_64QAM[]={
0		,0x0		,0x51		,
0		,0x1		,0x0		,
0		,0x2		,0xC0		,
0		,0x3		,0x0		,
0		,0x4		,0x0		,
0		,0x5		,0x0		,
0		,0x6		,0x0		,
0		,0x7		,0x0		,
0		,0x8		,0x0		,
0		,0x9		,0x0		,
0		,0xA		,0x16		,
0		,0xB		,0x35		,
0		,0xC		,0x56		,
0		,0xD		,0x97		,
0		,0xE		,0xDE		,
0		,0xF		,0xD8		,
0		,0x10		,0x0		,
0		,0x11		,0x14		,
0		,0x12		,0xE4		,
0		,0x13		,0x74		,
0		,0x14		,0x8		,
0		,0x15		,0x5		,
0		,0x16		,0x63		,
0		,0x17		,0x0		,
0		,0x18		,0x0		,
0		,0x19		,0x0		,
0		,0x1A		,0x0		,
0		,0x1B		,0x0		,
0		,0x1C		,0x0		,
0		,0x1D		,0x0		,
0		,0x1E		,0x0		,
0		,0x1F		,0x0		,
0		,0x20		,0x0		,
0		,0x21		,0x50		,
0		,0x22		,0x60		,
0		,0x23		,0x36		,
0		,0x24		,0x2		,
0		,0x25		,0x0		,
0		,0x26		,0x0		,
0		,0x27		,0x0		,
0		,0x28		,0x0		,
0		,0x29		,0x0		,
0		,0x2A		,0x20		,
0		,0x2B		,0xC0		,
0		,0x2C		,0x23		,
0		,0x2D		,0x3F		,
0		,0x2E		,0x3F		,
0		,0x2F		,0x0		,
0		,0x30		,0x2E		,
0		,0x31		,0x0		,
0		,0x32		,0x0		,
0		,0x33		,0x81		,
0		,0x34		,0x0		,
0		,0x35		,0x26		,
0		,0x36		,0x21		,
0		,0x37		,0x88		,
0		,0x38		,0x3		,
0		,0x39		,0x19		,
0		,0x3A		,0x85		,
0		,0x3B		,0x5		,
0		,0x3C		,0xC9		,
0		,0x3D		,0x2		,
0		,0x3E		,0x30		,
0		,0x3F		,0x69		,
0		,0x40		,0x1F		,
0		,0x41		,0xF0		,
0		,0x42		,0x0		,
0		,0x43		,0x96		,
0		,0x44		,0x72		,
0		,0x45		,0x1B		,
0		,0x46		,0x2D		,
0		,0x47		,0x1A		,
0		,0x48		,0x31		,
0		,0x49		,0xFE		,
0		,0x4A		,0x96		,
0		,0x4B		,0x7		,
0		,0x4C		,0x26		,
0		,0x4D		,0xE3		,
0		,0x4E		,0x22		,
0		,0x4F		,0x1B		,
0		,0x50		,0x26		,
0		,0x51		,0x12		,
0		,0x52		,0x40		,
0		,0x53		,0x50		,
0		,0x54		,0x60		,
0		,0x55		,0x40		,
0		,0x56		,0xED		,
0		,0x57		,0x96		,
0		,0x58		,0x4A		,
0		,0x59		,0xDE		,
0		,0x5A		,0x88		,
0		,0x5B		,0xC8		,
0		,0x5C		,0x2D		,
0		,0x5D		,0x1		,
0		,0x5E		,0x0		,
0		,0x5F		,0xB8		,
0		,0x60		,0x40		,
0		,0x61		,0x14		,
0		,0x62		,0x13		,
0		,0x63		,0x30		,
0		,0x64		,0x59		,
0		,0x65		,0x7		,
0		,0x66		,0xF0		,
0		,0x67		,0x47		,
0		,0x68		,0xA1		,
0		,0x69		,0x80		,
0		,0x6A		,0x0		,
0		,0x6B		,0x0		,
0		,0x6C		,0xC0		,
0		,0x6D		,0x0		,
0		,0x6E		,0x0		,
0		,0x6F		,0x0		,
0		,0x70		,0x0		,
0		,0x71		,0x0		,
0		,0x72		,0x0		,
0		,0x73		,0x0		,
0		,0x74		,0x2F		,
0		,0x75		,0x0		,
0		,0x76		,0x0		,
0		,0x77		,0x0		,
0		,0x78		,0x0		,
0		,0x79		,0x40		,
0		,0x7A		,0x20		,
0		,0x7B		,0x40		,
0		,0x7C		,0x80		,
0		,0x7D		,0xFF		,
0		,0x7E		,0x20		,
0		,0x7F		,0x40		,
0		,0x80		,0x10		,
0		,0x81		,0x0		,
0		,0x82		,0xE5		,
0		,0x83		,0xC4		,
0		,0x84		,0xE9		,
0		,0x85		,0x52		,
0		,0x86		,0x5F		,
0		,0x87		,0x53		,
0		,0x88		,0x20		,
0		,0x89		,0x43		,
0		,0x8A		,0x11		,
0		,0x8B		,0x8		,
0		,0x8C		,0x43		,
0		,0x8D		,0x11		,
0		,0x8E		,0x8		,
0		,0x8F		,0x0		,
0		,0x90		,0x60		,
0		,0x91		,0x5B		,
0		,0x92		,0x80		,
0		,0x93		,0xE4		,
0		,0x94		,0x34		,
0		,0x95		,0x0		,
0		,0x96		,0x0		,
0		,0x97		,0x7		,
0		,0x98		,0x0		,
0		,0x99		,0xB0		,
0		,0x9A		,0x68		,
0		,0x9B		,0x50		,
0		,0x9C		,0x84		,
0		,0x9D		,0xA9		,
0		,0x9E		,0x11		,
0		,0x9F		,0x4		,
0		,0xA0		,0x44		,
0		,0xA1		,0x14		,
0		,0xA2		,0x9C		,
0		,0xA3		,0x15		,
0		,0xA4		,0xF6		,
0		,0xA5		,0x36		,
0		,0xA6		,0x16		,
0		,0xA7		,0x3F		,
0		,0xA8		,0x1		,
0		,0xA9		,0x8		,
0		,0xAA		,0x0		,
0		,0xAB		,0x0		,
0		,0xAC		,0x0		,
0		,0xAD		,0x65		,
0		,0xAE		,0x87		,
0		,0xAF		,0x56		,
0		,0xB0		,0x52		,
0		,0xB1		,0x55		,
0		,0xB2		,0x35		,
0		,0xB3		,0x13		,
0		,0xB4		,0x88		,
0		,0xB5		,0x23		,
0		,0xB6		,0x28		,
0		,0xB7		,0x3C		,
0		,0xB8		,0x1		,
0		,0xB9		,0x5B		,
0		,0xBA		,0x0		,
0		,0xBB		,0x0		,
0		,0xBC		,0x0		,
0		,0xBD		,0x0		,
0		,0xBE		,0x0		,
0		,0xBF		,0x0		,
0		,0xC0		,0x0		,
0		,0xC1		,0x0		,
0		,0xC2		,0x0		,
0		,0xC3		,0x0		,
0		,0xC4		,0x0		,
0		,0xC5		,0x0		,
0		,0xC6		,0x0		,
0		,0xC7		,0x0		,
0		,0xC8		,0x0		,
0		,0xC9		,0x0		,
0		,0xCA		,0x0		,
0		,0xCB		,0x0		,
0		,0xCC		,0x0		,
0		,0xCD		,0x0		,
0		,0xCE		,0x0		,
0		,0xCF		,0x0		,
0		,0xD0		,0x0		,
0		,0xD1		,0x0		,
0		,0xD2		,0x0		,
0		,0xD3		,0x0		,
0		,0xD4		,0x0		,
0		,0xD5		,0x0		,
0		,0xD6		,0x0		,
0		,0xD7		,0x0		,
0		,0xD8		,0x0		,
0		,0xD9		,0xA0		,
0		,0xDA		,0x0		,
0		,0xDB		,0x0		,
0		,0xDC		,0x0		,
0		,0xDD		,0x0		,
0		,0xDE		,0x0		,
0		,0xDF		,0x9		,
0		,0xE0		,0x8		,
0		,0xE1		,0x8		,
0		,0xE2		,0xA8		,
0		,0xE3		,0x0		,
0		,0xE4		,0x0		,
0		,0xE5		,0x0		,
0		,0xE6		,0x0		,
0		,0xE7		,0xB4		,
0		,0xE8		,0xA		,
0		,0xE9		,0x0		,
0		,0xEA		,0x0		,
0		,0xEB		,0x0		,
0		,0xEC		,0x0		,
0		,0xED		,0x0		,
0		,0xEE		,0x0		,
0		,0xEF		,0xB4		,
0		,0xF0		,0x91		,
0		,0xF1		,0x0		,
0		,0xF2		,0x0		,
0		,0xF3		,0x0		,
0		,0xF4		,0x0		,
0		,0xF5		,0x0		,
0		,0xF6		,0x0		,
0		,0xF7		,0x0		,
0		,0xF8		,0x0		,
0		,0xF9		,0x0		,
0		,0xFA		,0x0		,
0		,0xFB		,0x80		,
0		,0xFC		,0x10		,
0		,0xFD		,0x2		,
0		,0xFE		,0x20		,
0		,0xFF		,0xAA		,
1		,0x9		,0x0		,
1		,0xA		,0x0		,
1		,0xB		,0x0		,
1		,0xC		,0x8		,
1		,0xD		,0x0		,
1		,0x14		,0x8		,
1		,0x5A		,0x3C		,
1		,0x5B		,0x0		,
1		,0x5C		,0x0		,
1		,0x5D		,0x0		,
1		,0x5E		,0x0		,
1		,0x5F		,0x0		,
1		,0x60		,0x0		,
1		,0x61		,0xAC		,
1		,0x62		,0x24		,
1		,0x63		,0x0		,
1		,0x64		,0x0		,
1		,0x65		,0x0		,
1		,0x66		,0x0		,
1		,0x67		,0x0		,
1		,0x68		,0x0		,
1		,0x69		,0x0		,
1		,0x6A		,0x0		,
1		,0x6B		,0x0		,
1		,0x74		,0x0		,
1		,0x79		,0x0		,
1		,0x80		,0x5		,
1		,0x81		,0x0		,
1		,0x82		,0x40		,
1		,0x83		,0x0		,
1		,0x84		,0x40		,
1		,0x85		,0x37		,
1		,0x86		,0x2		,
1		,0x87		,0x20		,
1		,0x88		,0x0		,
1		,0x89		,0x20		,
1		,0x8A		,0x0		,
1		,0x8B		,0x3		,
1		,0x8C		,0x60		,
1		,0x8D		,0x30		,
1		,0x8E		,0x88		,
1		,0x8F		,0x11		,
1		,0x90		,0x0		,
1		,0x91		,0x0		,
1		,0x92		,0x71		,
1		,0x93		,0xE7		,
1		,0x94		,0x23		,
1		,0x95		,0x52		,
1		,0x96		,0x73		,
1		,0x97		,0x96		,
1		,0x98		,0xA5		,
1		,0x99		,0x3		,
1		,0x9A		,0xBA		,
1		,0x9B		,0x95		,
1		,0x9C		,0x25		,
1		,0x9D		,0xD4		,
1		,0x9E		,0x0		,
1		,0x9F		,0x0		,
1		,0xA0		,0xFD		,
1		,0xA1		,0x6D		,
1		,0xA2		,0x63		,
1		,0xA3		,0x52		,
1		,0xFF		,0x55		,
0xff,0xff,0xff};


// Mode : QAM_B_256QAM
DMD_u8_t DMD_REG_QAM_B_256QAM[]={
0		,0x0		,0x53		,
0		,0x1		,0x0		,
0		,0x2		,0xC0		,
0		,0x3		,0x0		,
0		,0x4		,0x0		,
0		,0x5		,0x0		,
0		,0x6		,0x0		,
0		,0x7		,0x0		,
0		,0x8		,0x0		,
0		,0x9		,0x0		,
0		,0xA		,0x16		,
0		,0xB		,0x35		,
0		,0xC		,0x56		,
0		,0xD		,0x8F		,
0		,0xE		,0x44		,
0		,0xF		,0xED		,
0		,0x10		,0x0		,
0		,0x11		,0x14		,
0		,0x12		,0xE4		,
0		,0x13		,0x74		,
0		,0x14		,0x8		,
0		,0x15		,0x5		,
0		,0x16		,0x63		,
0		,0x17		,0x0		,
0		,0x18		,0x0		,
0		,0x19		,0x0		,
0		,0x1A		,0x0		,
0		,0x1B		,0x0		,
0		,0x1C		,0x0		,
0		,0x1D		,0x0		,
0		,0x1E		,0x0		,
0		,0x1F		,0x0		,
0		,0x20		,0x0		,
0		,0x21		,0x50		,
0		,0x22		,0x60		,
0		,0x23		,0x36		,
0		,0x24		,0x2		,
0		,0x25		,0x0		,
0		,0x26		,0x0		,
0		,0x27		,0x0		,
0		,0x28		,0x0		,
0		,0x29		,0x0		,
0		,0x2A		,0x20		,
0		,0x2B		,0xC0		,
0		,0x2C		,0x23		,
0		,0x2D		,0x3F		,
0		,0x2E		,0x3F		,
0		,0x2F		,0x0		,
0		,0x30		,0x2E		,
0		,0x31		,0x0		,
0		,0x32		,0x0		,
0		,0x33		,0x81		,
0		,0x34		,0x0		,
0		,0x35		,0x26		,
0		,0x36		,0x21		,
0		,0x37		,0x88		,
0		,0x38		,0x3		,
0		,0x39		,0x19		,
0		,0x3A		,0x85		,
0		,0x3B		,0x5		,
0		,0x3C		,0xC9		,
0		,0x3D		,0x2		,
0		,0x3E		,0x30		,
0		,0x3F		,0x69		,
0		,0x40		,0x1F		,
0		,0x41		,0xF0		,
0		,0x42		,0x0		,
0		,0x43		,0x96		,
0		,0x44		,0x72		,
0		,0x45		,0x1B		,
0		,0x46		,0x2D		,
0		,0x47		,0x1A		,
0		,0x48		,0x31		,
0		,0x49		,0xFE		,
0		,0x4A		,0x96		,
0		,0x4B		,0x7		,
0		,0x4C		,0x26		,
0		,0x4D		,0xE3		,
0		,0x4E		,0x22		,
0		,0x4F		,0x1B		,
0		,0x50		,0x26		,
0		,0x51		,0x12		,
0		,0x52		,0x40		,
0		,0x53		,0x50		,
0		,0x54		,0x60		,
0		,0x55		,0x40		,
0		,0x56		,0xED		,
0		,0x57		,0x96		,
0		,0x58		,0x4A		,
0		,0x59		,0xDE		,
0		,0x5A		,0x88		,
0		,0x5B		,0xC8		,
0		,0x5C		,0x2D		,
0		,0x5D		,0x1		,
0		,0x5E		,0x0		,
0		,0x5F		,0xB8		,
0		,0x60		,0x40		,
0		,0x61		,0x14		,
0		,0x62		,0x13		,
0		,0x63		,0x30		,
0		,0x64		,0x59		,
0		,0x65		,0x7		,
0		,0x66		,0xF0		,
0		,0x67		,0x47		,
0		,0x68		,0xA1		,
0		,0x69		,0x80		,
0		,0x6A		,0x0		,
0		,0x6B		,0x0		,
0		,0x6C		,0xC0		,
0		,0x6D		,0x0		,
0		,0x6E		,0x0		,
0		,0x6F		,0x0		,
0		,0x70		,0x0		,
0		,0x71		,0x0		,
0		,0x72		,0x0		,
0		,0x73		,0x0		,
0		,0x74		,0x2F		,
0		,0x75		,0x0		,
0		,0x76		,0x0		,
0		,0x77		,0x0		,
0		,0x78		,0x0		,
0		,0x79		,0x40		,
0		,0x7A		,0x20		,
0		,0x7B		,0x40		,
0		,0x7C		,0x80		,
0		,0x7D		,0xFF		,
0		,0x7E		,0x20		,
0		,0x7F		,0x40		,
0		,0x80		,0x10		,
0		,0x81		,0x0		,
0		,0x82		,0xE5		,
0		,0x83		,0xC4		,
0		,0x84		,0xE9		,
0		,0x85		,0x52		,
0		,0x86		,0x5F		,
0		,0x87		,0x53		,
0		,0x88		,0x20		,
0		,0x89		,0x43		,
0		,0x8A		,0x11		,
0		,0x8B		,0x8		,
0		,0x8C		,0x43		,
0		,0x8D		,0x11		,
0		,0x8E		,0x8		,
0		,0x8F		,0x0		,
0		,0x90		,0x60		,
0		,0x91		,0x5B		,
0		,0x92		,0x80		,
0		,0x93		,0xE4		,
0		,0x94		,0x34		,
0		,0x95		,0x0		,
0		,0x96		,0x0		,
0		,0x97		,0x7		,
0		,0x98		,0x0		,
0		,0x99		,0xB0		,
0		,0x9A		,0x68		,
0		,0x9B		,0x50		,
0		,0x9C		,0x84		,
0		,0x9D		,0xA9		,
0		,0x9E		,0x11		,
0		,0x9F		,0x4		,
0		,0xA0		,0x44		,
0		,0xA1		,0x14		,
0		,0xA2		,0x9C		,
0		,0xA3		,0x15		,
0		,0xA4		,0xF6		,
0		,0xA5		,0x36		,
0		,0xA6		,0x16		,
0		,0xA7		,0x3F		,
0		,0xA8		,0x1		,
0		,0xA9		,0x8		,
0		,0xAA		,0x0		,
0		,0xAB		,0x0		,
0		,0xAC		,0x0		,
0		,0xAD		,0x65		,
0		,0xAE		,0x87		,
0		,0xAF		,0x56		,
0		,0xB0		,0x52		,
0		,0xB1		,0x55		,
0		,0xB2		,0x35		,
0		,0xB3		,0x13		,
0		,0xB4		,0x88		,
0		,0xB5		,0x23		,
0		,0xB6		,0x28		,
0		,0xB7		,0x3C		,
0		,0xB8		,0x1		,
0		,0xB9		,0x5B		,
0		,0xBA		,0x0		,
0		,0xBB		,0x0		,
0		,0xBC		,0x0		,
0		,0xBD		,0x0		,
0		,0xBE		,0x0		,
0		,0xBF		,0x0		,
0		,0xC0		,0x0		,
0		,0xC1		,0x0		,
0		,0xC2		,0x0		,
0		,0xC3		,0x0		,
0		,0xC4		,0x0		,
0		,0xC5		,0x0		,
0		,0xC6		,0x0		,
0		,0xC7		,0x0		,
0		,0xC8		,0x0		,
0		,0xC9		,0x0		,
0		,0xCA		,0x0		,
0		,0xCB		,0x0		,
0		,0xCC		,0x0		,
0		,0xCD		,0x0		,
0		,0xCE		,0x0		,
0		,0xCF		,0x0		,
0		,0xD0		,0x0		,
0		,0xD1		,0x0		,
0		,0xD2		,0x0		,
0		,0xD3		,0x0		,
0		,0xD4		,0x0		,
0		,0xD5		,0x0		,
0		,0xD6		,0x0		,
0		,0xD7		,0x0		,
0		,0xD8		,0x0		,
0		,0xD9		,0xA0		,
0		,0xDA		,0x0		,
0		,0xDB		,0x0		,
0		,0xDC		,0x0		,
0		,0xDD		,0x0		,
0		,0xDE		,0x0		,
0		,0xDF		,0x9		,
0		,0xE0		,0x8		,
0		,0xE1		,0x8		,
0		,0xE2		,0xA8		,
0		,0xE3		,0x0		,
0		,0xE4		,0x0		,
0		,0xE5		,0x0		,
0		,0xE6		,0x0		,
0		,0xE7		,0xB4		,
0		,0xE8		,0xA		,
0		,0xE9		,0x0		,
0		,0xEA		,0x0		,
0		,0xEB		,0x0		,
0		,0xEC		,0x0		,
0		,0xED		,0x0		,
0		,0xEE		,0x0		,
0		,0xEF		,0xB4		,
0		,0xF0		,0x91		,
0		,0xF1		,0x0		,
0		,0xF2		,0x0		,
0		,0xF3		,0x0		,
0		,0xF4		,0x0		,
0		,0xF5		,0x0		,
0		,0xF6		,0x0		,
0		,0xF7		,0x0		,
0		,0xF8		,0x0		,
0		,0xF9		,0x0		,
0		,0xFA		,0x0		,
0		,0xFB		,0x80		,
0		,0xFC		,0x10		,
0		,0xFD		,0x2		,
0		,0xFE		,0x20		,
0		,0xFF		,0xAA		,
1		,0x9		,0x0		,
1		,0xA		,0x0		,
1		,0xB		,0x0		,
1		,0xC		,0x8		,
1		,0xD		,0x0		,
1		,0x14		,0x8		,
1		,0x5A		,0x3C		,
1		,0x5B		,0x0		,
1		,0x5C		,0x0		,
1		,0x5D		,0x0		,
1		,0x5E		,0x0		,
1		,0x5F		,0x0		,
1		,0x60		,0x0		,
1		,0x61		,0xAC		,
1		,0x62		,0x24		,
1		,0x63		,0x0		,
1		,0x64		,0x0		,
1		,0x65		,0x0		,
1		,0x66		,0x0		,
1		,0x67		,0x0		,
1		,0x68		,0x0		,
1		,0x69		,0x0		,
1		,0x6A		,0x0		,
1		,0x6B		,0x0		,
1		,0x74		,0x0		,
1		,0x79		,0x0		,
1		,0x80		,0x5		,
1		,0x81		,0x0		,
1		,0x82		,0x40		,
1		,0x83		,0x0		,
1		,0x84		,0x40		,
1		,0x85		,0x37		,
1		,0x86		,0x2		,
1		,0x87		,0x20		,
1		,0x88		,0x0		,
1		,0x89		,0x20		,
1		,0x8A		,0x0		,
1		,0x8B		,0x3		,
1		,0x8C		,0x60		,
1		,0x8D		,0x30		,
1		,0x8E		,0x88		,
1		,0x8F		,0x11		,
1		,0x90		,0x0		,
1		,0x91		,0x0		,
1		,0x92		,0x76		,
1		,0x93		,0xA6		,
1		,0x94		,0x19		,
1		,0x95		,0x4F		,
1		,0x96		,0x27		,
1		,0x97		,0x49		,
1		,0x98		,0xA5		,
1		,0x99		,0x3		,
1		,0x9A		,0xBA		,
1		,0x9B		,0x95		,
1		,0x9C		,0x25		,
1		,0x9D		,0xD4		,
1		,0x9E		,0x0		,
1		,0x9F		,0x0		,
1		,0xA0		,0xFD		,
1		,0xA1		,0x6D		,
1		,0xA2		,0x63		,
1		,0xA3		,0x52		,
1		,0xFF		,0x55		,
0xff,0xff,0xff};

