copy .\images\*.* .\output
tex2rtf decoda.tex output/Decoda -html -twice -macros tex2rtf.ini
"C:\Program Files\HTML Help Workshop\hhc" output/Decoda.hhp
pause
start output/Decoda.chm
