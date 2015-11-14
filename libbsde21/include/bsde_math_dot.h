#ifndef BSDE_MATH_DOT_H
#define BSDE_MATH_DOT_H

#define V3_NDOT(a, n)		((a)[0]*(n)[0]+(a)[1]*(n)[1]+ \
				(a)[2]*(n)[2]-(n)[3])
#define V3_NDOT(a, n)		((a)[0]*(n)[0]+(a)[1]*(n)[1]+ \
				(a)[2]*(n)[2]-(n)[3])

#define V2_DOTJK(a, b, j, k)	((a)[0*(j)]*(b)[0*(k)] + \
				 (a)[1*(j)]*(b)[1*(k)])
#define V3_DOTJK(a, b, j, k)	((a)[0*(j)]*(b)[0*(k)] + \
				 (a)[1*(j)]*(b)[1*(k)] + \
				 (a)[2*(j)]*(b)[2*(k)])
#define V4_DOTJK(a, b, j, k)	((a)[0*(j)]*(b)[0*(k)] + \
				 (a)[1*(j)]*(b)[1*(k)] + \
				 (a)[2*(j)]*(b)[2*(k)] + \
				 (a)[3*(j)]*(b)[3*(k)])

#define V2_DOT11(a, b)		V2_DOTJK(a, b, 1, 1)
#define V2_DOT12(a, b)		V2_DOTJK(a, b, 1, 2)
#define V2_DOT13(a, b)		V2_DOTJK(a, b, 1, 3)
#define V2_DOT14(a, b)		V2_DOTJK(a, b, 1, 4)
#define V2_DOT21(a, b)		V2_DOTJK(a, b, 2, 1)
#define V2_DOT22(a, b)		V2_DOTJK(a, b, 2, 2)
#define V2_DOT23(a, b)		V2_DOTJK(a, b, 2, 3)
#define V2_DOT24(a, b)		V2_DOTJK(a, b, 2, 4)
#define V2_DOT31(a, b)		V2_DOTJK(a, b, 3, 1)
#define V2_DOT32(a, b)		V2_DOTJK(a, b, 3, 2)
#define V2_DOT33(a, b)		V2_DOTJK(a, b, 3, 3)
#define V2_DOT34(a, b)		V2_DOTJK(a, b, 3, 4)
#define V2_DOT41(a, b)		V2_DOTJK(a, b, 4, 1)
#define V2_DOT42(a, b)		V2_DOTJK(a, b, 4, 2)
#define V2_DOT43(a, b)		V2_DOTJK(a, b, 4, 3)
#define V2_DOT44(a, b)		V2_DOTJK(a, b, 4, 4)

#define V3_DOT11(a, b)		V3_DOTJK(a, b, 1, 1)
#define V3_DOT12(a, b)		V3_DOTJK(a, b, 1, 2)
#define V3_DOT13(a, b)		V3_DOTJK(a, b, 1, 3)
#define V3_DOT14(a, b)		V3_DOTJK(a, b, 1, 4)
#define V3_DOT21(a, b)		V3_DOTJK(a, b, 2, 1)
#define V3_DOT22(a, b)		V3_DOTJK(a, b, 2, 2)
#define V3_DOT23(a, b)		V3_DOTJK(a, b, 2, 3)
#define V3_DOT24(a, b)		V3_DOTJK(a, b, 2, 4)
#define V3_DOT31(a, b)		V3_DOTJK(a, b, 3, 1)
#define V3_DOT32(a, b)		V3_DOTJK(a, b, 3, 2)
#define V3_DOT33(a, b)		V3_DOTJK(a, b, 3, 3)
#define V3_DOT34(a, b)		V3_DOTJK(a, b, 3, 4)
#define V3_DOT41(a, b)		V3_DOTJK(a, b, 4, 1)
#define V3_DOT42(a, b)		V3_DOTJK(a, b, 4, 2)
#define V3_DOT43(a, b)		V3_DOTJK(a, b, 4, 3)
#define V3_DOT44(a, b)		V3_DOTJK(a, b, 4, 4)

#define V4_DOT11(a, b)		V4_DOTJK(a, b, 1, 1)
#define V4_DOT12(a, b)		V4_DOTJK(a, b, 1, 2)
#define V4_DOT13(a, b)		V4_DOTJK(a, b, 1, 3)
#define V4_DOT14(a, b)		V4_DOTJK(a, b, 1, 4)
#define V4_DOT21(a, b)		V4_DOTJK(a, b, 2, 1)
#define V4_DOT22(a, b)		V4_DOTJK(a, b, 2, 2)
#define V4_DOT23(a, b)		V4_DOTJK(a, b, 2, 3)
#define V4_DOT24(a, b)		V4_DOTJK(a, b, 2, 4)
#define V4_DOT31(a, b)		V4_DOTJK(a, b, 3, 1)
#define V4_DOT32(a, b)		V4_DOTJK(a, b, 3, 2)
#define V4_DOT33(a, b)		V4_DOTJK(a, b, 3, 3)
#define V4_DOT34(a, b)		V4_DOTJK(a, b, 3, 4)
#define V4_DOT41(a, b)		V4_DOTJK(a, b, 4, 1)
#define V4_DOT42(a, b)		V4_DOTJK(a, b, 4, 2)
#define V4_DOT43(a, b)		V4_DOTJK(a, b, 4, 3)
#define V4_DOT44(a, b)		V4_DOTJK(a, b, 4, 4)

#endif

