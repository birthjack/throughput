/* crypto/md32_common.h */
/* ====================================================================
 * Copyright (c) 1999-2002 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

/*
 * This is a generic 32 bit "collector" for message digest algorithms.
 * Whenever needed it collects input character stream into chunks of
 * 32 bit values and invokes a block function that performs actual hash
 * calculations.
 *
 * Porting guide.
 *
 * Obligatory macros:
 *
 * DATA_ORDER_IS_BIG_ENDIAN or DATA_ORDER_IS_LITTLE_ENDIAN
 *	this macro defines byte order of input stream.
 * HASH_CBLOCK
 *	size of a unit chunk HASH_BLOCK operates on.
 * HASH_LONG
 *	has to be at lest 32 bit wide, if it's wider, then
 *	HASH_LONG_LOG2 *has to* be defined along
 * HASH_CTX
 *	context structure that at least contains following
 *	members:
 *		typedef struct {
 *			...
 *			HASH_LONG	Nl,Nh;
 *			HASH_LONG	data[HASH_LBLOCK];
 *			int		num;
 *			...
 *			} HASH_CTX;
 * HASH_UPDATE
 *	name of "Update" function, implemented here.
 * HASH_TRANSFORM
 *	name of "Transform" function, implemented here.
 * HASH_FINAL
 *	name of "Final" function, implemented here.
 * HASH_BLOCK_HOST_ORDER
 *	name of "block" function treating *aligned* input message
 *	in host byte order, implemented externally.
 * HASH_BLOCK_DATA_ORDER
 *	name of "block" function treating *unaligned* input message
 *	in original (data) byte order, implemented externally (it
 *	actually is optional if data and host are of the same
 *	"endianess").
 * HASH_MAKE_STRING
 *	macro convering context variables to an ASCII hash string.
 *
 * Optional macros:
 *
 * B_ENDIAN or L_ENDIAN
 *	defines host byte-order.
 * HASH_LONG_LOG2
 *	defaults to 2 if not states otherwise.
 * HASH_LBLOCK
 *	assumed to be HASH_CBLOCK/4 if not stated otherwise.
 * HASH_BLOCK_DATA_ORDER_ALIGNED
 *	alternative "block" function capable of treating
 *	aligned input message in original (data) order,
 *	implemented externally.
 *
 * MD5 example:
 *
 *	#define DATA_ORDER_IS_LITTLE_ENDIAN
 *
 *	#define HASH_LONG		MD5_LONG
 *	#define HASH_LONG_LOG2		MD5_LONG_LOG2
 *	#define HASH_CTX		MD5_CTX
 *	#define HASH_CBLOCK		MD5_CBLOCK
 *	#define HASH_LBLOCK		MD5_LBLOCK
 *	#define HASH_UPDATE		MD5_Update
 *	#define HASH_TRANSFORM		MD5_Transform
 *	#define HASH_FINAL		MD5_Final
 *	#define HASH_BLOCK_HOST_ORDER	md5_block_host_order
 *	#define HASH_BLOCK_DATA_ORDER	md5_block_data_order
 *
 *					<appro@fy.chalmers.se>
 */

#ifndef __md32_common_h 
#define __md32_common_h 



#ifdef  __cplusplus
extern "C" {
#endif

#ifdef OPENSSL_NO_MD5
#error MD5 is disabled.
#endif

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * ! MD5_LONG has to be at least 32 bits wide. If it's wider, then !
 * ! MD5_LONG_LOG2 has to be defined along.			   !
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#if defined(OPENSSL_SYS_WIN16) || defined(__LP32__)
#define MD5_LONG unsigned long
#elif defined(OPENSSL_SYS_CRAY) || defined(__ILP64__)
#define MD5_LONG unsigned long
#define MD5_LONG_LOG2 3
/*
 * _CRAY note. I could declare short, but I have no idea what impact
 * does it have on performance on none-T3E machines. I could declare
 * int, but at least on C90 sizeof(int) can be chosen at compile time.
 * So I've chosen long...
 *					<appro@fy.chalmers.se>
 */
#else
#define MD5_LONG unsigned int
#endif

#define MD5_CBLOCK	64
#define MD5_LBLOCK	(MD5_CBLOCK/4)
#define MD5_DIGEST_LENGTH 16
//#define MD5_CBLOCK	512
//#define MD5_LBLOCK	(MD5_CBLOCK/4)
//#define MD5_DIGEST_LENGTH 128
typedef struct MD5state_st
	{
	MD5_LONG A,B,C,D;
	MD5_LONG Nl,Nh;
	MD5_LONG data[MD5_LBLOCK];
	int num;
	} MD5_CTX;

int MD5_Init(MD5_CTX *c);
int MD5_Update(MD5_CTX *c, const void *data, unsigned long len);
int MD5_Final(unsigned char *md, MD5_CTX *c);
unsigned char *MD5(const unsigned char *d, unsigned long n, unsigned char *md);
void MD5_Transform(MD5_CTX *c, const unsigned char *b);

#ifdef  __cplusplus
}
#endif

