/**
 *  Author: 張皓鈞(HAO) m831718@gmail.com
 *  Create Date: 2023-06-16 03:02:15
 *  Editor: 張皓鈞(HAO) m831718@gmail.com
 *  Update Date: 2023-06-19 17:17:58
 *  Description: Sand Machine
 */

#include <Arduino.h>
#include <FastAccelStepper.h>

#include "Config.hpp"
#include "Coords.hpp"
#include "Move.hpp"
#include "MoveManager.hpp"
#include "SandMachine.hpp"
#include "Utilities.hpp"

FastAccelStepperEngine engine = FastAccelStepperEngine();
SandMachine machine;

CartesianCoord current(0, 0);
float speed = MOVE_SPEED;  // mm/s

MoveManager manager;

// void moveTest(const CartesianCoord& end) {
//   CartesianCoord start = current;
//   CartesianCoord dis = end - start;
//   PolarCoord dis_polar = Utilities::cartesianToPolar(dis);
//   float r = dis_polar.r;
//   unsigned long move_time = (r / speed) * 1000;
//   unsigned long start_time = millis();
//   while (millis() - start_time < move_time) {
//     unsigned long elapsed = millis() - start_time;
//     float current_r = r * min((float)elapsed / (float)move_time, 1.0f);
//     PolarCoord move_polar = dis_polar;
//     move_polar.r = current_r;
//     CartesianCoord move = Utilities::polarToCartesian(move_polar) + start;

//     machine.setPos(move.x, move.y);
//     current = move;
//     Serial.printf("%f %f\n", move.x, move.y);
//   }
// }

void setup() {
  // 初始化串口
  Serial.begin(115200);

  // 初始化步進引擎
  engine.init();

  // 初始化步進馬達0
  FastAccelStepper* stepper0 = engine.stepperConnectToPin(STP0_PUL);
  if (stepper0) {
    stepper0->setDirectionPin(STP0_DIR);
  }

  // 初始化步進馬達1
  FastAccelStepper* stepper1 = engine.stepperConnectToPin(STP1_PUL);
  if (stepper1) {
    stepper1->setDirectionPin(STP1_DIR);
  }

  // 設置控制器
  machine.setStepper0(stepper0);
  machine.setStepper1(stepper1);
  machine.setStepper0ResetPin(STP0_RST_PIN);
  machine.setStepper1ResetPin(STP1_RST_PIN);

  // 初始化控制器
  machine.init();

  // 回原點
  machine.home();

  delay(500);
}

void loop() {
  if (Serial.available() > 0) {
    float x = Serial.parseFloat();
    float y = Serial.parseFloat();
    CartesianCoord coord(x, y);
    manager.addMove(Move(coord));
  }

  // 刷新位置
  machine.setPos(manager.move());
}