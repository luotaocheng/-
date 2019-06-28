#include <pthread.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "1.8.h"
#include <pthread.h>
#include <linux/input.h>
#include <math.h>

int *p;
int level;


//6818串口所对应的文件名
#define COM2 "/dev/ttySAC1"
#define COM3 "/dev/ttySAC2"
#define COM4 "/dev/ttySAC3"



/*
file:串口所对应的文件名
peed:波特率,是9600
返回值是初始化之后的串口文件的文件描述符
*/
int serial_init(char *file,int peed)
{
  /*
    打开串口文件
  */
    int fd = open(file,O_RDWR);
    if(fd == -1)
    {
        perror("open serial error");
        return -1;
    }
  
    /*定义串口属性结构体*/
    struct termios myserial;
    memset(&myserial,0,sizeof(myserial));//清零
  
  /*设置控制模式 本地连接  使能接收*/
    myserial.c_cflag |= (CLOCAL | CREAD); 
    myserial.c_cflag &=  ~CSIZE; //清空数据位
    myserial.c_cflag &= ~CRTSCTS; //无硬件控制流
    myserial.c_cflag |= CS8; //数据位 8
    myserial.c_cflag &= ~CSTOPB; //停止位 1
    myserial.c_cflag &= ~PARENB; //不要校验

    /*设置波特率*/
  switch(peed)
  {
    case 9600:
      cfsetospeed(&myserial,B9600);
      cfsetispeed(&myserial,B9600);
      break;
    case 57600:
      cfsetospeed(&myserial,B57600);
      cfsetispeed(&myserial,B57600);
      break;
    case 115200:
      cfsetospeed(&myserial,B115200);
      cfsetispeed(&myserial,B115200);
      break;
  }
  
    /*刷新输出队列，清除正接收的数据*/
    tcflush(fd,TCIFLUSH);

    /*更改配置*/
    tcsetattr(fd,TCSANOW,&myserial);
  
    return fd;
}




//画点函数
void display_point(int color,int x,int y){
  *(p+x*800+y) = color;
}

//写字函数
void disp_word(char word[],int color,int m,int x0 ,int y0,int width,int heigh){
  int i,j;
  int x,y;
  for(i=0;i<(width*heigh/8);i++){
    for(j=0;j<8;j++){
      if(word[i] >> (7-j) & 1 == 1)
      {
        x = i/(width/8);
        y = (i%(width/8))*8+m*16+j;
        display_point(0x000000,x+x0,y+y0);
      }
    }
  }
}

void disply_world(char word[],int color,int x0,int y0,int w,int h)
{
  int i,j;
  int a,b;
  for (i = 0; i < (w*h/8); i++)
  {
      for (j = 0; j < 8; j++)
      {

        if (word[i] >> (7-j) & 1 == 1)
        {
          a = i/(w/8);
          b = (i%(w/8))*8+j;
            display_point(color,a+x0,b+y0);
        }
      }

    }
}

//打印图片
dis_bmp(char word[],int x,int y){
  int w,h;
  int n=0;
  int fdd;
  int color;
  int i,j;
  fdd = open(word,O_RDWR);
  lseek(fdd,0x12,SEEK_SET);
  read(fdd,&w,4);

  lseek(fdd,0x16,SEEK_SET);
  read(fdd,&h,4);

  char buf[w*h*3];
  lseek(fdd,54,SEEK_SET);
  read(fdd,buf,w*h*3);
  //printf("%d\n", w);
  for (i = 0; i < h; i++)
  {
    for (j = 0; j < w; j++)
    {
      color = buf[3*n+2] << 16 | buf[3*n+1] << 8 | buf[3*n];
        n++;

      display_point(color,x+h-i-1,y+j);
    
    }
  }
  close(fdd);

}

