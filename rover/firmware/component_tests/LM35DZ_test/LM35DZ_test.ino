// LM35DZ测试代码
// LM35DZ的输出电压与温度成正比，每摄氏度输出10mV
// 连接方式：
// - VCC连接到Arduino的5V
// - GND连接到Arduino的GND
// - OUT连接到Arduino的模拟输入引脚（如A0）

const int tempSensorPin = A0; // LM35DZ的输出引脚连接到A0

void setup() {
  Serial.begin(9600); // 初始化串口通信
  Serial.println("LM35DZ温度传感器测试");
}

void loop() {
  // 读取LM35DZ的模拟值
  int sensorValue = analogRead(tempSensorPin);

  // 将模拟值转换为电压（单位：伏特）
  // 注意：如果使用3.3V参考电压，将5.0改为3.3
  float voltage = sensorValue * (5.0 / 1023.0);

  // 将电压转换为温度（单位：摄氏度）
  // LM35DZ输出10mV/°C，因此温度 = 电压 / 0.01
  float temperature = voltage / 0.01;

  // 输出温度值，保留两位小数
  Serial.print("温度: ");
  Serial.print(temperature, 2); // 保留两位小数
  Serial.println(" °C");

  delay(1000); // 每秒读取一次
}