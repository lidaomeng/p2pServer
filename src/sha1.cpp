/* -*- buffer-read-only: t -*- vi: set ro: */
/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
#line 1
/* sha1.c - Functions to compute SHA1 message digest of files or
   memory blocks according to the NIST specification FIPS-180-1.

   Copyright (C) 2000, 2001, 2003, 2004, 2005, 2006, 2008 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Scott G. Miller
   Credits:
      Robert Klep <robert@ilse.nl>  -- Expansion function fix
*/
/*
*@file:sha1.cpp
*@brief:��ϣУ�飬�������⣬������Щ�޸ġ�
*@author: zhangmiao@oristartech.com
*@date: 2012/06/05
*/

#include "sha1.h"

#include <stddef.h>
#include <string.h>

//#if USE_UNLOCKED_IO
//#include "unlocked-io.h"
//#endif

#ifdef WORDS_BIGENDIAN
# define SWAP(n) (n)
#else
# define SWAP(n) \
    (((n) << 24) | (((n) & 0xff00) << 8) | (((n) >> 8) & 0xff00) | ((n) >> 24))
#endif

#define BLOCKSIZE 4096
#if BLOCKSIZE % 64 != 0
# error "invalid BLOCKSIZE"
#endif

/* This array contains the bytes used to pad the buffer to the next
   64-byte boundary.  (RFC 1321, 3.1: Step 1)  */
static const unsigned char fillbuf[64] = { 0x80, 0 /* , 0, 0, ...  */ };

//@author zhangmiao@oristartech.com
//@date [2012-07-10]
//@brief ���塢��������hashУ���ȫ�ֱ���
//@new
int g_flag_SHA1CheckStop = 0;
unsigned long long g_AllFile_Size = 0ull;
unsigned long long g_hasHashChecked_Size = 0ull;
//����������
#ifdef WIN32
#include <windows.h>
extern HANDLE hMutex;
#else
#include <pthread.h>
extern pthread_mutex_t work_mutex;
#endif
//@modify end;

/* Take a pointer to a 160 bit block of data (five 32 bit ints) and
   initialize it to the start constants of the SHA1 algorithm.  This
   must be called before using hash in the call to sha1_hash.  */
void sha1_init_ctx (struct sha1_ctx *ctx)
{
  ctx->A = 0x67452301;
  ctx->B = 0xefcdab89;
  ctx->C = 0x98badcfe;
  ctx->D = 0x10325476;
  ctx->E = 0xc3d2e1f0;

  ctx->total[0] = ctx->total[1] = 0;
  ctx->buflen = 0;
}

/* Copy the 4 byte value from v into the memory location pointed to by *cp,
   If your architecture allows unaligned access this is equivalent to
   * (uint32_t *) cp = v  */
static inline void set_uint32 (char *cp, uint32_t v)
{
	memcpy (cp, &v, sizeof v);
}

/* Put result from CTX in first 20 bytes following RESBUF.  The result
   must be in little endian byte order.  */
void* sha1_read_ctx (const struct sha1_ctx *ctx, void *resbuf)
{
	char *r =(char *)resbuf;
	set_uint32 (r + 0 * sizeof ctx->A, SWAP (ctx->A));
	set_uint32 (r + 1 * sizeof ctx->B, SWAP (ctx->B));
	set_uint32 (r + 2 * sizeof ctx->C, SWAP (ctx->C));
	set_uint32 (r + 3 * sizeof ctx->D, SWAP (ctx->D));
	set_uint32 (r + 4 * sizeof ctx->E, SWAP (ctx->E));

	return resbuf;
}

/* Process the remaining bytes in the internal buffer and the usual
   prolog according to the standard and write the result to RESBUF.  */