dis_bmp1(char word[],int x,int y){
  int w,h;
  int n=0;
  int fdd;
  int color;
  int i,j;
  fdd = open(word,O_RDWR);
  lseek(fdd,0x12,SEEK_SET);
  read(fdd,&w,4);

  lseek(fdd,0x16,SEEK_SET);
  read(fdd,&h,4);

  char buf[w*h*3];
  lseek(fdd,54,SEEK_SET);
  read(fdd,buf,w*h*3);
  //printf("%d\n", w);
  for (i = 0; i < h; i++)
  {
    for (j = 0; j < w; j++)
    {
      color = buf[3*n+2] << 16 | buf[3*n+1] << 8 | buf[3*n];
        n++;

      display_point(color,x+h-i-1,y+j);
    
    }
    usleep(5000);
  }
  close(fdd);

}

//计量一个值
void num(int value,int x,int y,int w,int h)
{
    int yigezhi = value;

    int count=0;
    while(value!=0)				//计算一个数的位数
    {
     
        value/=10;
        count++;
    }
 
    
    int shuzi = count;
	int array_[20]={0};
	int i,j;
	int zhongjian;
 	int zz;
 	
  	for (i = 0; i < shuzi; i++)					//给数组赋值
  	{
  		zz = yigezhi%(chengf(shuzi-i));
  		zhongjian = (zz/(chengf(shuzi-i-1)));
  		array_[i]=zhongjian;
  		array_[i+1] = (zz%(chengf(shuzi-i-1)))/(chengf(shuzi-i-1));
  		//printf("%d\n", array_[i]);
  	}
    for (i = 0; i < count; i++)
    {
      
      if (array_[i]==0)
      {
        disp_word(buf0,0x000000,i,x,y,w,h);
      }
      else if (array_[i]==1)
      {
        disp_word(buf1,0x000000,i,x,y,w,h);
      }
      else if (array_[i]==2)
      {
        disp_word(buf2,0x000000,i,x,y,w,h);
      }
      else if (array_[i]==3)
      {
        disp_word(buf3,0x000000,i,x,y,w,h);
      }
      else if (array_[i]==4)
      {
        disp_word(buf4,0x000000,i,x,y,w,h);
      }
      else if (array_[i]==5)
      {
        disp_word(buf5,0x000000,i,x,y,w,h);
      }
      else if (array_[i]==6)
      {
        disp_word(buf6,0x000000,i,x,y,w,h);
      }
      else if (array_[i]==7)
      {
        disp_word(buf7,0x000000,i,x,y,w,h);
      }
      else if (array_[i]==8)
      {
        disp_word(buf8,0x000000,i,x,y,w,h);
      }
      else if (array_[i]==9)
      {
        disp_word(buf9,0x000000,i,x,y,w,h);
      }

  
  }
}

//乘方函数
int chengf(int w){
  int i,sum=1;
  for(i=0;i<w;i++){
    sum = 10 * sum;
  }
  return sum;
}
 

//数组映射
pic_num(int num){
	if(pic[num]==0){
		dis_bmp1(pic3,219,500);
	}
	else if(pic[num]==1){
		dis_bmp1(pic4,219,500);
	}
	else if(pic[num]==2){
		dis_bmp1(pic5,219,500);
	}
	
}

/*线程1*/
//烟雾
void thread_1(void)
{
	
		int fdd=open("/dev/beep",O_RDWR);
	int fd = serial_init(COM3,9600);
	/*if(fd ==-1){
		perror("error!");
		return -1;
	}*/
	dis_bmp(pic6,0,0);
	while(1){
		char w_buf[9]={0xFF,0x01,0x86,0,0,0,0,0,0x79};
		write(fd,w_buf,9);
		usleep(1000);

		char r_buf[9];
		read(fd,r_buf,9);


		int n = r_buf[2] << 8 | r_buf[3];

		printf("n=%d\n", n);
		if(n>130){
			ioctl(fdd,0,1);
			sleep(3);
			ioctl(fdd,1,1);
		}
		else
		{
			ioctl(fdd,1,1);
		}
		sleep(3);
	}
	close(fdd);
	close(fd);
		sleep(1);
	
}
 
