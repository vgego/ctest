/*----------------------------------------------------
TEST L3G4200d



----------------------------------------------------*/
#include <stdio.h>
#include <sys/time.h>
#include "gyro_spi.h"
#include "gyro_l3g4200d.h"
#include "data_server.h"
#include "filters.h"
#include "i2c_oled_128x64.h"

#define RECORD_DATA_SIZE 4096 //

inline double math_tmIntegral(const double fx, double *sum);

int main(void)
{
   uint8_t val;
   int i,k;
   int send_count=0;
   int16_t bias_RXYZ[3];//bias value of RX RY RZ
   int16_t angRXYZ[3];//angular rate of XYZ
   double fangRXYZ[3]; //float angular rate of X,Y,Z aixs. --- fangRXYZ[] = sensf * angRXYZ[]
   double sensf=70/1000000000.0;//dpus //70/1000.0-dps, 70/1000000.0-dpms //sensitivity factor for FS=2000 dps.
//   float fRXYZbuff[RECORD_DATA_SIZE*3];//for TCP transfer
   //------ PID -----
   uint32_t dt_us; //delta time in us //U16: 0-65535 
   uint32_t sum_dt=0;//sum of dt //U32: 0-4294967295 ~4.3*10^9
   double fangXYZ[3]={0};// angle value of X Y Z  ---- fangXYZ[]= integ{ dt_us*fangRXYZ[] }

   struct int16MAFilterDB fdb_RX,fdb_RY,fdb_RZ; // filter contexts for RX RY RZ

   struct timeval tm_start,tm_end;
   struct timeval tmTestStart,tmTestEnd;

   //----- init spi
   printf("Open SPI ...\n");
   SPI_Open(); //SP clock set to 10MHz OK, if set to 5MHz, then read value of WHO_AM_I is NOT correct !!!!???????

   //----- init L3G4200D
   printf("Init L3G4200D ...\n");
   Init_L3G4200D();

   //----- init i2c and oled
   printf("Init I2C OLED ...\n");
   init_I2C_Slave();
   initOledDefault();
   clearOledV();
   push_Oled_Ascii32x18_Buff("-- Widora-NEO --",3,0);
   refresh_Oled_Ascii32x18_Buff(false);

   //---- init filter data base
   printf("Init int16MA filter data base ...\n");
   Init_int16MAFilterDB(&fdb_RX, 6, 0x7fff);
   Init_int16MAFilterDB(&fdb_RY, 6, 0x7fff);
   Init_int16MAFilterDB(&fdb_RZ, 6, 0x7fff);
   if( fdb_RX.f_buff==NULL || fdb_RY.f_buff==NULL || fdb_RZ.f_buff==NULL)
   {
		printf("fail to init filter data base strut!\n");
		return -1;
   }

   //---- preare TCP data server
   printf("Prepare TCP data server ...\n");
   if(prepare_data_server() < 0)
	printf(" fail to prepare data server!\n");


   val=halSpiReadReg(L3G_WHO_AM_I);
   printf("L3G_WHO_AM_I: 0x%02x\n",val);
   val=halSpiReadReg(L3G_FIFO_CTRL_REG);
   printf("FIFO_CTRL_REG: 0x%02x\n",val);
   val=halSpiReadReg(L3G_OUT_TEMP);
   printf("OUT_TEMP: %d\n",val);

   //----- to get bias value for RXYZ
   printf("---  Read and calculate the bias value now, keep the L3G4200D even and static !!!  ---\n");
   sleep(1);
   gyro_get_int16BiasXYZ(bias_RXYZ);
   printf("bias_RX: %f,  bias_RY: %f, bias_RZ: %f \n",sensf*bias_RXYZ[0],sensf*bias_RXYZ[1],sensf*bias_RXYZ[2]);

   //----- wait to accept data client ----
   printf("wait for Matlab to connect...\n");
   cltsock_desc=accept_data_client();
   if(cltsock_desc < 0){
	printf(" Fail to accept data client!\n");
	return -1;
   }

   //================   loop: get data and record  ===============
   printf(" starting testing ...\n");
   gettimeofday(&tmTestStart,NULL);
   for(k=0;k<130000;k++) {

	   //------- read angular rate of XYZ
	   gyro_read_int16RXYZ(angRXYZ);
//	   printf("Raw data: angRX=%d angRY=%d angRZ=%d \n",angRXYZ[0],angRXYZ[1],angRXYZ[2]);

	   //------  deduce bias to adjust zero level
	   for(i=0; i<3; i++)
		   angRXYZ[i]=angRXYZ[i]-bias_RXYZ[i];
//	   printf("Zero_leveled data: angRX=%d angRY=%d angRZ=%d \n",angRXYZ[0],angRXYZ[1],angRXYZ[2]);

	   //---- Activate filter for  RX RY RZ
	   // first reset each filter context struct, then you must use the same data stream until end.

	   int16_MAfilter(&fdb_RX, &angRXYZ[0], &angRXYZ[0], 0); //No.0 fitler
	   int16_MAfilter(&fdb_RY, &angRXYZ[1], &angRXYZ[1], 0); //No.1 fitler
	   int16_MAfilter(&fdb_RZ, &angRXYZ[2], &angRXYZ[2], 0); //No.2 fitler
//	   printf("MA filtered data: angRX=%d angRY=%d angRZ=%d \n",angRXYZ[0],angRXYZ[1],angRXYZ[2]);

	   //----- convert to real value
	   for(i=0; i<3; i++)
		   fangRXYZ[i]=sensf*angRXYZ[i];

	   //<<<<<<<<   put data to buffer  >>>>>>>>>>>
//	   for(i=0; i<3; i++)
//		   fRXYZbuff[3*k+i]=fangRXYZ[i];

//	   printf("angRX=%d angRY=%d angRZ=%d \n",angRXYZ[0],angRXYZ[1],angRXYZ[2]);
//	   printf("fangRX=%f  fangRY=%f  fangRZ=%f \n",fangRXYZ[0],fangRXYZ[1],fangRXYZ[2]);


	   gettimeofday(&tm_end,NULL);// !!!--end of read !!!
	   //------ get time span for integration calculation
	   dt_us=get_costtimeus(tm_start,tm_end);
	   sum_dt += dt_us;
	   //------ start timing immediatly to minimize error
	   gettimeofday(&tm_start,NULL);// !!! --start time !!!

	   //------ integration of angle XYZ -----
//	   printf("dt_us:%dus\n",dt_us);
	   for(i=0; i<3; i++)
		   fangXYZ[i] += dt_us*fangRXYZ[i];

//	   //------ time integration of angluar rate RXYZ -----

	   //<<<<<<<<<< Every 500th count:  send integral XYZ angle to client Matlab    >>>>>>>>>>>
	   if(send_count==0)
	   {
		if( send_client_data((uint8_t *)fangXYZ,3*sizeof(double)) < 0)
			printf("-------- fail to send client data ------\n");
		send_count=100;
	   }
	   else
		send_count-=1;

	   //------  print out
	   printf(" integral value:  fangX=%f  fangY=%f  fangZ=%f \r", fangXYZ[0], fangXYZ[1], fangXYZ[2]);
	   fflush(stdout);

//	   usleep(200000);
   }
 
   gettimeofday(&tmTestEnd,NULL);
   printf("\nTime elapsed: %fs \n",get_costtimeus(tmTestStart,tmTestEnd)/1000000.0);
   printf("Sum of dt: %fs \n", sum_dt/1000000.0);

   printf("-----  integral value of fangX=%f  fangY=%f  fangZ=%f  ------\n", fangXYZ[0], fangXYZ[1], fangXYZ[2]);

   //---- send data to client Matlab
//   if( send_client_data((uint8_t *)fRXYZbuff,RECORD_DATA_SIZE*3*sizeof(float)) < 0)
//	printf("-------- fail to send client data ------\n");


   //---- release filter data base
   Release_int16MAFilterDB(&fdb_RX);
   Release_int16MAFilterDB(&fdb_RY);
   Release_int16MAFilterDB(&fdb_RZ);
   //----- close I2C and Oled
    free_I2C_IOdata();
    intFcntlOp(g_fdOled,F_SETLK,F_UNLCK,0,SEEK_SET,10);
   //---- close spi
   SPI_Close();
   //----- close TCP server
   close_data_service();

   return 0;
}


