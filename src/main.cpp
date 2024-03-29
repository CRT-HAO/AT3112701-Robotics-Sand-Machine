/**
 *  Author: 張皓鈞(HAO) m831718@gmail.com
 *  Create Date: 2023-06-16 03:02:15
 *  Editor: 張皓鈞(HAO) m831718@gmail.com
 *  Update Date: 2023-12-05 00:43:42
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
#include "Watchdog.hpp"

FastAccelStepperEngine engine = FastAccelStepperEngine();
SandMachine machine;
MoveManager manager;
#ifdef ENABLE_WATCHDOG
Watchdog watchdog;
#endif

void setup() {
  // 初始化串口
  Serial.begin(115200);

  // 初始化步進引擎
  engine.init();

  // 初始化步進馬達0
  FastAccelStepper* stepper0 = engine.stepperConnectToPin(STP0_PUL);
  if (stepper0)
    stepper0->setDirectionPin(STP0_DIR);

  // 初始化步進馬達1
  FastAccelStepper* stepper1 = engine.stepperConnectToPin(STP1_PUL);
  if (stepper1)
    stepper1->setDirectionPin(STP1_DIR);

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

size_t read_data = 0;
float x, y;
String buffer;
void loop() {
  // 處裡串口通訊
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == ';') {
      float n = buffer.toFloat();
      if (read_data == 0) {
        x = n;
        read_data += 1;
      } else {
        y = n;
        CartesianCoord coord(x, y);
        manager.addMove(Move(coord));
        read_data = 0;
      }
      buffer.clear();
    } else
      buffer += c;
  }

  // 刷新位置
  CartesianCoord move = manager.move();
  machine.setPos(move);

#ifdef ENABLE_WATCHDOG
  // Watchdog 馬達失控保護
  watchdog.watch(machine);
#endif

  // 回傳緩存區剩餘空間
  // Serial.println(manager.getBufferSpace());
}
