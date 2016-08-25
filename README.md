# add35mmFocalLength
add FocalLengthIn35mmFormat to jpg's EXIF

35mm換算 焦点距離を計算してjpegのEXIFへ追加します。カメラのメーカー名、製品名、焦点距離が含まれている＋データベースにセンサーサイズが登録されている場合、jpgのEXIFへ35mm換算 焦点距離を追加します。

#### 使い方：
 - jpgやjpgの入ったフォルダをコピー貼り付けなどでバックアップして下さい。万が一写真が破損しても戻せません。必ずバックアップしてください。
 - 起動した画面にフォルダやjpgをドラッグアンドドロップ
 - 変換に成功するとOK表示がでます。変換できない/既に35mm換算焦点距離がある/失敗したなどの場合、!表示がでます。


### Special Thanks!
#### EXIF sample:
* https://github.com/openMVG/CameraSensorSizeDatabase (MIT license) Copyright (c) 2014 Camera Sensor Size Database authors.
* <A href="http://owl.phy.queensu.ca/~phil/exiftool/sample_images.html">ExifTool Meta Information Repository</A>

#### Library:
* exif c library (Apache License 2.0) (https://github.com/mkttanabe/exif): Copyright (C) 2013 KLab Inc.
* sqlite (public domain)