TARGET=hw2_write_up
HTML=main_html
SRC={hw2_write_up}

default: all 

all:
	latex hw2_write_up.tex
	dvips -R -Poutline -t letter hw2_write_up.dvi -o hw2_write_up.ps
	ps2pdf hw2_write_up.ps
	rm hw2_write_up.aux
	rm hw2_write_up.dvi
	rm hw2_write_up.log
	rm hw2_write_up.ps