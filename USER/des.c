#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "des.h"


int encrypt ( unsigned char key[8], unsigned char* data, int blocks )
{
   if ((!data)||(blocks<1))
	  return 0;
   deskey ( key, ENCRYPT );
   des ( data, data, blocks);
   return 1;
}

int decrypt ( unsigned char key[8], unsigned char* data, int blocks )
{
   if ((!data)||(blocks<1))
	  return 0;
   deskey ( key, DECRYPT );
   des ( data, data, blocks);
   return 1;
}

int yencrypt ( unsigned char key[8], unsigned char* data, int size )
{
	char lastChar;
	int  blocks;
	if ((!data)||(size<1))
	  return 0;
	
	// The last char of data is bitwise complemented and filled the rest
	// buffer.If size is 16, it will extend to 24,and 17 still 24.
	lastChar = *(data+size-1);
	blocks = size/8+1;
	memset (data+size, ~lastChar, blocks*8-size);
	deskey ( key, ENCRYPT );
	return encrypt ( data, data, blocks);
}

int ydecrypt ( unsigned char key[8], unsigned char* data, int blocks, int* size )
{
	if ( (!data) || (blocks<1) )
	  return 0;
	
	deskey ( key, DECRYPT );
	if ( !decrypt ( data, data, blocks) )
	  return 0;
	if ( size != 0 )
	{
	  int pos = blocks*8-1;
	  char endChar = data[pos];
	  while ((pos>0)&&(data[pos]==endChar))
	        pos--;
	  if ( data[pos] != ~endChar )
	     return 0;
	  *size = pos+1;
	}
	return 1;
}

// -----------------------------------------------------------------------
// des
//      Encrpts/Decrypts(according to the key currently loaded int the
//      internal key register) SOME blocks of eight bytes at address 'in'
//      into the block at address 'out'. They can be the same.
//
//      "in"
//      "out"
//      "block"  Number of blocks.
// -----------------------------------------------------------------------
void des ( unsigned char* in, unsigned char* out, int blocks )
{ 
	int i=0;
	for ( i= 0; i < blocks; i++,in+=8,out+=8)
		des_block(in,out);
}

// -----------------------------------------------------------------------
// des_block
//      Encrpts/Decrypts(according to the key currently loaded int the
//      internal key register) one block of eight bytes at address 'in'
//      into the block at address 'out'. They can be the same.
//
//      "in"
//      "out"
// -----------------------------------------------------------------------
void des_block(unsigned char *in, unsigned char *out)
{
  unsigned long work[2];

  scrunch(in, work);
  desfunc(work, KnL);
  unscrun(work, out);
}

// ----------------------------------------------------------------------
// deskey
//       Sets the internal key register (KnR) according to the hexadecimal
//       key contained in the 8 bytes of hexkey, according to the DES,
//       for encryption or decrytion according to MODE
//
//       "key" is the 64 bits key.
//       "md"  means encryption or decryption.
// ----------------------------------------------------------------------
void deskey(unsigned char key[8], Mode md)  /* Thanks to James Gillogly &amp; Phil Karn! */
{
  register int i, j, l, m, n;
  unsigned char pc1m[56], pcr[56];
  unsigned long kn[32];

  for (j = 0; j < 56; j++) 
  {
    l = pc1[j];
    m = l & 07;
    pc1m[j] = (key[l >> 3] & bytebit[m]) ? 1:0;
  }
 
  for (i = 0; i < 16; i++) 
  {
		if (md == DECRYPT) 
			m = (15 - i) << 1;
		else 
			m = i << 1;
		n = m + 1;
		kn[m] = kn[n] = 0L;
		for (j = 0; j < 28; j++) 
		{
			l = j + totrot[i];
			if (l < 28) 
				pcr[j] = pc1m[l];
			else 
				pcr[j] = pc1m[l - 28];
		}
		for (j = 28; j < 56; j++) 
		{
			l = j + totrot[i];
			if (l < 56) 
				pcr[j] = pc1m[l];
			else 
				pcr[j] = pc1m[l - 28];
		}
		for (j = 0; j < 24; j++) 
		{
			if (pcr[ pc2[j] ]) kn[m] |= bigbyte[j];
			if (pcr[ pc2[j+24] ]) kn[n] |= bigbyte[j];
		}
  }
  cookey(kn);
  return;
}

// ----------------------------------------------------------------------
// cookey
//       Only called by deskey.
// -----------------------------------------------------------------------
void cookey(register unsigned long *raw1)
{
  register unsigned long *cook, *raw0;
  unsigned long dough[32];
  register int i;

  cook = dough;
  for (i = 0; i < 16; i++, raw1++) {
    raw0 = raw1++;
    *cook = (*raw0 & 0x00fc0000L) << 6;
    *cook |= (*raw0 & 0x00000fc0L) << 10;
    *cook |= (*raw1 & 0x00fc0000L) >> 10;
    *cook++ |= (*raw1 & 0x00000fc0L) >> 6;
    *cook = (*raw0 & 0x0003f000L) << 12;
    *cook |= (*raw0 & 0x0000003fL) << 16;
    *cook |= (*raw1 & 0x0003f000L) >> 4;
    *cook++ |= (*raw1 & 0x0000003fL);
  }
  usekey(dough);
  return;
}

