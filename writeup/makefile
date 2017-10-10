TARGET=hw1_write_up
HTML=main_html
SRC={hw1_write_up}

default: all 

all:
	latex hw1_write_up.tex
	dvips -R -Poutline -t letter hw1_write_up.dvi -o hw1_write_up.ps
	ps2pdf hw1_write_up.ps
	rm hw1_write_up.aux
	rm hw1_write_up.dvi
	rm hw1_write_up.log
	rm hw1_write_up.out
	rm hw1_write_up.ps