void* sha1_finish_ctx (struct sha1_ctx *ctx, void *resbuf)
{
	/* Take yet unprocessed bytes into account.  */
	uint32_t bytes = ctx->buflen;
	size_t size = (bytes < 56) ? 64 / 4 : 64 * 2 / 4;

	/* Now count remaining bytes.  */
	ctx->total[0] += bytes;
	if (ctx->total[0] < bytes)
	++ctx->total[1];

	/* Put the 64-bit file length in *bits* at the end of the buffer.  */
	ctx->buffer[size - 2] = SWAP ((ctx->total[1] << 3) | (ctx->total[0] >> 29));
	ctx->buffer[size - 1] = SWAP (ctx->total[0] << 3);

	memcpy (&((char *) ctx->buffer)[bytes], fillbuf, (size - 2) * 4 - bytes);

	/* Process last bytes.  */
	sha1_process_block (ctx->buffer, size * 4, ctx);

	return sha1_read_ctx (ctx, resbuf);
}

/* Compute SHA1 message digest for bytes read from STREAM.  The
   resulting message digest number will be written into the 16 bytes
   beginning at RESBLOCK.  */
int sha1_stream (FILE *stream, void *resblock,
                unsigned long long *hasCpSize,int *stopCpFlag,int flag_cpucontl)
{
	struct sha1_ctx ctx;
	char buffer[BLOCKSIZE + 72];
	size_t sum;
	//unsigned long long fSize = 0llu;
	
	/* Initialize the computation context.  */
	sha1_init_ctx (&ctx);

#if 0
	struct cpu_usage_struct *cur,*old;   
	double use_rate,free_rate;   
	old = (struct cpu_usage_struct*)malloc(sizeof(struct cpu_usage_struct));   
	if(old == NULL)   
	{   
	  perror("malloc error");   
	  return -1;   
	}   
	cur = (struct cpu_usage_struct*)malloc(sizeof(struct cpu_usage_struct));   
	if(cur == NULL)   
	{   
	  free(old);
	  perror("malloc error");   
	  return -1;   
	}   
	get_cpuinfo_from_proc_stat(old);
#endif
	/* Iterate over full file contents.  */
	/*CPURate cpuRate;*/
	//printf("flag=%d\n",flag_cpucontl);
	/*if(flag_cpucontl==1)
	{
	  cpuRate.init();
	}
	*/

	while (1)
	{
		/*if(flag_cpucontl==1)
		cpuRate.controlRate(88.0);*/

		/* We read the file in blocks of BLOCKSIZE bytes.  One call of the
		computation function processes the whole buffer so that with the
		next round of the loop another block can be read.  */
		size_t ckn;
		sum = 0;

		//get_cpuinfo_from_proc_stat(cur);   
		// use_rate = get_cpu_use_rate(cur,old);   
		//printf("use_rate:%lf\n",use_rate);   
		//if(use_rate>20) 
		//{usleep(50);
		  //printf("use_rate:%lf\n",use_rate);
		//}
		
		/* Read block.  Take care for partial reads.  */
		while (1)
		{

//---zhangmiao:begin---[7/10/2012]----
//@author zhangmiao@oristartech.com
//@date [2012-07-10]
//@brief ����ȡ��hashУ�鹦��
//@new
			if( g_flag_SHA1CheckStop == 1 )
			{
				//����stopCpFlag����Ϊ����ֵ�����ڷ���״ֵ̬��hashУ���Ƿ�ȡ��;
				//�����ȡ�����򷵻�ֵ����=1�����򷵻�ֵ����=0��
				if( stopCpFlag != NULL && *stopCpFlag == 0 )
				{
					*stopCpFlag = 1;
				}

#ifdef _TEST_
				printf("sha1 check received the stop signal! Code is %d\n",CODE_SHA1CHECKCANCEL);
#endif
				errno = CODE_SHA1CHECKCANCEL;
				return CODE_SHA1CHECKCANCEL;
			}
//@modify end;
//---zhangmiao:end--[7/10/2012]--

			if(stopCpFlag!=NULL && *stopCpFlag==1)
			{
				//free(old);free(cur);
				printf("sha1 check received the stop signal!\n");
				return 1;
			}

			ckn = fread (buffer + sum, 1, BLOCKSIZE - sum, stream);

			if(hasCpSize!=NULL)*hasCpSize += ckn;
			//if(g_hasCp_Size>= g_File_Size) printf("n=%d\n",n);
			//fSize += n;
//---zhangmiao:begin---[7/10/2012]----
//@author zhangmiao@oristartech.com
//@date [2012-07-10]
//@brief ���ӻ�ȡhashУ����ȹ���
//@new
#ifdef WIN32
			WaitForSingleObject(hMutex,INFINITE);
#else
			pthread_mutex_lock(&work_mutex);
#endif

			g_hasHashChecked_Size += ckn;

#ifdef WIN32
			ReleaseMutex(hMutex);
#else
			pthread_mutex_unlock(&work_mutex);
#endif
//@modify end;
//---zhangmiao:end--[7/10/2012]--

			sum += ckn;

			if (sum == BLOCKSIZE)
			break;

			if (ckn == 0)
			{
				/* Check for the error flag IFF N == 0, so that we don't
				 exit the loop after a partial read due to e.g., EAGAIN
				 or EWOULDBLOCK.  */
			  if (ferror (stream))
			  {
				 // free(old);free(cur);
				  return 1;
			  }
			  goto process_partial_block;
			}

		  /* We've read at least one byte, so ignore errors.  But always
			 check for EOF, since feof may be true even though N > 0.
			 Otherwise, we could end up calling fread after EOF.  */
			if (feof (stream))
				goto process_partial_block;
	     }

	  /* Process buffer with BLOCKSIZE bytes.  Note that
			BLOCKSIZE % 64 == 0
	   */
		sha1_process_block (buffer, BLOCKSIZE, &ctx);
	}

	process_partial_block:;
	//free(old);free(cur);
	/* Process any remaining bytes.  */
	if (sum > 0)
		sha1_process_bytes (buffer, sum, &ctx);

	/* Construct result in desired memory.  */
	sha1_finish_ctx (&ctx, resblock);
	//printf("fSize=%llu\n",fSize);
	return 0;
}

