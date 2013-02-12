convert fileicons.bmp fileicons.xpm
convert explorer.bmp  explorer.xpm
convert functionicon.bmp functionicon.xpm
convert classicon.bmp classicon.xpm
convert filter_bitmap.bmp filter_bitmap.xpm

..\..\wxWidgets\bin\wxInclude.exe --const --respectcase --appendtype --input-type=.png --output-file=..\Bitmaps.h
