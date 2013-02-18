#include "stm32f10x.h"
#include "AT45DB161D.h"
#include "12864lcd.h"
#include "comdef.h"
#include "globleexten.h"
#include "common.h"
#include "USART.h"
//��д��ĸ
unsigned char ALPHA_A[] =
{
0x7, 0xf, 0x19, 0x31, 0x19, 0xf, 0x7, 0x0, 0xf0, 0xf0, 0x0, 0x0, 0x0, 0xf0, 0xf0, 0x0, 
};

unsigned char ALPHA_B[] =
{
0x20, 0x3f, 0x3f, 0x22, 0x22, 0x3f, 0x1d, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};

unsigned char ALPHA_C[] =
{
0xf, 0x1f, 0x30, 0x20, 0x20, 0x30, 0x18, 0x0, 0xc0, 0xe0, 0x30, 0x10, 0x10, 0x30, 0x60, 0x0, 
};

unsigned char ALPHA_D[] =
{
0x20, 0x3f, 0x3f, 0x20, 0x30, 0x1f, 0xf, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x30, 0xe0, 0xc0, 0x0, 
};

unsigned char ALPHA_E[] =
{
0x20, 0x3f, 0x3f, 0x22, 0x27, 0x30, 0x38, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x10, 0x30, 0x70, 0x0, 
};


unsigned char ALPHA_F[] =
{
0x20, 0x3f, 0x3f, 0x22, 0x27, 0x30, 0x38, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 0x0, 0x0, 0x0, 
};


unsigned char ALPHA_G[] =
{
0xf, 0x1f, 0x30, 0x21, 0x21, 0x31, 0x19, 0x0, 0xc0, 0xe0, 0x30, 0x10, 0x10, 0xe0, 0xf0, 0x0, 
};


unsigned char ALPHA_H[] =
{
0x3f, 0x3f, 0x2, 0x2, 0x2, 0x3f, 0x3f, 0x0, 0xf0, 0xf0, 0x0, 0x0, 0x0, 0xf0, 0xf0, 0x0, 
};


unsigned char ALPHA_I[] =
{
0x0, 0x0, 0x20, 0x3f, 0x3f, 0x20, 0x0, 0x0, 0x0, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 0x0, 
};


unsigned char ALPHA_J[] =
{
0x0, 0x0, 0x0, 0x20, 0x3f, 0x3f, 0x20, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0xf0, 0xe0, 0x0, 0x0, 
};


unsigned char ALPHA_K[] =
{
0x20, 0x3f, 0x3f, 0x3, 0x7, 0x3c, 0x38, 0x0, 0x10, 0xf0, 0xf0, 0x0, 0x80, 0xf0, 0x70, 0x0, 
};


unsigned char ALPHA_L[] =
{
0x20, 0x3f, 0x3f, 0x20, 0x0, 0x0, 0x0, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x10, 0x30, 0x70, 0x0, 
};


unsigned char ALPHA_M[] =
{
0x3f, 0x3f, 0x1c, 0xe, 0x1c, 0x3f, 0x3f, 0x0, 0xf0, 0xf0, 0x0, 0x0, 0x0, 0xf0, 0xf0, 0x0, 
};


unsigned char ALPHA_N[] =
{
0x3f, 0x3f, 0x1c, 0xe, 0x7, 0x3f, 0x3f, 0x0, 0xf0, 0xf0, 0x0, 0x0, 0x0, 0xf0, 0xf0, 0x0, 
};


unsigned char ALPHA_O[] =
{
0x1f, 0x3f, 0x20, 0x20, 0x20, 0x3f, 0x1f, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};


unsigned char ALPHA_P[] =
{
0x20, 0x3f, 0x3f, 0x22, 0x22, 0x3e, 0x1c, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 0x0, 0x0, 0x0, 
};