/* Compute SHA1 message digest for LEN bytes beginning at BUFFER.  The
   result is always in little endian byte order, so that a byte-wise
   output yields to the wanted ASCII representation of the message
   digest.  */
void * sha1_buffer (const char *buffer, size_t len, void *resblock)
{
	struct sha1_ctx ctx;

	/* Initialize the computation context.  */
	sha1_init_ctx (&ctx);

	/* Process whole buffer but last len % 64 bytes.  */
	sha1_process_bytes (buffer, len, &ctx);

	/* Put result in desired memory area.  */
	return sha1_finish_ctx (&ctx, resblock);
}

void sha1_process_bytes (const void *buffer, size_t len, struct sha1_ctx *ctx)
{
  /* When we already have some bits in our internal buffer concatenate
     both inputs first.  */
  if (ctx->buflen != 0)
    {
      size_t left_over = ctx->buflen;
      size_t add = 128 - left_over > len ? len : 128 - left_over;

      memcpy (&((char *) ctx->buffer)[left_over], buffer, add);
      ctx->buflen += add;

      if (ctx->buflen > 64)
	{
	  sha1_process_block (ctx->buffer, ctx->buflen & ~63, ctx);

	  ctx->buflen &= 63;
	  /* The regions in the following copy operation cannot overlap.  */
	  memcpy (ctx->buffer,
		  &((char *) ctx->buffer)[(left_over + add) & ~63],
		  ctx->buflen);
	}

      buffer = (const char *) buffer + add;
      len -= add;
    }

  /* Process available complete blocks.  */
  if (len >= 64)
    {
//#if !_STRING_ARCH_unaligned
#if 0
# define alignof(type) offsetof (struct { char c; type x; }, x)
# define UNALIGNED_P(p) (((size_t) p) % alignof (uint32_t) != 0)
      if (UNALIGNED_P (buffer))
	while (len > 64)
	  {
	    sha1_process_block (memcpy (ctx->buffer, buffer, 64), 64, ctx);
	    buffer = (const char *) buffer + 64;
	    len -= 64;
	  }
      else
#endif
	{
	  sha1_process_block (buffer, len & ~63, ctx);
	  buffer = (const char *) buffer + (len & ~63);
	  len &= 63;
	}
    }

  /* Move remaining bytes in internal buffer.  */
	if (len > 0)
    {
		size_t left_over = ctx->buflen;

		memcpy (&((char *) ctx->buffer)[left_over], buffer, len);
		left_over += len;
		if (left_over >= 64)
		{
			sha1_process_block (ctx->buffer, 64, ctx);
			left_over -= 64;
			memcpy (ctx->buffer, &ctx->buffer[16], left_over);
		}
		ctx->buflen = left_over;
    }
}