#define DATA_ORDER_IS_LITTLE_ENDIAN

#define HASH_LONG		MD5_LONG
#define HASH_LONG_LOG2		MD5_LONG_LOG2
#define HASH_CTX		MD5_CTX
#define HASH_CBLOCK		MD5_CBLOCK
#define HASH_LBLOCK		MD5_LBLOCK
#define HASH_UPDATE		MD5_Update
#define HASH_TRANSFORM		MD5_Transform
#define HASH_FINAL		MD5_Final
#define HASH_BLOCK_HOST_ORDER	md5_block_host_order
#define HASH_BLOCK_DATA_ORDER	md5_block_data_order


//#include <openssl/crypto.h>
//#include <openssl/fips.h>
//#include <openssl/err.h>

#if !defined(DATA_ORDER_IS_BIG_ENDIAN) && !defined(DATA_ORDER_IS_LITTLE_ENDIAN)
#error "DATA_ORDER must be defined!"
#endif

#ifndef HASH_CBLOCK
#error "HASH_CBLOCK must be defined!"
#endif
#ifndef HASH_LONG
#error "HASH_LONG must be defined!"
#endif
#ifndef HASH_CTX
#error "HASH_CTX must be defined!"
#endif

#ifndef HASH_UPDATE
#error "HASH_UPDATE must be defined!"
#endif
#ifndef HASH_TRANSFORM
#error "HASH_TRANSFORM must be defined!"
#endif
#ifndef HASH_FINAL
#error "HASH_FINAL must be defined!"
#endif

#ifndef HASH_BLOCK_HOST_ORDER
#error "HASH_BLOCK_HOST_ORDER must be defined!"
#endif

#if 0
/*
 * Moved below as it's required only if HASH_BLOCK_DATA_ORDER_ALIGNED
 * isn't defined.
 */
#ifndef HASH_BLOCK_DATA_ORDER
#error "HASH_BLOCK_DATA_ORDER must be defined!"
#endif
#endif

#ifndef HASH_LBLOCK
#define HASH_LBLOCK	(HASH_CBLOCK/4)
#endif

#ifndef HASH_LONG_LOG2
#define HASH_LONG_LOG2	2
#endif

/*
 * Engage compiler specific rotate intrinsic function if available.
 */
#undef ROTATE
#ifndef PEDANTIC
# if 0 /* defined(_MSC_VER) */
#  define ROTATE(a,n)	_lrotl(a,n)
# elif defined(__MWERKS__)
#  if defined(__POWERPC__)
#   define ROTATE(a,n)	__rlwinm(a,n,0,31)
#  elif defined(__MC68K__)
    /* Motorola specific tweak. <appro@fy.chalmers.se> */
#   define ROTATE(a,n)	( n<24 ? __rol(a,n) : __ror(a,32-n) )
#  else
#   define ROTATE(a,n)	__rol(a,n)
#  endif
# elif defined(__GNUC__) && __GNUC__>=2 && !defined(OPENSSL_NO_ASM) && !defined(OPENSSL_NO_INLINE_ASM)
  /*
   * Some GNU C inline assembler templates. Note that these are
   * rotates by *constant* number of bits! But that's exactly
   * what we need here...
   *
   * 					<appro@fy.chalmers.se>
   */
#  if defined(__i386) || defined(__i386__) || defined(__x86_64) || defined(__x86_64__)
#   define ROTATE(a,n)	({ register unsigned int ret;	\
				asm (			\
				"roll %1,%0"		\
				: "=r"(ret)		\
				: "I"(n), "0"(a)	\
				: "cc");		\
			   ret;				\
			})
#  elif defined(__powerpc) || defined(__ppc)
#   define ROTATE(a,n)	({ register unsigned int ret;	\
				asm (			\
				"rlwinm %0,%1,%2,0,31"	\
				: "=r"(ret)		\
				: "r"(a), "I"(n));	\
			   ret;				\
			})
#  endif
# endif

/*
 * Engage compiler specific "fetch in reverse byte order"
 * intrinsic function if available.
 */
# if defined(__GNUC__) && __GNUC__>=2 && !defined(OPENSSL_NO_ASM) && !defined(OPENSSL_NO_INLINE_ASM)
  /* some GNU C inline assembler templates by <appro@fy.chalmers.se> */