unsigned char ALPHA_Q[] =
{
0x1f, 0x3f, 0x20, 0x20, 0x20, 0x3f, 0x1f, 0x0, 0xe0, 0xf0, 0x10, 0x70, 0x3c, 0xfc, 0xe4, 0x0, 
};


unsigned char ALPHA_R[] =
{
0x20, 0x3f, 0x3f, 0x22, 0x23, 0x3f, 0x1c, 0x0, 0x10, 0xf0, 0xf0, 0x0, 0x0, 0xf0, 0xf0, 0x0, 
};


unsigned char ALPHA_S[] =
{
0x18, 0x3c, 0x26, 0x22, 0x23, 0x39, 0x18, 0x0, 0x60, 0x70, 0x10, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};


unsigned char ALPHA_T[] =
{
0x0, 0x38, 0x30, 0x3f, 0x3f, 0x30, 0x38, 0x0, 0x0, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 0x0, 
};


unsigned char ALPHA_U[] =
{
0x3f, 0x3f, 0x0, 0x0, 0x0, 0x3f, 0x3f, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};


unsigned char ALPHA_V[] =
{
0x3f, 0x3f, 0x0, 0x0, 0x0, 0x3f, 0x3f, 0x0, 0x80, 0xc0, 0x60, 0x30, 0x60, 0xc0, 0x80, 0x0, 
};


unsigned char ALPHA_W[] =
{
0x3f, 0x3f, 0x0, 0x3, 0x0, 0x3f, 0x3f, 0x0, 0xe0, 0xf0, 0x70, 0xc0, 0x70, 0xf0, 0xe0, 0x0, 
};


unsigned char ALPHA_X[] =
{
0x30, 0x3c, 0xf, 0x7, 0xf, 0x3c, 0x30, 0x0, 0x30, 0xf0, 0xc0, 0x80, 0xc0, 0xf0, 0x30, 0x0, 
};


unsigned char ALPHA_Y[] =
{
0x0, 0x3c, 0x3e, 0x3, 0x3, 0x3e, 0x3c, 0x0, 0x0, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 0x0, 
};


unsigned char ALPHA_Z[] =
{
0x38, 0x30, 0x21, 0x23, 0x26, 0x3c, 0x38, 0x0, 0x70, 0xf0, 0x90, 0x10, 0x10, 0x30, 0x70, 0x0, 
};



//Сд��ĸ
unsigned char LOWER_a[] =
{
0x0, 0x5, 0x5, 0x5, 0x7, 0x3, 0x0, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0xe0, 0xf0, 0x10, 0x0, 
};


unsigned char LOWER_b[] =
{
0x20, 0x3f, 0x3f, 0x4, 0x6, 0x3, 0x1, 0x0, 0x0, 0xf0, 0xf0, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};


unsigned char LOWER_c[] =
{
0x3, 0x7, 0x4, 0x4, 0x4, 0x6, 0x2, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0x10, 0x30, 0x20, 0x0, 
};


unsigned char LOWER_d[] =
{
0x1, 0x3, 0x6, 0x24, 0x3f, 0x3f, 0x0, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0xe0, 0xf0, 0x10, 0x0, 
};


unsigned char LOWER_e[] =
{
0x3, 0x7, 0x5, 0x5, 0x5, 0x7, 0x3, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0x10, 0x30, 0x20, 0x0, 
};


unsigned char LOWER_f[] =
{
0x0, 0x2, 0x1f, 0x3f, 0x22, 0x30, 0x18, 0x0, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 0x0, 0x0, 
};


unsigned char LOWER_g[] =
{
0x3, 0x7, 0x4, 0x4, 0x3, 0x7, 0x4, 0x0, 0xe4, 0xf6, 0x12, 0x12, 0xfe, 0xfc, 0x0, 0x0, 
};


unsigned char LOWER_h[] =
{
0x20, 0x3f, 0x3f, 0x2, 0x4, 0x7, 0x3, 0x0, 0x10, 0xf0, 0xf0, 0x0, 0x0, 0xf0, 0xf0, 0x0, 
};


