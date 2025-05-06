#include "robot_arm.h"

// Crée l'objet PCA9685 en utilisant l'adresse I2C par défaut (0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Constantes pour les angles des servos
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

// Pulses (valeurs dépendant du type de servo)
#define SERVO_MIN_PULSE  500   // en microsecondes
#define SERVO_MAX_PULSE  2500  // en microsecondes

#define L1 7
#define L2 7.6

// Canaux utilisés
const uint8_t servoChannels[4] = {12, 13, 14, 15};


uint16_t angleToPulse1(int angle) 
{
  int pulseUs = map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  // 4096 ticks = 20 ms (à 50 Hz), soit 1 tick = ~4.88 µs
  return (uint16_t)((pulseUs * 4096L) / 20000L);
}

int angleToPulse2(int angle) 
{
  if(angle < 0)
  {
    return SERVO_MIN_PULSE;
  }
    
  if(angle > 120)
  {
    return SERVO_MAX_PULSE;
  }

  return ((SERVO_MAX_PULSE - SERVO_MIN_PULSE)/120)*angle + SERVO_MIN_PULSE;
}

int angleToPulse(int angle) 
{
  angle = constrain(angle, 0, 180);
  int pulseUs = map(angle, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  int ticks = int((pulseUs * 4096L) / 20000L);
  return ticks;
}

int rad_to_deg(double rad)
{
  return (180/M_PI)*rad;
}

void grab()
{
  pwm.setPWM(servoChannels[3], 0, angleToPulse(0));
}

void release()
{
  pwm.setPWM(servoChannels[3], 0, angleToPulse(60));
}



void kinematics(double x, double y)
{
  double a1(0), a2(0), a3(0);

  
  a1 = atan(y/x)+M_PI-acos((1/(2*L1*sqrt(pow(x, 2)+pow(y, 2))))*(pow(L1, 2)+pow(x, 2)+pow(y, 2)-pow(L2, 2)));
  a2 = asin((-1/L2)*(x*sin(a1)-y*cos(a1)));
  a3 = a1-a2; 

  Serial.print("Position M: x = ");
  Serial.print(x);
  Serial.print(" y = ");
  Serial.println(y);

  Serial.print("Angles: theta_1 = " );
  Serial.print(a1);
  Serial.print(" theta_2 = ");
  Serial.println(a2);

  Serial.print("Angles after conversion: theta_1 = ");
  Serial.print(rad_to_deg(a1));
  Serial.print(" theta_2 = ");
  Serial.println(rad_to_deg(a2));

  Serial.println("____________________________________________");

  
  pwm.setPWM(servoChannels[0], 0, angleToPulse(rad_to_deg(a1)));
  pwm.setPWM(servoChannels[1], 0, angleToPulse(rad_to_deg(a2)));
  pwm.setPWM(servoChannels[2], 0, angleToPulse(10 + rad_to_deg(a3)));
}

void follow_line(double x_start, double y_start, double x_stop, double y_stop, double step)
{

  if(x_start == x_stop)
  {
    if(y_stop <= y_start)
    {
      for(double t(y_start); t >= y_stop; t -= step)
      {
        kinematics(x_start, t);
        delay(20);
      }
      
    }else{
      for(double t(y_start); t <= y_stop; t += step)
      {
        kinematics(x_start, t);
        delay(20);
      }
    }

  }else{
    double a = (y_start-y_stop)/(x_start-x_stop);
    double b = y_stop - a*x_stop;

    if(x_stop >= x_start)
    {
      for(double t(x_start); t<=x_stop; t+=step)
      {
        kinematics(t, a*t+b);
        delay(20);
      }

    }else{
      for(double t(x_start); t>=x_stop; t-=step)
      {
        kinematics(t, a*t+b);
        delay(20);
      }
    }
  }
}

void init_pos_robotic_arm()
{
  release();
  delay(20);
  
  kinematics(-1.5, 3.5);
  delay(20);
  
  grab();

  delay(100);
}

void take_with_robotic_arm()
{
  release();

  follow_line(-1.5, 3.5, -9, 5, 0.2);
  delay(50);

  follow_line(-9, 5, -9, -5, 0.2);
  delay(50);

  grab();

  delay(100);

  follow_line(-9, -5, -9, 5, 0.2);
  delay(50);

  follow_line(-9, 5, -1.5, 3.5, 0.2);
  delay(50);
}

void pose_object_with_robotic_arm()
{
  follow_line(-1.5, 3.5, -9, 5, 0.2);
  delay(50);

  follow_line(-9, 5, -9, -5, 0.2);
  delay(50);

  release();

  delay(100);

  follow_line(-9, -5, -9, 5, 0.2);
  delay(50);

  follow_line(-9, 5, -1.5, 3.5, 0.2);
  delay(50);

  grab();
}


void setup_arm() 
{
  Wire.begin(18, 19);

  pwm.begin();
  pwm.setPWMFreq(50);  // Fréquence standard pour les servos = 50 Hz

  delay(500);
  
  init_pos_robotic_arm();

  Serial.println("Servomoteurs prêts.");

}

void demo_ctrl_servos() 
{

  for (int angle = 0; angle <= 180; angle += 5) 
  {
    for (int i = 0; i < 4; i++) 
    {
      pwm.setPWM(servoChannels[i], 0, angleToPulse(angle));
    }
    delay(20);
  }

  for (int angle = 180; angle >= 0; angle -= 5) 
  {
    for (int i = 0; i < 4; i++) 
    {
      pwm.setPWM(servoChannels[i], 0, angleToPulse(angle));
    }
    delay(20);
  }

  delay(1000);
}

void demo_ctrl_servos_2() 
{
  take_with_robotic_arm();

  delay(2000);

  pose_object_with_robotic_arm();

  delay(2000);
  
}



