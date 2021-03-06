#include <stdlib.h>

#include "ch.h"     // needs for all ChibiOS programs
#include "hal.h"    // hardware abstraction layer header
#include "vex.h"    // vex library header

void setShooter(int s);
void setIntake(int speed);
void setPneumatic(int value);
void setArm(int value);
int motorOn;
int intakeUp = 0;
int shooting;

static vexDigiCfg dConfig[kVexDigital_Num] = {
  { kVexDigital_1,    kVexSensorDigitalOutput, kVexConfigOutput,      0 },
  { kVexDigital_2,    kVexSensorDigitalOutput, kVexConfigOutput,      0 },
  { kVexDigital_3,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
  { kVexDigital_4,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
  { kVexDigital_5,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
  { kVexDigital_6,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
  { kVexDigital_7,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
  { kVexDigital_8,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
  { kVexDigital_9,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
  { kVexDigital_10,   kVexSensorDigitalInput,  kVexConfigInput,       0 },
  { kVexDigital_11,   kVexSensorDigitalInput,  kVexConfigInput,       0 },
  { kVexDigital_12,   kVexSensorDigitalInput,  kVexConfigInput,       0 }
};

static vexMotorCfg mConfig[kVexMotorNum] = {
  { kVexMotor_1,      kVexMotorUndefined,      kVexMotorReversed,     kVexSensorNone,        0 },
  { kVexMotor_2,      kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
  { kVexMotor_3,      kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
  { kVexMotor_4,      kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
  { kVexMotor_5,      kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
  { kVexMotor_6,      kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
  { kVexMotor_7,      kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
  { kVexMotor_8,      kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
  { kVexMotor_9,      kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
  { kVexMotor_10,     kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 }
};

#define shooterLeft kVexMotor_3
#define shooterRight kVexMotor_2
#define intakeMotor kVexMotor_4
#define relay kVexDigital_1
#define arm kVexDigital_2

void vexUserSetup() {
  vexDigitalConfigure(dConfig, DIG_CONFIG_SIZE(dConfig));
  vexMotorConfigure(mConfig, MOT_CONFIG_SIZE(mConfig));
}

void vexUserInit() {
  // ...
}

msg_t vexAutonomous(void *arg) {
  (void)arg;
  vexTaskRegister("auton");

  while (1) {
    vexSleep(25);
  }

  return (msg_t)0;
}

msg_t vexOperator(void *arg) {
  (void)arg;
  vexTaskRegister("operator");

  int armPress = 0;
  int shotPress = 0;
  int intakePress = 0;

  while (!chThdShouldTerminate()) {

    //joystick and button motor control
    if(vexControllerGet(Btn8U))
      setShooter(127);
    else if(abs(vexControllerGet(Ch3)) > 15)
      setShooter(vexControllerGet(Ch3));
    else
      setShooter(0);

    // toggle shooter pneumatic
    if(!shotPress && vexControllerGet(Btn6D))
      setPneumatic(shooting ?  kVexDigitalLow : kVexDigitalHigh);
    shotPress = vexControllerGet(Btn6D);

    //intake is controlled with a joystick and the buttons
    if(vexControllerGet(Btn8L))
      setIntake(127);
    else if(vexControllerGet(Btn8D))
      setIntake(-127);
    else if(vexControllerGet(Btn6U))
      setIntake(127);
    else if(abs(vexControllerGet(Ch2)) <= 15)
      setIntake(0);//deadzone
    else
      setIntake(vexControllerGet(Ch2));

    //toggle intake pneumatic
    if(!armPress && vexControllerGet(Btn5U)){
      setArm(intakeUp ? kVexDigitalLow : kVexDigitalHigh);
      intakeUp = !intakeUp;
    }else if(!intakePress && vexControllerGet(Btn6U)){
      setArm(kVexDigitalHigh);
      intakeUp = true;
    }else if(intakePress && !vexControllerGet(Btn6U)){
      setArm(kVexDigitalLow);
      intakeUp = false;
    }
    armPress = vexControllerGet(Btn5U);
    intakePress = vexControllerGet(Btn6U);

    vexSleep(20);//don't starve other threads
  }

  return (msg_t)0;
}

void setShooter(int s) {
  vexMotorSet(shooterLeft, s);
  vexMotorSet(shooterRight, s);
  motorOn = s == 127;
}

void setIntake(int speed){
  vexMotorSet(intakeMotor, speed);
}

// sets the pneumatic relay to on or off
void setPneumatic(int value){
  vexDigitalPinSet(relay, value);
  shooting = value == kVexDigitalHigh;
}

// sets the state of the intake arm
void setArm(int value){
  vexDigitalPinSet(arm,value);
}
