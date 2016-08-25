# add35mmFocalLength
add FocalLengthIn35mmFormat to jpg's EXIF

35mm換算 焦点距離を計算してjpegのEXIFへ追加します。カメラのメーカー名、製品名、焦点距離が含まれている＋データベースにセンサーサイズが登録されている場合、jpgのEXIFへ35mm換算 焦点距離を追加します。

#### 使い方 (Windows)：
 1. jpgやjpgの入ったフォルダをコピー貼り付けなどでバックアップして下さい。万が一写真が破損しても戻せません。必ずバックアップしてください。
 2. 起動した画面にフォルダやjpgをドラッグアンドドロップ
 3. 変換に成功するとOK表示がでます。変換できない/既に35mm換算焦点距離がある/失敗したなどの場合、!表示がでます。


#### 使い方 (Mac OSX, Linux[Linuxは動作未確認。動くはず...]):
 1. ソースをダウンロードしてmake
 2. ./add35mmFocalLength ../../jpeg/*.jpg みたいな感じで使えます

#### ファイルの説明:
 - add35mmFocalLength.c プログラム本体
 - exif.c/.h exifライブラリ
 - initdb.c jpgからデータベースのひな形を作るプログラム
 - sample_main.c exifコマンドのソース。exifコマンドは関連exifを表示するコマンド
 - imgui-1.49/examples/sdl_opengl_example カメラデータ打ち込み用ツール
 - WinGUI/ Windows用GUIソース


### Special Thanks!
#### EXIF sample:
* https://github.com/openMVG/CameraSensorSizeDatabase (MIT license) Copyright (c) 2014 Camera Sensor Size Database authors.
* <A href="http://owl.phy.queensu.ca/~phil/exiftool/sample_images.html">ExifTool Meta Information Repository</A>

#### Library:
* exif c library (Apache License 2.0) (https://github.com/mkttanabe/exif): Copyright (C) 2013 KLab Inc.
* sqlite (public domain)