#  if (defined(__i386) || defined(__i386__) || defined(__x86_64) || defined(__x86_64__)) && !defined(I386_ONLY)
#   define BE_FETCH32(a)	({ register unsigned int l=(a);\
				asm (			\
				"bswapl %0"		\
				: "=r"(l) : "0"(l));	\
			  l;				\
			})
#  elif defined(__powerpc)
#   define LE_FETCH32(a)	({ register unsigned int l;	\
				asm (			\
				"lwbrx %0,0,%1"		\
				: "=r"(l)		\
				: "r"(a));		\
			   l;				\
			})

#  elif defined(__sparc) && defined(OPENSSL_SYS_ULTRASPARC)
#  define LE_FETCH32(a)	({ register unsigned int l;		\
				asm (				\
				"lda [%1]#ASI_PRIMARY_LITTLE,%0"\
				: "=r"(l)			\
				: "r"(a));			\
			   l;					\
			})
#  endif
# endif
#endif /* PEDANTIC */

#if HASH_LONG_LOG2==2	/* Engage only if sizeof(HASH_LONG)== 4 */
/* A nice byte order reversal from Wei Dai <weidai@eskimo.com> */
#ifdef ROTATE
/* 5 instructions with rotate instruction, else 9 */
#define REVERSE_FETCH32(a,l)	(					\
		l=*(const HASH_LONG *)(a),				\
		((ROTATE(l,8)&0x00FF00FF)|(ROTATE((l&0x00FF00FF),24)))	\
				)
#else
/* 6 instructions with rotate instruction, else 8 */
#define REVERSE_FETCH32(a,l)	(				\
		l=*(const HASH_LONG *)(a),			\
		l=(((l>>8)&0x00FF00FF)|((l&0x00FF00FF)<<8)),	\
		ROTATE(l,16)					\
				)
/*
 * Originally the middle line started with l=(((l&0xFF00FF00)>>8)|...
 * It's rewritten as above for two reasons:
 *	- RISCs aren't good at long constants and have to explicitely
 *	  compose 'em with several (well, usually 2) instructions in a
 *	  register before performing the actual operation and (as you
 *	  already realized:-) having same constant should inspire the
 *	  compiler to permanently allocate the only register for it;
 *	- most modern CPUs have two ALUs, but usually only one has
 *	  circuitry for shifts:-( this minor tweak inspires compiler
 *	  to schedule shift instructions in a better way...
 *
 *				<appro@fy.chalmers.se>
 */
#endif
#endif

#ifndef ROTATE
#define ROTATE(a,n)     (((a)<<(n))|(((a)&0xffffffff)>>(32-(n))))
#endif

/*
 * Make some obvious choices. E.g., HASH_BLOCK_DATA_ORDER_ALIGNED
 * and HASH_BLOCK_HOST_ORDER ought to be the same if input data
 * and host are of the same "endianess". It's possible to mask
 * this with blank #define HASH_BLOCK_DATA_ORDER though...
 *
 *				<appro@fy.chalmers.se>
 */
#if defined(B_ENDIAN)
#  if defined(DATA_ORDER_IS_BIG_ENDIAN)
#    if !defined(HASH_BLOCK_DATA_ORDER_ALIGNED) && HASH_LONG_LOG2==2
#      define HASH_BLOCK_DATA_ORDER_ALIGNED	HASH_BLOCK_HOST_ORDER
#    endif
#  elif defined(DATA_ORDER_IS_LITTLE_ENDIAN)
#    ifndef HOST_FETCH32
#      ifdef LE_FETCH32
#        define HOST_FETCH32(p,l)	LE_FETCH32(p)
#      elif defined(REVERSE_FETCH32)
#        define HOST_FETCH32(p,l)	REVERSE_FETCH32(p,l)
#      endif
#    endif
#  endif
#elif defined(L_ENDIAN)
#  if defined(DATA_ORDER_IS_LITTLE_ENDIAN)
#    if !defined(HASH_BLOCK_DATA_ORDER_ALIGNED) && HASH_LONG_LOG2==2
#      define HASH_BLOCK_DATA_ORDER_ALIGNED	HASH_BLOCK_HOST_ORDER
#    endif
#  elif defined(DATA_ORDER_IS_BIG_ENDIAN)
#    ifndef HOST_FETCH32
#      ifdef BE_FETCH32
#        define HOST_FETCH32(p,l)	BE_FETCH32(p)
#      elif defined(REVERSE_FETCH32)
#        define HOST_FETCH32(p,l)	REVERSE_FETCH32(p,l)
#      endif
#    endif
#  endif
#endif

#if !defined(HASH_BLOCK_DATA_ORDER_ALIGNED)
#ifndef HASH_BLOCK_DATA_ORDER
#error "HASH_BLOCK_DATA_ORDER must be defined!"
#endif
#endif