/*--------------------------------------------------------------------
Integral of time difference and fx
!!!!! ------ onlye one instance in a loop ------ !!!!!
In order to get a regular time difference,you shall call this function
in a loop.
dt_us will be 0 at first call

fx ---- function of time,whose unit is us(10^-6 s) !!!

return:
      integration(summation) of fx * dt_us
---------------------------------------------------------------------*/
struct  tmIntegralDB {  //Time integral data base
uint8_t num; // num of interal group
double  *fx;//

}

inline double math_tmIntegral(const double fx, double *sum)
{
	   uint32_t dt_us;//time in us
	   static struct timeval tm_end,tm_start;
	   static struct timeval tm_start_1, tm_start_2;
	   static double sum_dt; //summation of dt_us 

	   gettimeofday(&tm_end,NULL);// !!!--end of read !!!
           //------  to minimize error between two timer functions
           gettimeofday(&tm_start,NULL);// !!! --start time !!!
	   tm_start_2 = tm_start_1;
	   tm_start_1 = tm_start;
           //------ get time difference for integration calculation
	   if(tm_start_2.tv_sec != 0) 
	           dt_us=get_costtimeus(tm_start_2,tm_end); //return  0 if tm_start > tm_end
	   else // discard fisrt value
		   dt_us=0;

	   sum_dt +=dt_us;

           (*sum) += fx*dt_us;

	   return sum_dt;
}