/*线程2*/
//GY
void thread_2(void)
{
	
		int fd2 = open("/dev/fb0",O_RDWR);
  p=mmap(NULL,480*800*4,PROT_READ | PROT_WRITE,MAP_SHARED,fd2,0);
 int i,j;
 /*
  for(i=100;i<480;i++)
  {
    for(j=0;j<600;j++)
    {   
      display_point(0xffffff,i,j);
    }
  }
  */
  
  int fd = serial_init(COM2,9600);
  
  while(1){

    char w_buf[3]={0xa5,0x83,0x28};
    int w=write(fd,w_buf,3);
    
    //usleep(1000);
    char r_buf[24];
    int r = read(fd,r_buf,24);
    
    printf("r=%d\n", r);
    
    int lux;  //光照
    int temp; //温度
    int air;  //气压
    
    int hum;  //湿度
    int altitude;   //海拔
    lux = (r_buf[4] << 24| r_buf[5] << 16 | r_buf[6] << 8 | r_buf[7])/100;
    temp = (r_buf[4+9] << 8 | r_buf[5+9])/100;
    air = (r_buf[6+9] << 24 | r_buf[7+9] << 16 | r_buf[8+9] << 8 | r_buf[9+9])/100;
    hum = (r_buf[10+9] << 8 | r_buf[11+9])/100;
    
    altitude = r_buf[12+9] << 8 | r_buf[13+9];

    printf("lux=%d\n", lux);
    printf("temp=%d\n", temp);
    printf("air=%d\n", air);
    printf("hum=%d\n", hum);
    printf("altitude=%d\n", altitude);
    printf("\n");
    int i,j;
    for(i=151;i<480;i++)
    {
    for(j=180;j<350;j++)
    {   
      display_point(0xfffffff,i,j);
    }
	}

    
    //char word[],int color,int m,int x0 ,int y0
     num(lux,170,200,16,20);
    num(temp,220,200,16,20);
    num(hum,270,200,16,20);
    num(air,320,200,16,20);
    num(altitude,380,200,16,20);
    //display_bian();
    disply_chengyuan();
    
    if(lux>10 && level==1){
      dis_bmp(pic1,180,350);
    }
    else{
      dis_bmp(pic2,180,350);
    }
    //dis_bmp(pic1,556,120);
  


    sleep(2);
    
    
  }
 
  munmap(p,480*800*4);
  //close(fd);
  close(fd2);
		sleep(1);
	
}
/*线程3*/
//触摸
void thread_3(void)
{
	while(1)
	{
		int fd = open("/dev/input/event0",O_RDWR);
		struct input_event event;
		int x1,y1,x2,y2;
		int a=0,b=0;
		int x_num,y_num;
		int i,j;
		int fd1 = open("/dev/fb0",O_RDWR);
		p=mmap(NULL,480*800*4,PROT_READ | PROT_WRITE,MAP_SHARED,fd1,0);
		
		dis_bmp1(pic3,219,500);
		int cout=0;

		
		while(1){
			int r = read(fd,&event,sizeof(event));
			if(r != sizeof(event)){
				continue;
			}
			if(event.type==0x03 && event.code == 0){
				if(a==0){
					x1=event.value;
					a=1;
				}
				x2=event.value;
			}
			if(event.type==0x03 && event.code == 1){
				if(b==0){
					y1=event.value;
					b=1;
				}
				y2=event.value;
			}
			if(event.type==0x01 && event.code==0x14a && event.value==0){
				printf("x1=%d  y1=%d\n", x1 ,y1);
				printf("x2=%d  y2=%d\n", x2 ,y2);
				x_num = abs(x1-x2);
				y_num = abs(y1-y2);
				if(x_num<y_num){
					if(y1>y2){
						printf("UP\n");
						level=1;
            //dis_bmp(pic1,180,350);

					}
					else if(y1<y2){
						printf("DOWN\n");
            level=0;
            dis_bmp(pic2,180,350);
						
					}
				}
				else if(x_num>y_num){
					if(x1>x2){
						printf("LEFT\n");
						cout--;
						if(cout<0){
							cout=2;
						}
						
						pic_num(cout);
						
						
					}
					else if(x1<x2){
						printf("RIGHT\n");
						cout++;
						if(cout>2){
							cout=0;
						}
						
						pic_num(cout);
						
					}
				}

				a=0;
				b=0;
			}


		}
		munmap(p,480*800*4);
		close(fd1);
		close(fd);
		}
}
 

