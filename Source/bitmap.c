#include "lib.h"



void BitmapInit(Bitmap* bm)
{
    bm->bitmap = 0;
}

uint32_t BitmapMaxBit(void)
{
    return 32;
}

/*
 * pos 0~31
 */
void BitmapSet(Bitmap* bm, uint32_t pos)
{
	if(pos >= 32) return;
    bm->bitmap |= (1 << pos);
}

void BitmapClear(Bitmap* bm, uint32_t pos)
{
	if(pos >= 32) return;
    bm->bitmap &= ~(1 << pos);
}


//从最低位开始数，寻找第一个1的位置
uint32_t BitmapGetFirstSet(Bitmap* bm)
{
    static const uint8_t index_table[] = 
    {
		/* 00 */ 0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
    };
	
	//每8位数据查询一次，
	//确定在第一个8位的何处
    if((bm->bitmap & 0xff) > 0 )
    {
        return index_table[bm->bitmap&0xff];
    }
    else if(bm->bitmap & 0xff00 )
    {
        return index_table[(bm->bitmap >> 8) & 0xff] + 8;
    }
    else if(bm->bitmap & 0xff0000 )
    {
        return index_table[(bm->bitmap >> 16) & 0xff] + 16;
    }
    else if(bm->bitmap & 0xff000000 )
    {
        return index_table[(bm->bitmap >> 24) & 0xff] + 24;
    }
    else return BitmapMaxBit();
}