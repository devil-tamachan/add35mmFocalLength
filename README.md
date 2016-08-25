# add35mmFocalLength
add FocalLengthIn35mmFormat to jpg's EXIF

35mm���Z �œ_�������v�Z����jpeg��EXIF�֒ǉ����܂��B�J�����̃��[�J�[���A���i���A�œ_�������܂܂�Ă���{�f�[�^�x�[�X�ɃZ���T�[�T�C�Y���o�^����Ă���ꍇ�Ajpg��EXIF��35mm���Z �œ_������ǉ����܂��B

#### �g���� (Windows)�F
 1. jpg��jpg�̓������t�H���_���R�s�[�\��t���ȂǂŃo�b�N�A�b�v���ĉ������B������ʐ^���j�����Ă��߂��܂���B�K���o�b�N�A�b�v���Ă��������B
 2. �N��������ʂɃt�H���_��jpg���h���b�O�A���h�h���b�v
 3. �ϊ��ɐ��������OK�\�����ł܂��B�ϊ��ł��Ȃ�/����35mm���Z�œ_����������/���s�����Ȃǂ̏ꍇ�A!�\�����ł܂��B


#### �g���� (Mac OSX, Linux[Linux�͓��얢�m�F�B�����͂�...]):
 1. �\�[�X���_�E�����[�h����make
 2. ./add35mmFocalLength ../../jpeg/*.jpg �݂����Ȋ����Ŏg���܂�

#### �t�@�C���̐���:
 - add35mmFocalLength.c �v���O�����{��
 - exif.c/.h exif���C�u����
 - initdb.c jpg����f�[�^�x�[�X�̂ЂȌ`�����v���O����
 - sample_main.c exif�R�}���h�̃\�[�X�Bexif�R�}���h�͊֘Aexif��\������R�}���h
 - imgui-1.49/examples/sdl_opengl_example �J�����f�[�^�ł����ݗp�c�[��
 - WinGUI/ Windows�pGUI�\�[�X


### Special Thanks!
#### EXIF sample:
* https://github.com/openMVG/CameraSensorSizeDatabase (MIT license) Copyright (c) 2014 Camera Sensor Size Database authors.
* <A href="http://owl.phy.queensu.ca/~phil/exiftool/sample_images.html">ExifTool Meta Information Repository</A>

#### Library:
* exif c library (Apache License 2.0) (https://github.com/mkttanabe/exif): Copyright (C) 2013 KLab Inc.
* sqlite (public domain)