void disply_chengyuan()
{
  disply_world(zhang,0x000000,300,10,24,20);
  disply_world(wei,0x000000,300,40,24,20);
  disply_world(hong,0x000000,300,70,24,20);
  //disply_world(mao,0xff0000,410,160,24,20);

  //num(int value,int x,int y,int w,int h)
  num(41,320,40,16,20);
  disply_world(luo,0x000000,250,10,24,20);
  disply_world(tao,0x000000,250,40,24,20);
  disply_world(cheng,0x000000,250,70,24,20);
  //disply_world(mao,0xff0000,310,160,24,20);
  num(40,270,40,16,20);
  disply_world(zhan,0x000000,150,10,24,20);
  disply_world(run,0x000000,150,40,24,20);
  disply_world(hao,0x000000,150,70,24,20);
  //disply_world(mao,0xff0000,110,160,24,20);
  num(35,170,40,16,20);
  disply_world(lo,0x000000,200,10,24,20);
  disply_world(ying,0x000000,200,70,24,20);
  //disply_world(mao,0xff0000,210,250,24,20);
  num(36,220,40,16,20);
  
  disply_world(zhi,0xff0000,10,150,32,33);
  disply_world(neng,0xff0000,10,300,32,33);
  disply_world(jia,0xff0000,10,450,32,33);
  disply_world(ju,0xff0000,10,600,32,33);
  
  disply_world(guang,0x000000,150,200,24,20);
  disply_world(zhao,0x000000,150,240,24,20);
  //disply_world(mao,0xff0000,110,340,24,20);
  disply_world(wen,0x000000,200,200,24,20);
  disply_world(du,0x000000,200,240,24,20);
  //disply_world(mao,0xff0000,180,280,24,20);
  disply_world(shi,0x000000,250,200,24,20);
  disply_world(du,0x000000,250,240,24,20);
  //disply_world(mao,0xff0000,260,280,24,20);
  disply_world(qi,0x000000,300,200,24,20);
  disply_world(ya,0x000000,300,240,24,20);
  //disply_world(mao,0xff0000,320,280,24,20);
  disply_world(hai,0x000000,350,200,24,20);
  disply_world(ba,0x000000,350,240,24,20);
  //disply_world(mao,0xff0000,410,280,24,20);
}
/*
void display_bian(){
  int i,j;
  for(i=0;i<480;i++)
  {
    for(j=0;j<800;j++)
    {
      
        if(i==99)
        {
          *(p+i*800+j) = 0xFF0000;//红色
        }
        else if(i>99)
        {
          if(j==199)
          {
          *(p+i*800+j) = 0xFF0000;//红色
          }
          else if(j==399)
            {
          *(p+i*800+j) = 0xFF0000;//红色
            }
          else if(j==599)
          {
          *(p+i*800+j) = 0xFF0000;//红色
          }
          
        }
        

    }
  }
}
*/
 
int main(void)
{
    	pthread_t id_1,id_2,id_3,id_4;
    	int i,ret;
	printf("thread test!!!\n");
 
/*创建线程一*/
    ret=pthread_create(&id_1,NULL,(void  *) thread_1,NULL);
    	
 
/*创建线程二*/
    ret=pthread_create(&id_2,NULL,(void  *) thread_2,NULL);
  
 /*创建线程三*/
    ret=pthread_create(&id_3,NULL,(void  *) thread_3,NULL);  	

    	
 
	while(1)
	{
		sleep(1);		//do something or delay
		printf("main thread!\n");
	}
 
    return 0;
}