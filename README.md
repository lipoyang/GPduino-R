GPduino-R
=========

## 概要
GPduino-Rは、ESP8266搭載のWiFiロボット制御ボードです。  
（工事中）

## ファイル一覧

* hard/GPduino-R/
	* GPduino-R.sch/brd: 回路/基板設計データ(Eagle形式)
	* GPduino-R.pdf: 回路図(PDF)
	* GPduino-R_BOM.xlsx: BOMリスト(Excel形式)
	* GPduino-R.GBL/GBO/GBP/GBS/GML/GTL/GTO/GTP/GTS/TXT: ガーバデータ
* LICENSE: Apache Licence 2.0です。
	* ただし、回路/基板設計は、Creative Commons Attribution Share-Alike 3.0です。
* README.md これ

## ファームウェアの書き込み
GPduino-R はESP8266を搭載しており、Arduinoでファームウェアを開発できます。

* GPduino-RをUSBケーブルでPCに接続します。
* 書き込み時もUSBからの給電は受けません。CN1から給電してください。
* Windows 7以降であればドライバが自動でインストールされ、COMポートとして認識されます。
	* 自動でデバイスが認識されない環境ではFTDIのデバイスドライバをインストールしてください。
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
	* シリアルポート : GPduino-RのCOMポート番号
	* 書込装置 : USBasp
* BOOTボタン(SW2)を押し下げた状態で、RESETボタン(SW1)を押して離します。
* Uploadボタンで、ファームウェアを書き込みます。