/* --- Code below is the primary difference between md5.c and sha1.c --- */

/* SHA1 round constants */
#define K1 0x5a827999
#define K2 0x6ed9eba1
#define K3 0x8f1bbcdc
#define K4 0xca62c1d6

/* Round functions.  Note that F2 is the same as F4.  */
#define F1(B,C,D) ( D ^ ( B & ( C ^ D ) ) )
#define F2(B,C,D) (B ^ C ^ D)
#define F3(B,C,D) ( ( B & C ) | ( D & ( B | C ) ) )
#define F4(B,C,D) (B ^ C ^ D)

/* Process LEN bytes of BUFFER, accumulating context into CTX.
   It is assumed that LEN % 64 == 0.
   Most of this code comes from GnuPG's cipher/sha1.c.  */

void sha1_process_block (const void *buffer, size_t len, struct sha1_ctx *ctx)
{
  const uint32_t *words = (uint32_t *)buffer;
  size_t nwords = len / sizeof (uint32_t);
  const uint32_t *endp = words + nwords;
  uint32_t x[16];
  uint32_t a = ctx->A;
  uint32_t b = ctx->B;
  uint32_t c = ctx->C;
  uint32_t d = ctx->D;
  uint32_t e = ctx->E;

  /* First increment the byte count.  RFC 1321 specifies the possible
     length of the file up to 2^64 bits.  Here we only compute the
     number of bytes.  Do a double word increment.  */
  ctx->total[0] += len;
  if (ctx->total[0] < len)
    ++ctx->total[1];

#define rol(x, n) (((x) << (n)) | ((uint32_t) (x) >> (32 - (n))))

#define M(I) ( tm =   x[I&0x0f] ^ x[(I-14)&0x0f] \
		    ^ x[(I-8)&0x0f] ^ x[(I-3)&0x0f] \
	       , (x[I&0x0f] = rol(tm, 1)) )

