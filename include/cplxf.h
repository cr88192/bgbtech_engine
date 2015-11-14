#ifndef CPLXF_H
#define CPLXF_H

// #include <vecnf.h>
#include <bgbgc_conf.h>
// #include <bgbgc.h>
#include <math.h>

#ifdef __BSCC
#include <cplxf_bscc.h>
#else
#ifdef MSVC
#include <cplxf_msvc.h>
#else
#include <cplxf_raw.h>
#endif
#endif

#endif
