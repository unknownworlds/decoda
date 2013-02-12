copy .\images\*.* .\output
tex2rtf decoda.tex output/Decoda -html -twice -macros tex2rtf.ini
"C:\Program Files (x86)\HTML Help Workshop\hhc" output/Decoda.hhp
pause
start output/Decoda.chm
