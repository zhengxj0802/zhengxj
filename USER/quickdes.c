#include "quickdes.h"

int des_set_key(des_cblock(*key),des_key_schedule schedule)
{
	register DES_LONG c,d,t,s;
	register unsigned char *in;
	register DES_LONG *k;
	register int i;

	k=(DES_LONG *)schedule;
	in=(unsigned char *)key;

	c2l(in,c);
	c2l(in,d);

	PERM_OP (d,c,t,4,0x0f0f0f0fL);
	HPERM_OP(c,t,-2,0xcccc0000L);
	HPERM_OP(d,t,-2,0xcccc0000L);
	PERM_OP (d,c,t,1,0x55555555L);
	PERM_OP (c,d,t,8,0x00ff00ffL);
	PERM_OP (d,c,t,1,0x55555555L);
	d=	(((d&0x000000ffL)<<16L)| (d&0x0000ff00L)     |
		 ((d&0x00ff0000L)>>16L)|((c&0xf0000000L)>>4L));
	c&=0x0fffffffL;

	for (i=0; i<16; i++)
		{
		if (shifts2[i])
			{ c=((c>>2L)|(c<<26L)); d=((d>>2L)|(d<<26L)); }
		else
			{ c=((c>>1L)|(c<<27L)); d=((d>>1L)|(d<<27L)); }
		c&=0x0fffffffL;
		d&=0x0fffffffL;

		s=	des_skb[0][ (c    )&0x3f                ]|
			des_skb[1][((c>> 6)&0x03)|((c>> 7L)&0x3c)]|
			des_skb[2][((c>>13)&0x0f)|((c>>14L)&0x30)]|
			des_skb[3][((c>>20)&0x01)|((c>>21L)&0x06) |
						  ((c>>22L)&0x38)];
		t=	des_skb[4][ (d    )&0x3f                ]|
			des_skb[5][((d>> 7L)&0x03)|((d>> 8L)&0x3c)]|
			des_skb[6][ (d>>15L)&0x3f                ]|
			des_skb[7][((d>>21L)&0x0f)|((d>>22L)&0x30)];

		/* table contained 0213 4657 */
		*(k++)=((t<<16L)|(s&0x0000ffffL))&0xffffffffL;
		s=     ((s>>16L)|(t&0xffff0000L));
		
		s=(s<<4L)|(s>>28L);
		*(k++)=s&0xffffffffL;
		}
	return(0);
}

void des_ecb_encrypt(des_cblock(*input), des_cblock(*output), des_key_schedule ks,int encrypt)
{
	register DES_LONG l=0;
	register unsigned char *in,*out;
	DES_LONG ll[2];

	in=(unsigned char *)input;
	out=(unsigned char *)output;
	c2l(in,l); ll[0]=l;
	c2l(in,l); ll[1]=l;
	des_encrypt(ll,ks,encrypt);
	l=ll[0]; l2c(l,out);
	l=ll[1]; l2c(l,out);
	l=ll[0]=ll[1]=0;
}

void des_encrypt(DES_LONG *data, des_key_schedule ks, int encrypt)
{
	register DES_LONG l,r,u;
	union fudge 
	{
		DES_LONG  l;
		unsigned short s[2];
		unsigned char  c[4];
	} U,T;
	register int i;
	register DES_LONG *s;

	u=data[0];
	r=data[1];

	IP(u,r);

	l=(r<<1)|(r>>31);
	r=(u<<1)|(u>>31);

	/* clear the top bits on machines with 8byte longs */
	l&=0xffffffffL;
	r&=0xffffffffL;

	s=(DES_LONG *)ks;

	if (encrypt)
	{
		for (i=0; i<32; i+=4)
		{
			D_ENCRYPT(l,r,i+0); /*  1 */
			D_ENCRYPT(r,l,i+2); /*  2 */
		}
	}
	else
	{
		for (i=30; i>0; i-=4)
		{
			D_ENCRYPT(l,r,i-0); /* 16 */
			D_ENCRYPT(r,l,i-2); /* 15 */
		}
	}
	l=(l>>1)|(l<<31);
	r=(r>>1)|(r<<31);
	/* clear the top bits on machines with 8byte longs */
	l&=0xffffffffL;
	r&=0xffffffffL;

	FP(r,l);
	data[0]=l;
	data[1]=r;
}

void DES(unsigned char *key,unsigned char *s_text,unsigned char *d_text)
{
	des_key_schedule sch;
	des_set_key((des_cblock *)key,sch);
	des_ecb_encrypt((des_cblock *)s_text,(des_cblock *)d_text,&(sch[0]),1);
}

void _DES(unsigned char *key,unsigned char *s_text,unsigned char *d_text)
{
	des_key_schedule sch;
	des_set_key((des_cblock *)key,sch);
	des_ecb_encrypt((des_cblock *)s_text,(des_cblock *)d_text,&(sch[0]),0);
}