unsigned char LOWER_i[] =
{
0x0, 0x0, 0x4, 0x37, 0x37, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 0x0, 
};


unsigned char LOWER_j[] =
{
0x0, 0x0, 0x0, 0x0, 0x4, 0x37, 0x37, 0x0, 0x0, 0xc, 0xe, 0x2, 0x2, 0xfe, 0xfc, 0x0, 
};


unsigned char LOWER_k[] =
{
0x20, 0x3f, 0x3f, 0x1, 0x3, 0x6, 0x4, 0x0, 0x10, 0xf0, 0xf0, 0x80, 0xc0, 0x70, 0x30, 0x0, 
};


unsigned char LOWER_l[] =
{
0x0, 0x0, 0x20, 0x3f, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 0x0, 
};


unsigned char LOWER_m[] =
{
0x7, 0x7, 0x6, 0x3, 0x6, 0x7, 0x3, 0x0, 0xf0, 0xf0, 0x0, 0xe0, 0x0, 0xf0, 0xf0, 0x0, 
};

unsigned char LOWER_n[] =
{
0x4, 0x7, 0x3, 0x4, 0x4, 0x7, 0x3, 0x0, 0x0, 0xf0, 0xf0, 0x0, 0x0, 0xf0, 0xf0, 0x0, 
};


unsigned char LOWER_o[] =
{
0x3, 0x7, 0x4, 0x4, 0x4, 0x7, 0x3, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};



unsigned char LOWER_p[] =
{
0x4, 0x7, 0x3, 0x4, 0x4, 0x7, 0x3, 0x0, 0x2, 0xfe, 0xfe, 0x12, 0x10, 0xf0, 0xe0, 0x0, 
};


unsigned char LOWER_q[] =
{
0x3, 0x7, 0x4, 0x4, 0x3, 0x7, 0x4, 0x0, 0xe0, 0xf0, 0x10, 0x12, 0xfe, 0xfe, 0x2, 0x0, 
};


unsigned char LOWER_r[] =
{
0x4, 0x7, 0x3, 0x6, 0x4, 0x7, 0x3, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 0x0, 0x0, 0x0, 
};


unsigned char LOWER_s[] =
{
0x2, 0x7, 0x5, 0x4, 0x4, 0x6, 0x2, 0x0, 0x20, 0x30, 0x90, 0x90, 0xd0, 0x70, 0x20, 0x0, 
};


unsigned char LOWER_t[] =
{
0x4, 0x4, 0x1f, 0x3f, 0x4, 0x4, 0x0, 0x0, 0x0, 0x0, 0xe0, 0xf0, 0x10, 0x30, 0x20, 0x0, 
};


unsigned char LOWER_u[] =
{
0x7, 0x7, 0x0, 0x0, 0x7, 0x7, 0x0, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0xe0, 0xf0, 0x10, 0x0, 
};


unsigned char LOWER_v[] =
{
0x7, 0x7, 0x0, 0x0, 0x0, 0x7, 0x7, 0x0, 0xc0, 0xe0, 0x30, 0x10, 0x30, 0xe0, 0xc0, 0x0, 
};


unsigned char LOWER_w[] =
{
0x7, 0x7, 0x0, 0x1, 0x0, 0x7, 0x7, 0x0, 0xe0, 0xf0, 0x30, 0xe0, 0x30, 0xf0, 0xe0, 0x0, 
};


unsigned char LOWER_x[] =
{
0x4, 0x6, 0x3, 0x1, 0x3, 0x6, 0x4, 0x0, 0x10, 0x30, 0xe0, 0xc0, 0xe0, 0x30, 0x10, 0x0, 
};


unsigned char LOWER_y[] =
{
0x7, 0x7, 0x0, 0x0, 0x0, 0x7, 0x7, 0x0, 0xe2, 0xf2, 0x12, 0x12, 0x16, 0xfc, 0xf8, 0x0, 
};


