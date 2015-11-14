#ifdef __BSCC

#define s128mk4(a, b, c, d)		\
	(((s128)(a)) |
	(((s128)(b))<<32) |
	(((s128)(c))<<64) |
	(((s128)(d))<<96))

#define s128add(a, b)	((a)+(b))
#define s128sub(a, b)	((a)-(b))
#define s128mul(a, b)	((a)*(b))
#define s128div(a, b)	((a)/(b))
#define s128mod(a, b)	((a)%(b))

#define s128shl(a, b)	((a)<<(b))
#define s128shr(a, b)	((a)>>(b))

#define s128and(a, b)	((a)&(b))
#define s128or(a, b)	((a)|(b))
#define s128xor(a, b)	((a)^(b))


#define u128add(a, b)	((a)+(b))
#define u128sub(a, b)	((a)-(b))
#define u128mul(a, b)	((a)*(b))
#define u128div(a, b)	((a)/(b))
#define u128mod(a, b)	((a)%(b))

#define u128shl(a, b)	((a)<<(b))
#define u128shr(a, b)	((a)>>(b))

#define u128and(a, b)	((a)&(b))
#define u128or(a, b)	((a)|(b))
#define u128xor(a, b)	((a)^(b))

#else

#define s128mk4(a, b, c, d)		iv128_mk4(a, b, c, d)

#define s128add(a, b)	iv128_add(a, b)
#define s128sub(a, b)	iv128_sub(a, b)
#define s128mul(a, b)	iv128_mul(a, b)
#define s128div(a, b)	iv128_div(a, b)
#define s128mod(a, b)	iv128_mod(a, b)

#define s128shl(a, b)	iv128_shl(a, b)
#define s128shr(a, b)	iv128_shr(a, b)

#define s128and(a, b)	iv128_and(a, b)
#define s128or(a, b)	iv128_or(a, b)
#define s128xor(a, b)	iv128_xor(a, b)

#endif
