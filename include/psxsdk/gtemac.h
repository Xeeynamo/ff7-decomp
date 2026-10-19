// clang-format off
#ifndef GTEMAC_H
#define GTEMAC_H

#ifdef PLATFORM_PSYZ
#define gte_NormalColorCol(r1, r2, r3)
#define gte_MulMatrix0(r1, r2, r3)
#define gte_CompMatrix(r1, r2, r3)
#else
#define gte_NormalColorCol(r1,r2,r3) 				\
				{	gte_ldv0(r1);		\
					gte_ldrgb(r2);		\
					gte_nccs();		\
					gte_strgb(r3);	}

#define gte_MulMatrix0(r1,r2,r3)				\
				{	gte_SetRotMatrix(r1);	\
					gte_ldclmv(r2);		\
					gte_rtir();		\
					gte_stclmv(r3);		\
					gte_ldclmv((char*)r2+2);\
					gte_rtir();		\
					gte_stclmv((char*)r3+2);\
					gte_ldclmv((char*)r2+4);\
					gte_rtir();		\
					gte_stclmv((char*)r3+4);	}

#define gte_CompMatrix(r1,r2,r3)				\
				{	gte_MulMatrix0(r1,r2,r3);\
					gte_SetTransMatrix(r1);	\
					gte_ldlv0((char*)r2+20);\
					gte_rt();		\
					gte_stlvl((char*)r3+20);	}			
#endif
#endif
