/* -------------------------------------------------------------------------
original source: https://blog.csdn.net/luxiaoxun/article/details/7622988

1. Modified for a 240x320 SPI LCD display.
2. The width of the displaying picture must be a multiple of 4.
3. Applay show_jpg() or show_bmp() in main().

TODO:
    1. to pad width to a multiple of 4 for bmp file.
    2. jpgshow() picture flips --OK

./open-gcc -L./lib -I./include -ljpeg -o jpgshow fbshow.c

Midas
---------------------------------------------------------------------------*/
#ifndef __EGI_BMPJPG_H__
#define __EGI_BMPJPG_H__


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h> //u
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <jpeglib.h>
#include <jerror.h>
#include "egi.h"
#include "egi_image.h"
#include "egi_fbgeom.h"

//14byte文件头
typedef struct
{
	char cfType[2];//文件类型，"BM"(0x4D42)
	long cfSize;//文件大小（字节）
	long cfReserved;//保留，值为0
	long cfoffBits;//数据区相对于文件头的偏移量（字节）
}__attribute__((packed)) BITMAPFILEHEADER;
//__attribute__((packed))的作用是告诉编译器取消结构在编译过程中的优化对齐

//40byte信息头
typedef struct
{
	char ciSize[4];//BITMAPFILEHEADER所占的字节数
	long ciWidth;//宽度
	long ciHeight;//高度
	char ciPlanes[2];//目标设备的位平面数，值为1
	int ciBitCount;//每个像素的位数
	char ciCompress[4];//压缩说明
	char ciSizeImage[4];//用字节表示的图像大小，该数据必须是4的倍数
	char ciXPelsPerMeter[4];//目标设备的水平像素数/米
	char ciYPelsPerMeter[4];//目标设备的垂直像素数/米
	char ciClrUsed[4]; //位图使用调色板的颜色数
	char ciClrImportant[4]; //指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
}__attribute__((packed)) BITMAPINFOHEADER;

typedef struct
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
//	unsigned char reserved; // for 32bit pix only
}__attribute__((packed)) PIXEL;//颜色模式RGB


/* struct for 16bits_color image buffer */
/*
typedef struct
{
	int height;
	int width;
	uint16_t *imgbuf;
} EGI_IMGBUF;
*/


#define SHOW_BLACK_TRANSP	1
#define SHOW_BLACK_NOTRANSP	0

/*  ----- functions --------  */
unsigned char *open_jpgImg(char *filename, int *w, int *h, int *components, FILE **fil);
void close_jpgImg(unsigned char *imgbuf);

int show_bmp(char* fpath,FBDEV *fb_dev, int blackoff, int x0, int y0);
int show_jpg(char* fpath,FBDEV *fb_dev, int blackoff, int x0, int y0);

//int egi_imgbuf_loadjpg(char* fpath, FBDEV *fb_dev, EGI_IMGBUF *egi_imgbuf);
int egi_imgbuf_loadjpg(char* fpath, EGI_IMGBUF *egi_imgbuf);
void egi_imgbuf_release(EGI_IMGBUF *egi_imgbuf);

/* display picture buffer with full screen */
//int egi_imgbuf_display(const EGI_IMGBUF *egi_imgbuf, FBDEV *fb_dev, int xp, int yp);

/* display picture buffer in a window */
int egi_imgbuf_windisplay(const EGI_IMGBUF *egi_imgbuf, FBDEV *fb_dev, int xp, int yp,
			                          int xw, int yw, int winw, int winh);
/* roaming picture in a window */
int egi_roampic_inwin(char *path, FBDEV *fb_dev, int step, int ntrip,
                                			int xw, int yw, int winw, int winh);
/* find all jpg files in a path */
int egi_find_jpgfiles(const char* path, int *count, char **fpaths, int maxfnum, int maxflen);

#endif