// ----------------------------------------------------------------------
// usekey
//       Only called by cookey.
//       Loads the interal key register with the data in cookedkey.
// -----------------------------------------------------------------------
void usekey(register unsigned long *from)
{
  register unsigned long *to, *endp;

  to = KnL, endp = &KnL[32];
  while (to < endp) *to++ = *from++;
  return;
}

void scrunch(register unsigned char *outof, register unsigned long *into )
{
  *into = (*outof++ & 0xffL) << 24;
  *into |= (*outof++ & 0xffL) << 16;
  *into |= (*outof++ & 0xffL) << 8;
  *into++ |= (*outof++ & 0xffL);
  *into = (*outof++ & 0xffL) << 24;
  *into |= (*outof++ & 0xffL) << 16;
  *into |= (*outof++ & 0xffL) << 8;
  *into |= (*outof & 0xffL);
  return;
}

void unscrun(register unsigned long *outof, register unsigned char *into)
{
  *into++ = (*outof >> 24) & 0xffL;
  *into++ = (*outof >> 16) & 0xffL;
  *into++ = (*outof >> 8) & 0xffL;
  *into++ = *outof++ & 0xffL;
  *into++ = (*outof >> 24) & 0xffL;
  *into++ = (*outof >> 16) & 0xffL;
  *into++ = (*outof >> 8) & 0xffL;
  *into = *outof & 0xffL;
  return;
}

void desfunc(register unsigned long *block,register unsigned long *keys)
{
  register unsigned long fval, work, right, leftt;
  register int round;

  leftt = block[0];
  right = block[1];
  work = ((leftt >> 4) ^ right) & 0x0f0f0f0fL;
  right ^= work;
  leftt ^= (work << 4);
  work = ((leftt >> 16) ^ right) & 0x0000ffffL;
  right ^= work;
  leftt ^= (work << 16);
  work = ((right >> 2) ^ leftt) & 0x33333333L;
  leftt ^= work;
  right ^= (work << 2);
  work = ((right >> 8) ^ leftt) & 0x00ff00ffL;
  leftt ^= work;
  right ^= (work << 8);
  right = ((right << 1) | ((right >> 31) & 1L)) & 0xffffffffL;
  work = (leftt ^ right) & 0xaaaaaaaaL;
  leftt ^= work;
  right ^= work;
  leftt = ((leftt << 1) | ((leftt >> 31) & 1L)) & 0xffffffffL;

  for (round = 0; round < 8; round++) {
	work = (right << 28) | (right >> 4);
	work ^= *keys++;
	fval  = SP7[work         & 0x3fL];
	fval |= SP5[(work >>  8) & 0x3fL];
	fval |= SP3[(work >> 16) & 0x3fL];
	fval |= SP1[(work >> 24) & 0x3fL];
	work = right ^ *keys++;
	fval |= SP8[work         & 0x3fL];
	fval |= SP6[(work >>  8) & 0x3fL];
	fval |= SP4[(work >> 16) & 0x3fL];
	fval |= SP2[(work >> 24) & 0x3fL];
	leftt ^= fval;
	work = (leftt << 28) | (leftt >> 4);
	work ^= *keys++;
	fval  = SP7[work         & 0x3fL];
	fval |= SP5[(work >>  8) & 0x3fL];
	fval |= SP3[(work >> 16) & 0x3fL];
	fval |= SP1[(work >> 24) & 0x3fL];
	work = leftt ^ *keys++;
	fval |= SP8[work         & 0x3fL];
	fval |= SP6[(work >>  8) & 0x3fL];
	fval |= SP4[(work >> 16) & 0x3fL];
    fval |= SP2[(work >> 24) & 0x3fL];
    right ^= fval;
  }
  right = (right << 31) | (right >> 1);
  work = (leftt ^ right) & 0xaaaaaaaaL;
  leftt ^= work;
  right ^= work;
  leftt = (leftt << 31) | ( leftt >> 1);
  work = ((leftt >> 8) ^ right) & 0x00ff00ffL;
  right ^= work;
  leftt ^= (work << 8);
  work = ((leftt >> 2) ^ right) & 0x33333333L;
  right ^= work;
  leftt ^= (work << 2);
  work = ((right >> 16) ^ leftt) & 0x0000ffffL;
  leftt ^= work;
  right ^= (work << 16);
  work = ((right >> 4) ^ leftt) & 0x0f0f0f0fL;
  leftt ^= work;
  right ^= (work << 4);
  *block++ = right;
  *block = leftt;
  return;
}
