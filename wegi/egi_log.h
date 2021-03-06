#ifndef __EGI_LOG__
#define __EGI_LOG__

#include <stdio.h>
#include <stdlib.h>

#define EGI_LOGFILE_PATH "/tmp/egi_log"


#define ENABLE_LOGBUFF_PRINT 	1	/* enable to print log buff content */
#define EGI_LOG_MAX_BUFFITEMS	128 	/* MAX. number of log buff items */
#define EGI_LOG_MAX_ITEMLEN	256 	/* Max length for each log string item */
#define EGI_LOG_WRITE_SLEEPGAP	10  	/* in ms, sleep gap between two buff write session in egi_log_thread_write() */
#define EGI_LOG_QUITWAIT 	55 	/* in ms, wait for other thread to finish pushing inhand log string,
				     	 * before quit the log process */

/* LOG LEVEL */
enum egi_log_level
{
	LOGLV_NONE	  =(1<<0),
	LOGLV_INFO        =(1<<1),
	LOGLV_WARN        =(1<<2),
	LOGLV_ERROR       =(1<<3),
	LOGLV_CRITICAL	  =(1<<4),
	LOGLV_TEST        =(1<<15),
};

/* Only log levels included in DEFAULT_LOG_LEVELS will be effective in EGI_PLOG() */
#define DEFAULT_LOG_LEVELS   (LOGLV_NONE|LOGLV_TEST|LOGLV_INFO|LOGLV_WARN|LOGLV_ERROR|LOGLV_CRITICAL)

/* Only log level gets threshold(>=) that will be written to log file directly with nobuffer */
#define LOGLV_NOBUFF_THRESHOLD		LOGLV_ERROR


/* --- logger functions --- */
int egi_push_log(enum egi_log_level log_level, const char *fmt, ...);
//static void egi_log_thread_write(void);
//static int egi_malloc_buff2D(char ***buff, int items, int item_len);
//static int egi_free_buff2D(char ***buff, int items, int item_len);
int egi_init_log(void);
//static int egi_stop_log(void);
//static int egi_free_logbuff(void)
int egi_quit_log(void);


/* define egi_plog(), push to log_buff
 * Let the caller to put FILE and FUNCTION, we can not ensure that two egi_push_log()
 * will push string to the log buff exactly one after the other,because of concurrency
 * race condition.
 *  egi_push_log(" From file %s, %s(): \n",__FILE__,__FUNCTION__);
*/

#define EGI_PLOG(level, fmt, args...)                 \
        do {                                            \
                if(level & DEFAULT_LOG_LEVELS)          \
                {                                       \
			egi_push_log(level,fmt, ## args);	\
                }                                       \
        } while(0)



#endif