#define R(A,B,C,D,E,F,K,M)  do { E += rol( A, 5 )     \
				      + F( B, C, D )  \
				      + K	      \
				      + M;	      \
				 B = rol( B, 30 );    \
			       } while(0)

  while (words < endp)
    {
      uint32_t tm;
      int t;
      for (t = 0; t < 16; t++)
	{
	  x[t] = SWAP (*words);
	  words++;
	}

      R( a, b, c, d, e, F1, K1, x[ 0] );
      R( e, a, b, c, d, F1, K1, x[ 1] );
      R( d, e, a, b, c, F1, K1, x[ 2] );
      R( c, d, e, a, b, F1, K1, x[ 3] );
      R( b, c, d, e, a, F1, K1, x[ 4] );
      R( a, b, c, d, e, F1, K1, x[ 5] );
      R( e, a, b, c, d, F1, K1, x[ 6] );
      R( d, e, a, b, c, F1, K1, x[ 7] );
      R( c, d, e, a, b, F1, K1, x[ 8] );
      R( b, c, d, e, a, F1, K1, x[ 9] );
      R( a, b, c, d, e, F1, K1, x[10] );
      R( e, a, b, c, d, F1, K1, x[11] );
      R( d, e, a, b, c, F1, K1, x[12] );
      R( c, d, e, a, b, F1, K1, x[13] );
      R( b, c, d, e, a, F1, K1, x[14] );
      R( a, b, c, d, e, F1, K1, x[15] );
      R( e, a, b, c, d, F1, K1, M(16) );
      R( d, e, a, b, c, F1, K1, M(17) );
      R( c, d, e, a, b, F1, K1, M(18) );
      R( b, c, d, e, a, F1, K1, M(19) );
      R( a, b, c, d, e, F2, K2, M(20) );
      R( e, a, b, c, d, F2, K2, M(21) );
      R( d, e, a, b, c, F2, K2, M(22) );
      R( c, d, e, a, b, F2, K2, M(23) );
      R( b, c, d, e, a, F2, K2, M(24) );
      R( a, b, c, d, e, F2, K2, M(25) );
      R( e, a, b, c, d, F2, K2, M(26) );
      R( d, e, a, b, c, F2, K2, M(27) );
      R( c, d, e, a, b, F2, K2, M(28) );
      R( b, c, d, e, a, F2, K2, M(29) );
      R( a, b, c, d, e, F2, K2, M(30) );
      R( e, a, b, c, d, F2, K2, M(31) );
      R( d, e, a, b, c, F2, K2, M(32) );
      R( c, d, e, a, b, F2, K2, M(33) );
      R( b, c, d, e, a, F2, K2, M(34) );
      R( a, b, c, d, e, F2, K2, M(35) );
      R( e, a, b, c, d, F2, K2, M(36) );
      R( d, e, a, b, c, F2, K2, M(37) );
      R( c, d, e, a, b, F2, K2, M(38) );
      R( b, c, d, e, a, F2, K2, M(39) );
      R( a, b, c, d, e, F3, K3, M(40) );
      R( e, a, b, c, d, F3, K3, M(41) );
      R( d, e, a, b, c, F3, K3, M(42) );
      R( c, d, e, a, b, F3, K3, M(43) );
      R( b, c, d, e, a, F3, K3, M(44) );
      R( a, b, c, d, e, F3, K3, M(45) );
      R( e, a, b, c, d, F3, K3, M(46) );
      R( d, e, a, b, c, F3, K3, M(47) );
      R( c, d, e, a, b, F3, K3, M(48) );
      R( b, c, d, e, a, F3, K3, M(49) );
      R( a, b, c, d, e, F3, K3, M(50) );
      R( e, a, b, c, d, F3, K3, M(51) );
      R( d, e, a, b, c, F3, K3, M(52) );
      R( c, d, e, a, b, F3, K3, M(53) );
      R( b, c, d, e, a, F3, K3, M(54) );
      R( a, b, c, d, e, F3, K3, M(55) );
      R( e, a, b, c, d, F3, K3, M(56) );
      R( d, e, a, b, c, F3, K3, M(57) );
      R( c, d, e, a, b, F3, K3, M(58) );
      R( b, c, d, e, a, F3, K3, M(59) );
      R( a, b, c, d, e, F4, K4, M(60) );
      R( e, a, b, c, d, F4, K4, M(61) );
      R( d, e, a, b, c, F4, K4, M(62) );
      R( c, d, e, a, b, F4, K4, M(63) );
      R( b, c, d, e, a, F4, K4, M(64) );
      R( a, b, c, d, e, F4, K4, M(65) );
      R( e, a, b, c, d, F4, K4, M(66) );
      R( d, e, a, b, c, F4, K4, M(67) );
      R( c, d, e, a, b, F4, K4, M(68) );
      R( b, c, d, e, a, F4, K4, M(69) );
      R( a, b, c, d, e, F4, K4, M(70) );
      R( e, a, b, c, d, F4, K4, M(71) );
      R( d, e, a, b, c, F4, K4, M(72) );
      R( c, d, e, a, b, F4, K4, M(73) );
      R( b, c, d, e, a, F4, K4, M(74) );
      R( a, b, c, d, e, F4, K4, M(75) );
      R( e, a, b, c, d, F4, K4, M(76) );
      R( d, e, a, b, c, F4, K4, M(77) );
      R( c, d, e, a, b, F4, K4, M(78) );
      R( b, c, d, e, a, F4, K4, M(79) );

      a = ctx->A += a;
      b = ctx->B += b;
      c = ctx->C += c;
      d = ctx->D += d;
      e = ctx->E += e;
    }
}
