#include <stdlib.h>

#include "ch.h"     // needs for all ChibiOS programs
#include "hal.h"    // hardware abstraction layer header
#include "vex.h"    // vex library header

void setShooter(int s);
void setPneumatic(int value);
int pneumaticUp;
int motorOn;

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

#define shooterLeft kVexMotor_1
#define shooterRight kVexMotor_2
#define relay kVexDigital_1

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

  int buttonPressed = 0;
  int currentRamp = 0;

  while (!chThdShouldTerminate()) {
    //ramping
    currentRamp += (vexControllerGet(Ch3) - currentRamp) * (vexControllerGet(Btn6D) ? .2 : .42);

    // Btn6U will turn the motors to full speed
    if (vexControllerGet(Btn6U))
      setShooter(127);
    else if(abs(vexControllerGet(Ch3)) < 15)
      // deadzone
      setShooter(0);
    else
      setShooter(vexControllerGet(Btn5D) ? vexControllerGet(Ch3) : currentRamp);
    // if button 5d is pressed no ramping

    // toggle pneumatic
    if(!buttonPressed && vexControllerGet(Btn5U))
      setPneumatic(pneumaticUp ? kVexDigitalLow : kVexDigitalHigh);
    buttonPressed = vexControllerGet(Btn5U);


    vexSleep(50);//don't starve other threads
  }

  return (msg_t)0;
}

void setShooter(int s) {
  vexMotorSet(shooterLeft, s);
  vexMotorSet(shooterRight, s);
}

// sets the pneumatic relay to on or off
void setPneumatic(int value){
  vexDigitalPinSet(relay, value);
  pneumaticUp = value == kVexDigitalHigh;
}
