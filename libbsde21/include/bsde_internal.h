#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifndef BSDE_INTERNAL_H
#define BSDE_INTERNAL_H

#ifndef BSDE_MAX_CONTACTS
#define BSDE_MAX_CONTACTS	4096
#endif

#include <bsde_conf.h>

#include <bsde_types.h>
#include <bsde_math.h>

#include <bsde_vecnf.h>
#include <bsde_matnf.h>

#include <bsde_const.h>
#include <bsde_const_i.h>
#include <bsde_solid.h>
#include <bsde_solid_bsp.h>
#include <bsde_auto.h>

#endif