unsigned char LOWER_z[] =
{
0x6, 0x6, 0x4, 0x5, 0x7, 0x6, 0x4, 0x0, 0x30, 0x70, 0xd0, 0x90, 0x10, 0x30, 0x30, 0x0, 
};




//����
unsigned char NUM_0[] =
{
0xf, 0x1f, 0x30, 0x23, 0x30, 0x1f, 0xf, 0x0, 0xc0, 0xe0, 0x30, 0x10, 0x30, 0xe0, 0xc0, 0x0, 
};


unsigned char NUM_1[] =
{
0x0, 0x8, 0x18, 0x3f, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x10, 0x10, 0xf0, 0xf0, 0x10, 0x10, 0x0, 
};


unsigned char NUM_2[] =
{
0x10, 0x30, 0x21, 0x23, 0x26, 0x3c, 0x18, 0x0, 0x70, 0xf0, 0x90, 0x10, 0x10, 0x30, 0x30, 0x0, 
};


unsigned char NUM_3[] =
{
0x10, 0x30, 0x22, 0x22, 0x22, 0x3f, 0x1d, 0x0, 0x20, 0x30, 0x10, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};


unsigned char NUM_4[] =
{
0x3, 0x7, 0xd, 0x19, 0x3f, 0x3f, 0x1, 0x0, 0x0, 0x0, 0x0, 0x10, 0xf0, 0xf0, 0x10, 0x0, 
};


unsigned char NUM_5[] =
{
0x3e, 0x3e, 0x22, 0x22, 0x22, 0x23, 0x21, 0x0, 0x20, 0x30, 0x10, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};


unsigned char NUM_6[] =
{
0xf, 0x1f, 0x32, 0x22, 0x22, 0x3, 0x1, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};


unsigned char NUM_7[] =
{
0x30, 0x30, 0x20, 0x21, 0x23, 0x3e, 0x3c, 0x0, 0x0, 0x0, 0xf0, 0xf0, 0x0, 0x0, 0x0, 0x0, 
};


unsigned char NUM_8[] =
{
0x1d, 0x3f, 0x22, 0x22, 0x22, 0x3f, 0x1d, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0x10, 0xf0, 0xe0, 0x0, 
};


unsigned char NUM_9[] =
{
0x1c, 0x3e, 0x22, 0x22, 0x22, 0x3f, 0x1f, 0x0, 0x0, 0x10, 0x10, 0x10, 0x30, 0xe0, 0xc0, 0x0, 
};



