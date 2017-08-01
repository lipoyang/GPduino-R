GPduino-R
=========

## 概要
GPduino-Rは、ESP8266搭載のWiFiロボット制御ボードです。

以下のことができます。
* ラジコンサーボ最大16個を制御
* DCモータ2個を制御 (最大連続1.2A / 最大瞬間3.0A)
* 電源 3.6V～7.2V (4.8～6V推奨)、電源電圧を監視
* WiFiによる通信　(APにもSTAにもなれる)

## ファイル一覧

* hard/GPduino-R/
	* GPduino-R.sch/brd: 回路/基板設計データ(Eagle形式)
	* GPduino-R.pdf: 回路図(PDF)
	* GPduino-R_BOM.xlsx: BOMリスト(Excel形式)
	* GPduino-R.GBL/GBO/GBP/GBS/GML/GTL/GTO/GTP/GTS/TXT: ガーバデータ
* firm/
	* GPduinoR/: Arduinoライブライリ
	* Mini4LD/Mini4LD.ino: 四足歩行ロボットのArduinoスケッチ
* LICENSE: Apache Licence 2.0です。
	* ただし、回路/基板設計は、Creative Commons Attribution Share-Alike 3.0です。
* README.md これ

## Arduino開発環境のセットアップ
GPduino-R はESP8266を搭載しており、Arduinoでファームウェアを開発できます。

* まず、ArduinoIDE をインストールしておきます。 (ここでは 1.6.9 を使用)
* [ファイル] > [環境設定] > [Additional Boards Manager URLs:]に下記URLをコピペ
	* http://arduino.esp8266.com/stable/package_esp8266com_index.json (安定版)
	* http://arduino.esp8266.com/staging/package_esp8266com_index.json (開発版)
* [ツール] > [ボード] > [ボードマネージャ] で [esp8266] をインストール
* [ツール] > [ボード] で [Generic ESP8266 Module]を選択
	* Flash Mode : QIO
	* Flash Frequency : 40MHz
	* Upload Using : Serial
	* CPU Frequency : 80MHz
	* Flash Size : 4M(3M SPIFFS)　(3MBをファイルシステムで使用 / 1MBをスケッチで使用)
	* Debug port : Disabled
	* Debug Level : None
	* Reset Method : nodemcu
	* Upload Speed : 115200

## Arduinoライブラリのインストール

firm/ にある GPduinoR/ フォルダを、Arduinoのライブラリフォルダにコピーします。

Arduinoのライブラリフォルダは、以下の場所です。
* Documents/Arduino/libraries (Windows10, Mac, Linux)
* My Documents/Arduino/libraries (Windows7)

## Arduinoスケッチの作成
GPduinoR.h をインクルードします。
``` c
#include <GPduinoR.h>
```
すると、以下のオブジェクトが使用できます。詳細は、GPduinoR/フォルダにあるソースを参照してください。
- Battery: バッテリー電圧の取得
- DcMotor: DCモータの制御
- RcServo: ラジコンサーボの制御
- UdpComm: WiFiによるUDP/IP通信

Mini4LD/Mini4LD.ino は、四足歩行ロボット([歩くミニ四駆](https://www.youtube.com/watch?v=Bx3RYRKqrKQ))のArduinoスケッチです。参考にしてください。

## Arduinoスケッチの書き込み

* GPduino-RをUSBケーブルでPCに接続します。
* 書き込み時もUSBからの給電は受けません。CN1から給電してください。
* Windows 7以降であればドライバが自動でインストールされ、COMポートとして認識されます。
	* 自動でデバイスが認識されない環境ではFTDIのデバイスドライバをインストールしてください。
* [ツール] > [シリアルポート]でGPduino-RのCOMポート番号を指定します。
* BOOTボタン(SW2)を押し下げた状態で、RESETボタン(SW1)を押して離します。
* Uploadボタンで、ファームウェアを書き込みます。

## 既知の問題点
* サーボのコネクタの間隔が狭すぎる。(干渉して16個全部を接続できない)
