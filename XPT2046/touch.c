#include <stdio.h>
#include "spi.h"
#include "fblines.h"

/*--------------   8BITS CONTROL FOR XPT2046   -------------------
[7] 	S 		-- 1:   new control bits,
	     		   0:  ignore data on pins

[6-4]   A2-A0 		-- 101: X-position
		   	   001: Y-position

[3]	MODE 		-- 1:  8bits resolution
		 	   0:  12bits resolution

[2]	SER/(-)DFR	-- 1:  normal mode
			   0:  differential mode

[1-0]	PD1-PD0		-- 11: normal power
			   00: power saving mode
----------------------------------------------------------------*/
#define XPT_CMD_READXP	0xD8 //0xD0 //1101,0000  /* read X position data */
#define XPT_CMD_READYP	0x98 //0x90 //1001,0000 /* read Y position data */
#define XPT_XP_MIN 7
#define XPT_XP_MAX 116
#define XPT_YP_MIN 7
#define XPT_YP_MAX 116
#define LCD_SIZE_X 240
#define LCD_SIZE_Y 320

int main(void)
{
	int sx,sy;  //LCD screen coordinates.
	uint8_t cmd;
	uint8_t  xp[2],yp[2];
        FBDEV     fr_dev;

	/* open spi dev */
	SPI_Open();
	/* prepare fb device */
        fr_dev.fdfd=-1;
        init_dev(&fr_dev);


	while(1)
	{
	    cmd=XPT_CMD_READXP;
	    SPI_Write_then_Read(&cmd, 1, xp, 2);
	    cmd=XPT_CMD_READYP;
	    SPI_Write_then_Read(&cmd, 1, yp, 2);

	    if(xp[0]){
//	    	    printf("Xp[0]=%d\n",xp[0]); //untoched: Xp[0]=0, Xp[1]=0,
	    }

	    if(yp[0]<117){
  //     	    	printf("Yp[0]=%d\n",yp[0]); //untoched: Yp[0]=127=[2b0111,1111] ,Yp[1]=248=[2b1111,1100]
	    }
	    else
		yp[0]=0;

	    /* normalize x,y dat */
	    if(xp[0]<XPT_XP_MIN)xp[0]=XPT_XP_MIN;
	    if(xp[0]>XPT_XP_MAX)xp[0]=XPT_XP_MAX;
	    if(yp[0]<XPT_YP_MIN)yp[0]=XPT_YP_MIN;
	    if(yp[0]>XPT_YP_MAX)yp[0]=XPT_YP_MAX;

	    /* calculate screen coordinate */
	    sx=LCD_SIZE_X*(xp[0]-XPT_XP_MIN)/(XPT_XP_MAX-XPT_XP_MIN+1);
	    sy=LCD_SIZE_Y*(yp[0]-XPT_YP_MIN)/(XPT_YP_MAX-XPT_YP_MIN+1);
//	    printf("sx=%d, sy=%d \n",sx,sy);

	    draw_dot(&fr_dev,sx,sy);
//	    draw_dot(&fr_dev,sx+1,sy);
//	    draw_dot(&fr_dev,sx,sy+1);
//	    draw_dot(&fr_dev,sx+1,sy+1);

	    usleep(5000);
	}

	/* close fb dev */
        munmap(fr_dev.map_fb,fr_dev.screensize);
        close(fr_dev.fdfd);

	/* close spi dev */
	SPI_Close();
	return 0;
}