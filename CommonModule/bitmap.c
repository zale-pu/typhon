/**********************************************************
*
 * Proprietary & confidential, All right reserved, 2010
 *         Greenliant System, Ltd.
*
*     Module Name : bitmap.c
*     Description : bitmap functions
**********************************************************/

/*********************************************************
*     Kaifeng Zhuang @ 2012.01.17
*     Description  : Initial create
**********************************************************/

/*********************************************************
*  Include section
*  Add all #includes here
*
**********************************************************/
#include "bitmap.h"

/*********************************************************
* Defines section
* Add all #defines here
*
*********************************************************/
/*********************************************************
* Global variables section
* Add all globalvariables here
*
*********************************************************/


/*********************************************************
 * Static Function Prototype Section
 * Add prototypes for all functions called by this
 * module, with the exception of runtime routines.
 *
 *********************************************************/


/****************************************************************************
 * BitMapTestBit() - Test bit status.
 * startaddr: start address of bitmap buffer
 * offset:    bit offset of bitmap buffer
 * return: 1 for bit value is ONE,  0 for bit value is ZERO
 *
****************************************************************************/
unsigned int BitmapTstBit(void *startaddr, unsigned int offset)
{
	unsigned int t;

	t = ((unsigned char *)startaddr)[offset/8];
	t = (t>>(offset%8)) & 1;

    return t;
}

/****************************************************************************
 * BitMapClearBit() - Clear bit
 * startaddr: start address of bitmap buffer
 * offset:    bit offset of bitmap buffer
 * return:
 *
****************************************************************************/
void BitmapClrBit(void *startaddr, unsigned int offset)
{
	unsigned int t;

	t = ((unsigned char *)startaddr)[offset/8];
	t &= ~( 1 << (offset%8) );
	((unsigned char *)startaddr)[offset/8] = t;
}
/****************************************************************************
 * BitMapSetBit() - Set bit
 * startaddr: start address of bitmap buffer
 * offset:    bit offset of bitmap buffer
 * return:
 *
****************************************************************************/
void BitmapSetBit(void *startaddr, unsigned int offset)
{
	unsigned int t;

	t = ((unsigned char *)startaddr)[offset/8];
	t |= ( 1 << (offset%8) );
	((unsigned char *)startaddr)[offset/8] = t;
}

