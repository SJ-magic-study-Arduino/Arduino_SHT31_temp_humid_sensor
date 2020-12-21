/************************************************************
■参考
	■Arduinoではじめる電子工作 超入門
		https://www.amazon.co.jp/%E3%81%93%E3%82%8C1%E5%86%8A%E3%81%A7%E3%81%A7%E3%81%8D%E3%82%8B%EF%BC%81Arduino%E3%81%A7%E3%81%AF%E3%81%98%E3%82%81%E3%82%8B%E9%9B%BB%E5%AD%90%E5%B7%A5%E4%BD%9C-%E8%B6%85%E5%85%A5%E9%96%80-%E6%94%B9%E8%A8%82%E7%AC%AC3%E7%89%88-%E7%A6%8F%E7%94%B0-%E5%92%8C%E5%AE%8F/dp/4800712106
		p169
		
	■SHT31の使い方
		https://spiceman.jp/arduino-sht31-program/
	
	■debug lab
		■温湿度センサーSHT31を使う場合の注意点（パート1）
			https://debuglab.jp/some_tips_using_sht31_part_1/
		
		■温湿度センサーSHT31を使う場合の注意点（パート2）
			https://debuglab.jp/some_tips_using_sht31_part_2/
			
		■温湿度センサーSHT31を使う場合の注意点（パート3）
			https://debuglab.jp/some_tips_using_sht31_part_3/
			
		■温湿度センサーSHT31を使う場合の注意点（まとめ）
			https://debuglab.jp/some_tips_using_sht31_conclusion/
			
		■温湿度センサーSHT31をクロックストレッチで使う時の備忘録
			https://debuglab.jp/tag/%E3%82%AF%E3%83%AD%E3%83%83%E3%82%AF%E3%82%B9%E3%83%88%E3%83%AC%E3%83%83%E3%83%81/
		
	■Arduino IDEの簡単グラフ表示機能 : serial plotter
		https://programresource.net/2020/02/26/2996.html
		
	■ＳＨＴ３１使用　高精度温湿度センサモジュールキット
		https://akizukidenshi.com/catalog/g/gK-12125/
************************************************************/
// #define PERIODIC
#define PLOTTER

/************************************************************
************************************************************/
#include <Wire.h>

/************************************************************
************************************************************/
#define SHT31_ADDR 0x45

#define SOFT_RESET_MSB 0x30
#define SOFT_RESET_LSB 0xA2

#define CLEAR_STATUS_REGISTER_MSB 0x30
#define CLEAR_STATUS_REGISTER_LSB 0x41

#define HEATER_OFF_MSB 0x30
#define HEATER_OFF_LSB 0x66

// single shot
#define SINGLE_SHOT_HIGH_MSB 0x24
#define SINGLE_SHOT_HIGH_LSB 0x00

// 測定 1回/sec
#define PERIODIC_HIGH_1_MSB 0x21
#define PERIODIC_HIGH_1_LSB 0x30

// 測定 2回/sec
#define PERIODIC_HIGH_2_MSB 0x22
#define PERIODIC_HIGH_2_LSB 0x36

// 測定 4回/sec
#define PERIODIC_HIGH_4_MSB 0x23
#define PERIODIC_HIGH_4_LSB 0x34

// 測定 10回/sec
#define PERIODIC_HIGH_10_MSB 0x27
#define PERIODIC_HIGH_10_LSB 0x37

/************************************************************
************************************************************/

/******************************
******************************/
void setup()
{
	Serial.begin(9600);
	Wire.begin();
	
	Wire.beginTransmission(SHT31_ADDR);
	Wire.write(SOFT_RESET_MSB);
	Wire.write(SOFT_RESET_LSB);
	Wire.endTransmission();
	// delay(500);
	delay(10);
	
	Wire.beginTransmission(SHT31_ADDR);
	Wire.write(CLEAR_STATUS_REGISTER_MSB);
	Wire.write(CLEAR_STATUS_REGISTER_LSB);
	Wire.endTransmission();
	// delay(500);
	delay(10);
	
	Wire.beginTransmission(SHT31_ADDR);
	Wire.write(HEATER_OFF_MSB);
	Wire.write(HEATER_OFF_LSB);
	Wire.endTransmission();
	// delay(500);
	delay(10);
	
#ifdef PERIODIC
	Wire.beginTransmission(SHT31_ADDR);
	Wire.write(PERIODIC_HIGH_10_MSB);
	Wire.write(PERIODIC_HIGH_10_LSB);
	Wire.endTransmission();
	// delay(300);//300msec待機(0.3秒待機)
	delay(30);
#endif
}

/******************************
******************************/
void loop() {
	unsigned int dac[6];
	unsigned int i, t ,h;
	float temperature, humidity;
	
#ifndef PERIODIC
	Wire.beginTransmission(SHT31_ADDR);
	Wire.write(SINGLE_SHOT_HIGH_MSB);
	Wire.write(SINGLE_SHOT_HIGH_LSB);
	Wire.endTransmission();
	// delay(300);
	delay(30);
#endif

	Wire.requestFrom(SHT31_ADDR, 6); //dI2Cデバイス「SHT31」に6Byteのデータ要求
	for ( i = 0 ; i < 6 && Wire.available() ; i++ ){
		dac[i] = Wire.read();
	}
	// Wire.endTransmission(); // これを書いてる情報が多いけど、不要。
	
	if(i == 6){
		t = (dac[0] << 8) | dac[1];
		temperature = (float)(t) * 175 / 65535.0 - 45.0;
		
		h = (dac[3] << 8) | dac[4];
		humidity = (float)(h) / 65535.0 * 100.0;
	}/*else{
		Serial.println("skip");
	}
	*/
	
#ifdef PLOTTER
	{
		float Height = 100.0;
		Serial.print(millis() / 1000 % 2 * Height);
		Serial.print(",");
		Serial.println( humidity );
		
		/********************
		Serial plotterの時間軸は、1画面でdata x 500個(500を超えるとdataが流れて行く)。
		∴ ここのdelayで時間軸を調整できる。
		...何秒に1回dataを書き出すか、で時間軸が変わる。
			delay(long);	// 中々 書き出さない = 1秒の幅が狭まる
			delay(short);	// どんどん書き出す = 1秒の幅が広がる
		********************/
		delay (100);
	}
	
#else
	Serial.print("温度：");
	Serial.print(temperature);
	Serial.println("℃");
	Serial.print("湿度：");
	Serial.print(humidity);
	Serial.println("％");
	
	delay(1000);
#endif
}

