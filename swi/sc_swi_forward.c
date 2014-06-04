/*
 * Driver: sc_sswi_forward.c
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 04.06.2014
 * Description:
 * TODO
 */

#include<inttypes.h>
#include "sc_swi_forward.h"
#include <stdio.h>
#include <stdlib.h>


void SwiStdioPrintf(const char* text)
{
    printf(text);
}