//������1	(�ո�) ! " # $ % ' ( ) * + , - . /
unsigned char PUNC_1_0[] =
{
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_1_1[] =
{
0x0, 0x0, 0x1c, 0x3f, 0x3f, 0x1c, 0x0, 0x0, 0x0, 0x0, 0x0, 0xb0, 0xb0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_1_2[] =
{
0x0, 0x70, 0x78, 0x0, 0x0, 0x78, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_1_3[] =
{
0x4, 0x1f, 0x1f, 0x4, 0x1f, 0x1f, 0x4, 0x0, 0x40, 0xf0, 0xf0, 0x40, 0xf0, 0xf0, 0x40, 0x0, 
};


unsigned char PUNC_1_4[] =
{
0x1c, 0x3e, 0x22, 0xe2, 0xe2, 0x33, 0x19, 0x0, 0x60, 0x30, 0x10, 0x1c, 0x1c, 0xf0, 0xe0, 0x0, 
};


unsigned char PUNC_1_5[] =
{
0xc, 0xc, 0x0, 0x1, 0x3, 0x6, 0xc, 0x0, 0x30, 0x60, 0xc0, 0x80, 0x0, 0x30, 0x30, 0x0, 
};


unsigned char PUNC_1_6[] =
{
0x1, 0x1b, 0x3e, 0x27, 0x3d, 0x1b, 0x2, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0xe0, 0xf0, 0x10, 0x0, 
};


unsigned char PUNC_1_7[] =
{
0x0, 0x8, 0x78, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_1_8[] =
{
0x0, 0x0, 0xf, 0x1f, 0x30, 0x20, 0x0, 0x0, 0x0, 0x0, 0xc0, 0xe0, 0x30, 0x10, 0x0, 0x0, 
};


unsigned char PUNC_1_9[] =
{
0x0, 0x0, 0x20, 0x30, 0x1f, 0xf, 0x0, 0x0, 0x0, 0x0, 0x10, 0x30, 0xe0, 0xc0, 0x0, 0x0, 
};


unsigned char PUNC_1_10[] =
{
0x1, 0x5, 0x7, 0x3, 0x3, 0x7, 0x5, 0x1, 0x0, 0x40, 0xc0, 0x80, 0x80, 0xc0, 0x40, 0x0, 
};


unsigned char PUNC_1_11[] =
{
0x0, 0x1, 0x1, 0x7, 0x7, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0xc0, 0xc0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_1_12[] =
{
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0x78, 0x70, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_1_13[] =
{
0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_1_14[] =
{
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x30, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_1_15[] =
{
0x0, 0x0, 0x0, 0x1, 0x3, 0x6, 0xc, 0x0, 0x30, 0x60, 0xc0, 0x80, 0x0, 0x0, 0x0, 0x0, 
};



//������2	: ; < = > ? @
unsigned char PUNC_2_0[] =
{
0x0, 0x0, 0x0, 0xc, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x60, 0x60, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_2_1[] =
{
0x0, 0x0, 0x0, 0xc, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x70, 0x60, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_2_2[] =
{
0x0, 0x1, 0x3, 0x6, 0xc, 0x18, 0x10, 0x0, 0x0, 0x0, 0x80, 0xc0, 0x60, 0x30, 0x10, 0x0, 
};


unsigned char PUNC_2_3[] =
{
0x0, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x0, 0x0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x0, 
};


unsigned char PUNC_2_4[] =
{
0x0, 0x10, 0x18, 0xc, 0x6, 0x3, 0x1, 0x0, 0x0, 0x10, 0x30, 0x60, 0xc0, 0x80, 0x0, 0x0, 
};


unsigned char PUNC_2_5[] =
{
0x18, 0x38, 0x20, 0x23, 0x27, 0x3c, 0x18, 0x0, 0x0, 0x0, 0x0, 0xb0, 0xb0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_2_6[] =
{
0xf, 0x1f, 0x10, 0x13, 0x13, 0x1f, 0xf, 0x0, 0xe0, 0xf0, 0x10, 0xd0, 0xd0, 0xd0, 0x80, 0x0, 
};


//������3	[ \ ] ^ _ `
unsigned char PUNC_3_0[] =
{
0x0, 0x0, 0x3f, 0x3f, 0x20, 0x20, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xf0, 0x10, 0x10, 0x0, 0x0, 
};


unsigned char PUNC_3_1[] =
{
0x1c, 0xe, 0x7, 0x3, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0xc0, 0xe0, 0x70, 0x0, 
};


unsigned char PUNC_3_2[] =
{
0x0, 0x0, 0x20, 0x20, 0x3f, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x10, 0x10, 0xf0, 0xf0, 0x0, 0x0, 
};


unsigned char PUNC_3_3[] =
{
0x10, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_3_4[] =
{
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 
};


unsigned char PUNC_3_5[] =
{
0x0, 0x0, 0x40, 0x60, 0x30, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
};


//������4  	{ | } ~
unsigned char PUNC_4_0[] =
{
0x0, 0x2, 0x2, 0x1f, 0x3d, 0x20, 0x20, 0x0, 0x0, 0x0, 0x0, 0xe0, 0xf0, 0x10, 0x10, 0x0, 
};


unsigned char PUNC_4_1[] =
{
0x0, 0x0, 0x0, 0x3f, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xf0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_4_2[] =
{
0x0, 0x20, 0x20, 0x3d, 0x1f, 0x2, 0x2, 0x0, 0x0, 0x10, 0x10, 0xf0, 0xe0, 0x0, 0x0, 0x0, 
};


unsigned char PUNC_4_3[] =
{
0x20, 0x60, 0x40, 0x60, 0x20, 0x60, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
};
//��ģ����
unsigned char *CHARARRAY[95] =
{
	PUNC_1_0, PUNC_1_1, PUNC_1_2, PUNC_1_3, PUNC_1_4, PUNC_1_5, PUNC_1_6, PUNC_1_7, PUNC_1_8, PUNC_1_9, PUNC_1_10, PUNC_1_11, PUNC_1_12, PUNC_1_13, PUNC_1_14, PUNC_1_15, 
	NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, 
	PUNC_2_0, PUNC_2_1, PUNC_2_2, PUNC_2_3, PUNC_2_4, PUNC_2_5, PUNC_2_6, 
	ALPHA_A, ALPHA_B, ALPHA_C, ALPHA_D, ALPHA_E, ALPHA_F, ALPHA_G, ALPHA_H, ALPHA_I, ALPHA_J, ALPHA_K, ALPHA_L, ALPHA_M, ALPHA_N, 
	ALPHA_O, ALPHA_P, ALPHA_Q, ALPHA_R, ALPHA_S, ALPHA_T, ALPHA_U, ALPHA_V, ALPHA_W, ALPHA_X, ALPHA_Y, ALPHA_Z, 
	PUNC_3_0, PUNC_3_1, PUNC_3_2, PUNC_3_3, PUNC_3_4, PUNC_3_5, 
	LOWER_a, LOWER_b, LOWER_c, LOWER_d, LOWER_e, LOWER_f, LOWER_g, LOWER_h, LOWER_i, LOWER_j, LOWER_k, LOWER_l, LOWER_m, LOWER_n, 
	LOWER_o, LOWER_p, LOWER_q, LOWER_r, LOWER_s, LOWER_t, LOWER_u, LOWER_v, LOWER_w, LOWER_x, LOWER_y, LOWER_z,
	PUNC_4_0, PUNC_4_1, PUNC_4_2, PUNC_4_3
};

/*Һ�����ܽ�����*/
void LCD_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG, ENABLE); 						 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_10 | GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13  | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4
	 							| GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/*����Ƶ���*/
	OpenLcdLight();

	/*��Դ���ƹܽ�����ߵ�ƽ��ά��������·�ĵ�Դ����*/
	GPIO_SetBits(GPIOD , GPIO_Pin_14);
}
/*��Һ���������*/
void OpenLcdLight(void)
{
	GPIO_SetBits(GPIOG , GPIO_Pin_14);	
}
/*�ر���ʾ�������*/
void CloseLcdLight(void)
{
	GPIO_ResetBits(GPIOG , GPIO_Pin_14);	
}
/*���Ϳ��������LCD������*/
void SetCommand(unsigned char Command)
{
	unsigned char i=0;
	CLR_A0();
	CLR_RW();
	for(i=0;i<8;i++)
	{
		if((Command>>i)&0x01==0x01)/*����ߵ�ƽ*/
		{
			GPIO_WriteBit(GPIOD,GPIO_Pin_7>>i,Bit_SET);
		}
		else /*����͵�ƽ*/
		{
		    GPIO_WriteBit(GPIOD,GPIO_Pin_7>>i,Bit_RESET);
		}
	}
   	CLR_CS();
	SET_E();
	CLR_E();
	SET_CS();
}

/*������ʾ���ݸ�LCD������*/
void SetData(unsigned char DData)
{
	unsigned char i=0;
	SET_A0();
	CLR_RW();
	for(i=0;i<8;i++)
	{
		if((DData>>i)&0x01==0x01)/*����ߵ�ƽ*/
		{
			GPIO_WriteBit(GPIOD,GPIO_Pin_7>>i,Bit_SET);
		}
		else /*����͵�ƽ*/
		{
			GPIO_WriteBit(GPIOD,GPIO_Pin_7>>i,Bit_RESET);
		}
	}
	CLR_CS();
	SET_E();
	CLR_E();
	SET_CS();
}

/*��ʼ��LCD������*/
void Init_LCD(void)
{
	SET_CS();
	SET_RES();
	SET_A0();
	CLR_E();

	/*reset LCD*/
	SET_RES();
	CLR_RES();
	SET_RES();
    	
	SetCommand(0xaf);            	// display on
	SetCommand(0x40);            	// display start line=0
	SetCommand(0xa0);            	// ADC=0
	SetCommand(0xa6);            	// normal display
	SetCommand(0xa4);            	// Duisplay all point = off
	SetCommand(0xa2);            	// LCD bias = 1/9
	SetCommand(0xc8);            	// Common output mode select= reverse
	SetCommand(0x2f);            	// Power control = all on
	SetCommand(0x26);            	// Rab Ratio
	SetCommand(0xf8);            	// Booster Ratio = 4x
	SetCommand(0x00);            	// (two byte command)
	
	SetCommand(0x81);            	// E-Vol setting
	SetCommand(0x1a);   			// (2byte command)
}

/*���LCD*/
void ClearLcd(void)    
{
	u16 i, j;

	for(i=0;i<8;i++)	
	{
		SetCommand(0xb0 | i);    	// select page 0~7
		SetCommand(0x10);        	// start form column 4
		SetCommand(0x00);          // (2byte command)
		for(j=0;j<128;j++)     	
		{
			SetData(0x00);      	//��ʾ�հ׵�
		}
	}

	/*�ָ���ʾ������Ҫ�������ʾ��ʷ��¼*/
//	memset(DispBuffer,0x20,MAXDISPCOL*7);
//	memset(UseFlag,0x00,7);
//	memset(DISPMODE, NORMALDISP, MAXDISPCOL*7);
}
/*��ʾһ��������ģ*/
void WriteENG(unsigned char *WriteData, unsigned char x, unsigned char y, unsigned char sx, unsigned char sy, unsigned char dispMode)    
{
	unsigned char TempData;
	unsigned char i, j, k;
	unsigned char covData=0;
	for(i=y;i<=sy;i++)
	{
		SetCommand(0xb0 | i);    			// select page 0~7
	 	SetCommand(0x10 | (x >> 4));
		SetCommand(x & 0x0f);        		// (2byte command)
		 
	 	for(j=x;j<=sx;j++)
	 	{
	      	covData=0;
	      	TempData=WriteData[((i-y)*(sx-x+1))+(j-x)];
	      	for (k=0; k<8; k++)
			{
	        	if (TempData & 0x01)
				{
	            	covData |= 0x01;
				}
	         	TempData = TempData>>1;
	        	if (k < 7)
				{
	            	covData = covData<<1;
	        	}					  
			}
			if(dispMode==REVERSEDISP)
				covData=~covData;
	        SetData(covData);
		}
	}
}
/*���Ҳ���ʾһ��������ģ
*ֻ����ʾ���֣���д��ĸ(Сд��ĸҲ��ʾΪ��д��ĸ)��ĳЩ����
*������english.c��english.h������ض�������������ӷ���
*/
void printChar(unsigned char mychar, unsigned char x, unsigned char y, unsigned char sx, unsigned char sy, unsigned char dispMode) 
{
	if(mychar>=' '&& mychar<='~')
		WriteENG(CHARARRAY[mychar-' '], x, y, sx, sy,dispMode);
	else
		WriteENG(CHARARRAY[0], x, y, sx, sy,dispMode);
}
/*��ʾһ�������ַ�*/
void DispOutENGInRow(unsigned char cRow, unsigned char cCol, unsigned char ENGchar, unsigned char dispMode)
{
	printChar(ENGchar, cCol*8, cRow, cCol*8 + 7, cRow + 1, dispMode) ;
}

/*��ʾһ��������ģ*/
void WriteCHN(unsigned char *WriteData, unsigned char x, unsigned char y, unsigned char sx, unsigned char sy, unsigned char dispMode)    
{
	unsigned char i, j;
	unsigned char covData=0;
	for(i=y;i<=sy;i++)
	{
		SetCommand(0xb0 | i);    			// select page 0~7
		SetCommand(0x10 | (x >> 4));
		SetCommand(x & 0x0f);        		// (2byte command)
		 
		for(j=x;j<=sx;j++)
		{
	    	if(dispMode==REVERSEDISP)
				covData = ~WriteData[ (i - y) * 16 + (j - x)];
			else
				covData = WriteData[ (i - y) * 16 + (j - x)];
	        SetData(covData);
	   	}
	}
}
/*��ʾһ�������ַ�����dataflash����������ģ����ʾ*/
void DispOutCHNInRow(unsigned char cRow, unsigned char cCol, unsigned long cpAddress, unsigned char dispMode)
{
	unsigned long pageNo,addInPage;
	unsigned char word[32];

	pageNo = cpAddress / FLASH_PAGE_SIZE;
	addInPage = cpAddress % FLASH_PAGE_SIZE;
	Read_AT45DB161D(pageNo,addInPage,32,word);
	WriteCHN(word, cCol*8, cRow, cCol*8 + 15, cRow + 1, dispMode);	
}
/*���ݺ��ֱ���ȡ����ģ����ģ���е�ƫ��*/	
unsigned long GetHZKOffset(unsigned int wHZCode)
{
	unsigned long dwOffset;
	unsigned short int cQCode,cWCode;
	unsigned short int cHighByte,cLowByte;

	/*������λƫ��*/
	cHighByte = wHZCode/256;
	cLowByte = wHZCode%256;
	cQCode= cHighByte - 0xb0;	
	cWCode= cLowByte - 0xa1;
	dwOffset=(94*cQCode+cWCode)*32;
	return(dwOffset);
}
/*��ʾ�ַ�����ָ����LCD����
*cPage��Ϊ��ʾ����ʼҳ���У�����Ϊ0-7
*cCol��Ϊ��ʾ����ʼ��,��Ϊ0-15��ע�⺺��ֻ����0-14
*cLen��Ϊ��ʾ���ݵĳ��ȣ��ַ�����������2���ַ���
*cpText��Ϊ��ʾ������
*dispMode: Ϊ��ʾ��ģʽ������������ʾNORMALDISP�ͷ�ɫ��ʾREVERSEDISP
*/
void DispOutString(unsigned char cPage, unsigned char cCol, unsigned char cLen, unsigned char *cpText, unsigned char dispMode)
{
   	unsigned char i;
   	unsigned char cCurCol, cChar;
   	unsigned long dwpFontOffset;
   	unsigned int wHZCode;

	/*����ַ���ʾ
	 *�ȶ�ȡһ���ֽڣ�����Ϊ���ĺ����ģ��ֱ���
	 *����������ַ���Ҫ�ٶ�ȡһ���ֽڣ���������ַ������ƫ��ֵ
	 */
   	cCurCol = cCol;
   	i=0;
   	while(i<cLen) 
   	{
		cChar = *(cpText+i);
		if (cChar>=0x80)			//�����ַ�
	  	{
			i++;
			wHZCode = cChar << 8;
			wHZCode += *(cpText+i);
			dwpFontOffset = GetHZKOffset(wHZCode); 
			DispOutCHNInRow(cPage, cCurCol, dwpFontOffset,dispMode);
			cCurCol += 2;
		}   
		else						//�����ַ�
		{
			DispOutENGInRow(cPage, cCurCol, cChar,dispMode);
			cCurCol += 1;
		}
		i++;
   	}
}