#if defined(DATA_ORDER_IS_BIG_ENDIAN)

#define HOST_c2l(c,l)	(l =(((unsigned long)(*((c)++)))<<24),		\
			 l|=(((unsigned long)(*((c)++)))<<16),		\
			 l|=(((unsigned long)(*((c)++)))<< 8),		\
			 l|=(((unsigned long)(*((c)++)))    ),		\
			 l)
#define HOST_p_c2l(c,l,n)	{					\
			switch (n) {					\
			case 0: l =((unsigned long)(*((c)++)))<<24;	\
			case 1: l|=((unsigned long)(*((c)++)))<<16;	\
			case 2: l|=((unsigned long)(*((c)++)))<< 8;	\
			case 3: l|=((unsigned long)(*((c)++)));		\
				} }
#define HOST_p_c2l_p(c,l,sc,len) {					\
			switch (sc) {					\
			case 0: l =((unsigned long)(*((c)++)))<<24;	\
				if (--len == 0) break;			\
			case 1: l|=((unsigned long)(*((c)++)))<<16;	\
				if (--len == 0) break;			\
			case 2: l|=((unsigned long)(*((c)++)))<< 8;	\
				} }
/* NOTE the pointer is not incremented at the end of this */
#define HOST_c2l_p(c,l,n)	{					\
			l=0; (c)+=n;					\
			switch (n) {					\
			case 3: l =((unsigned long)(*(--(c))))<< 8;	\
			case 2: l|=((unsigned long)(*(--(c))))<<16;	\
			case 1: l|=((unsigned long)(*(--(c))))<<24;	\
				} }
#define HOST_l2c(l,c)	(*((c)++)=(unsigned char)(((l)>>24)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>>16)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>> 8)&0xff),	\
			 *((c)++)=(unsigned char)(((l)    )&0xff),	\
			 l)

#elif defined(DATA_ORDER_IS_LITTLE_ENDIAN)

#define HOST_c2l(c,l)	(l =(((unsigned long)(*((c)++)))    ),		\
			 l|=(((unsigned long)(*((c)++)))<< 8),		\
			 l|=(((unsigned long)(*((c)++)))<<16),		\
			 l|=(((unsigned long)(*((c)++)))<<24),		\
			 l)
#define HOST_p_c2l(c,l,n)	{					\
			switch (n) {					\
			case 0: l =((unsigned long)(*((c)++)));		\
			case 1: l|=((unsigned long)(*((c)++)))<< 8;	\
			case 2: l|=((unsigned long)(*((c)++)))<<16;	\
			case 3: l|=((unsigned long)(*((c)++)))<<24;	\
				} }
#define HOST_p_c2l_p(c,l,sc,len) {					\
			switch (sc) {					\
			case 0: l =((unsigned long)(*((c)++)));		\
				if (--len == 0) break;			\
			case 1: l|=((unsigned long)(*((c)++)))<< 8;	\
				if (--len == 0) break;			\
			case 2: l|=((unsigned long)(*((c)++)))<<16;	\
				} }
/* NOTE the pointer is not incremented at the end of this */
#define HOST_c2l_p(c,l,n)	{					\
			l=0; (c)+=n;					\
			switch (n) {					\
			case 3: l =((unsigned long)(*(--(c))))<<16;	\
			case 2: l|=((unsigned long)(*(--(c))))<< 8;	\
			case 1: l|=((unsigned long)(*(--(c))));		\
				} }
#define HOST_l2c(l,c)	(*((c)++)=(unsigned char)(((l)    )&0xff),	\
			 *((c)++)=(unsigned char)(((l)>> 8)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>>16)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>>24)&0xff),	\
			 l)

#endif

/*
 * Time for some action:-)
 */



#ifndef MD32_REG_T
#define MD32_REG_T long
/*
 * This comment was originaly written for MD5, which is why it
 * discusses A-D. But it basically applies to all 32-bit digests,
 * which is why it was moved to common header file.
 *
 * In case you wonder why A-D are declared as long and not
 * as MD5_LONG. Doing so results in slight performance
 * boost on LP64 architectures. The catch is we don't
 * really care if 32 MSBs of a 64-bit register get polluted
 * with eventual overflows as we *save* only 32 LSBs in
 * *either* case. Now declaring 'em long excuses the compiler
 * from keeping 32 MSBs zeroed resulting in 13% performance
 * improvement under SPARC Solaris7/64 and 5% under AlphaLinux.
 * Well, to be honest it should say that this *prevents* 
 * performance degradation.
 *				<appro@fy.chalmers.se>
 * Apparently there're LP64 compilers that generate better
 * code if A-D are declared int. Most notably GCC-x86_64
 * generates better code.
 *				<appro@fy.chalmers.se>
 */
#endif

#endif
