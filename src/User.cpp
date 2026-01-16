#include <board.h>

void sensorMonitor();

void setup() {
}

void loop() {
	int lineL, lineR;
	int error;
	int controlValue;
	int speedL, speedR;
	float pGain;
    int min_speed = 0;
    int max_speed = 100;
    int base_speed;
    int dynamic_speed;
    int speed_switch = 0;

    /* BTN_1を押すまで待つ */
    while (digitalRead(PIN_BTN1) == LOW) {}

    while (1) {
    	pGain = (float)analogRead(PIN_VOLUME) / 100.0f;
        lineL = analogRead(PIN_LINE_L); //地面の明るさ左 0~4095
        lineR = analogRead(PIN_LINE_R); //地面の明るさ右 0~4095

        speed_switch = analogRead(PIN_TOGGLE);

        switch(speed_switch){

        	case 0 :
        		base_speed = 40;
        		break;

        	case 1 :
        		base_speed = 60;
        		break;

        	case 2 :
        		base_speed = 80;
        		break;

        	default :
        		base_speed = 40;
        }



        error = lineL - lineR;  //偏差 -4095~4095
        controlValue = (int)((float)error / 40.0f *  pGain);

        dynamic_speed = base_speed - (abs(controlValue) / 2);
        speedL = dynamic_speed - controlValue;
        speedR = dynamic_speed + controlValue;



        if(speedL < min_speed){
        	speedL = min_speed;
        } else if (speedL > max_speed){
        	speedL = max_speed;
        }

        if(speedR < min_speed){
        	speedR = min_speed;
        } else if (speedR > max_speed){
        	speedR = max_speed;
        }

        analogWrite(PIN_MOTOR_L, speedL);
        analogWrite(PIN_MOTOR_R, speedR);

        sensorMonitor();

        delay(10);
    }
}

void sensorMonitor(){
    static uint32_t lastLcdUpdate = 0;

    /* 50[ms]毎に処理を実行 */
    /* ※早すぎると数字が読み取れなくなるため */
    if ((millis() - lastLcdUpdate) >= 50) {
        lastLcdUpdate = millis();

        LcdDrv_setCursor(0, 0);
        LcdDrv_print("L");
        LcdDrv_setNum(analogRead(PIN_LINE_L), 4);

        LcdDrv_print(" R");
        LcdDrv_setNum(analogRead(PIN_LINE_R), 4);

        LcdDrv_setCursor(1, 0);
        LcdDrv_print("V");
        LcdDrv_setNum(analogRead(PIN_VOLUME), 3);

        LcdDrv_print(" T");
        LcdDrv_setNum(analogRead(PIN_TOGGLE), 1);

        LcdDrv_update();
    }
}
