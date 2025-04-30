#include "robot_arm.h"

// Crée l'objet PCA9685 en utilisant l'adresse I2C par défaut (0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Constantes pour les angles des servos
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

// Pulses (valeurs dépendant du type de servo)
#define SERVO_MIN_PULSE  500   // en microsecondes
#define SERVO_MAX_PULSE  2500  // en microsecondes

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

void setup_arm() 
{

  Wire.begin(18, 19);

  pwm.begin();
  pwm.setPWMFreq(50);  // Fréquence standard pour les servos = 50 Hz

  delay(500);
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
