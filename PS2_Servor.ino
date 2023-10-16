#include "include.h"

#define  BEEP_PIN    12              //定义蜂鸣器引脚D12
#define  NLED_PIN    13             //定义LED灯引脚D13
#define  PS2_MS_BETWEEN         50
#define  NLED_MS_BETWEEN        500
#define  DUOJI_MS_BETWEEN       20
String uart1_receive_buf = "";   //声明一个字符串数组
int get_byte = 0;       // 接收到的 data byte
boolean newLineReceived = false; // 前一次数据结束标志
boolean startBit  = false;  //协议开始标志
uint16 UartRec[9]={1500,1500,1500,1500,1500,1500,1500,1500,1500};
uint16 pwm_num=0;
/******************************************************************
  宏定义PS2手柄引脚，用有有意义的字符代表相应是引脚，便于使用
******************************************************************/
#define PS2_DAT      8
#define PS2_CMD      9
#define PS2_ATT      2
#define PS2_CLK      4
byte type = 0;

#define SER1_BAUD 115200

void Ps2Init(void);
void handle_nled();
void dida(u8 times, u8 frequency);
void TaskServo();
void Parse_str(String str);
void setup() 
{
  pinMode(BEEP_PIN, OUTPUT);
  pinMode(NLED_PIN, OUTPUT);
  Serial.begin(SER1_BAUD);
  InitPwm();
  Ps2Init();
  dida(1, 1000);
 
}


void loop() 
{
  handle_nled();
  TaskServo();
  Parse_str(uart1_receive_buf);
}

//serialEvent()是IDE1.0及以后版本新增的功能，不清楚为什么大部份人不愿意用，这个可是相当于中断功能一样的啊!  
void serialEvent()
{
  while (Serial.available()) 
  {    
    get_byte = Serial.read();              //一个字节一个字节地读，下一句是读到的放入字符串数组中组成一个完成的数据包
    if( get_byte=='B'||get_byte=='<'||get_byte=='#')
    {
      startBit= true;
    }
    if(startBit == true)
    {
       uart1_receive_buf += (char) get_byte;     // 全双工串口可以不用在下面加延时，半双工则要加的//
    }  
    if (get_byte=='\n'||get_byte=='>') 
    {
       newLineReceived = true; 
       startBit = false;
    }
  }
}


void Parse_str(String str)
{
  int motor_time=0;
  uint8 motor_num=0;      
  uint16 motor_jidu=0;    
  uint8 num_now=0;      
  uint8 jidu_now=0;     
  uint8 time_now=0;      
  uint8 flag_num=0;     
  uint8 flag_jidu=0;       
  uint8 flag_time=0;     
  uint8 i=0,j=0;  
  if (newLineReceived)
  {
       while( str[i]!='\n'  && i < 255)
      {
        if(flag_num==1)         
        {
          if(str[i]!='P')      
          {
            num_now=str[i]-'0';
            motor_num=motor_num*10+num_now;
          }
          else             
            flag_num=0;
        }
    
        if(flag_jidu==1)       
        {
          if((str[i]!='T')&(str[i]!='#')) 
          {            
            jidu_now=str[i]-'0';
            motor_jidu=motor_jidu*10+jidu_now;
          }
          else            
          {
            flag_jidu=0;
            if(motor_jidu>2500)
              motor_jidu=2500;
            UartRec[motor_num]=motor_jidu;
            pwm_num=motor_num;
            motor_jidu=0;
            motor_num=0;
          }
        }
    
        if(flag_time==1 && str[i]!='\r')       
        {
              time_now=str[i]-'0';
              motor_time=motor_time*10+time_now;
              UartRec[0]=motor_time;
          if(str[i+1]=='\r')
          { 
              //Serial.println(pwm_num);
             // Serial.println(UartRec[pwm_num]);
             // Serial.println( UartRec[0]);
            for(j=1;j<7;j++)
            {
             ServoSetPluseAndTime(j,UartRec[j],UartRec[0]);
            }

          }
        }
      
        if(str[i]=='#')
          flag_num=1;
        if(str[i]=='P')
          flag_jidu=1;
        if(str[i]=='T')
          flag_time=1;
        i++;
      }   
  
      newLineReceived=false;
      uart1_receive_buf=""; 
   } 
}




void Ps2Init(void)
{


  switch (type) {
    case 0:
      Serial.println("Unknown Controller type");
      break;
    case 1:
      Serial.println("DualShock Controller Found");
      break;
    case 2:
      Serial.println("GuitarHero Controller Found");
      break;
  } 
 }

 unsigned char handle_ms_between( unsigned long *time_ms, unsigned int ms_between) {
  if (millis() - *time_ms < ms_between) {
    return 0;
  } else {
    *time_ms = millis();
    return 1;
  }
}
unsigned char val = 0;
void handle_nled() {
  static unsigned long systick_ms_bak = 0;
  if (!handle_ms_between(&systick_ms_bak, NLED_MS_BETWEEN))return;
  digitalWrite(NLED_PIN, val);
  val = ~val;
}
void dida(u8 times, u8 frequency) {
  for (byte i = 0; i < times; i++ ) {
    digitalWrite(BEEP_PIN, LOW);
    delay(frequency);
    delay(frequency);
    digitalWrite(BEEP_PIN, HIGH );
    delay(frequency);
    delay(frequency);
  }
}

void TaskServo()
{
  static unsigned long systick_ms_bak = 0;
  if (!handle_ms_between(&systick_ms_bak, DUOJI_MS_BETWEEN))return; //每隔20MS调用一次 ServoPwmPluseCompare()
  ServoPwmDutyCompare(); 
 }
