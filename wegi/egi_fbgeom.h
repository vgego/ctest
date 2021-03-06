/*------------------------------------------------------------------------------
Referring to: http://blog.chinaunix.net/uid-22666248-id-285417.html
 本文的copyright归yuweixian4230@163.com 所有，使用GPL发布，可以自由拷贝，转载。
但转载请保持文档的完整性，注明原作者及原链接，严禁用于任何商业用途。

作者：yuweixian4230@163.com
博客：yuweixian4230.blog.chinaunix.net


Modified by: Midas Zhou
-----------------------------------------------------------------------------*/
#ifndef __EGI_FBGEOM_H__
#define __EGI_FBGEOM_H__

#include <stdio.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdbool.h>
#include "egi.h"

/* for draw_dot(), roll back to start if reach boundary of FB mem */
#define no_FB_DOTOUT_ROLLBACK /* also check FB_SYMBOUT_ROLLBACK in symbol.h */


#ifndef _TYPE_FBDEV_
#define _TYPE_FBDEV_

    typedef struct fbdev{
        int fdfd; //open "dev/fb0"
        struct fb_var_screeninfo vinfo;
        struct fb_fix_screeninfo finfo;
        long int screensize;
        unsigned char *map_fb;
    }FBDEV;

#endif


/* global variale, Frame buffer device */
extern FBDEV   gv_fb_dev;

/* -------- functions ---------*/
void init_dev(FBDEV *dev);
void release_dev(FBDEV *dev);
bool point_inbox(int px,int py,int x1,int y1,int x2,int y2);
void fbset_color(uint16_t color);
void clear_screen(FBDEV *dev, uint16_t color);
int draw_dot(FBDEV *dev,int x,int y); //(x.y) 是坐标
void draw_line(FBDEV *dev,int x1,int y1,int x2,int y2);
void draw_oval(FBDEV *dev,int x,int y);
void draw_rect(FBDEV *dev,int x1,int y1,int x2,int y2);
int draw_filled_rect(FBDEV *dev,int x1,int y1,int x2,int y2);
void draw_circle(FBDEV *dev, int x, int y, int r);
void draw_filled_circle(FBDEV *dev, int x, int y, int r);
int fb_cpyto_buf(FBDEV *fb_dev, int x1, int y1, int x2, int y2, uint16_t *buf);
int fb_cpyfrom_buf(FBDEV *fb_dev, int x1, int y1, int x2, int y2, const uint16_t *buf);

/* square map */
void fb_drawimg_SQMap(int n, struct egi_point_coord x0y0, const uint16_t *image,
   	                                           const struct egi_point_coord *SQMat_XRYR);
/* annulus map */
void fb_drawimg_ANMap(int n, int ni, struct egi_point_coord x0y0, const uint16_t *image,
                                                       const struct egi_point_coord *ANMat_XRYR);

int fb_scale_pixbuf(unsigned int owid, unsigned int ohgt, unsigned int nwid, unsigned int nhgt,
                       const uint16_t *obuf, uint16_t *nbuf);

int egi_getpoit_interpol2p(EGI_POINT *pn, int off, const EGI_POINT *pa, const EGI_POINT *pb);
int egi_numstep_btw2p(int step, const EGI_POINT *pa, const EGI_POINT *pb);
int egi_randp_inbox(EGI_POINT *pr, const EGI_BOX *box);
int egi_randp_boxsides(EGI_POINT *pr, const EGI_BOX *box);
#endif
