TARGET=hw4
HTML=main_html
SRC={hw4}

default: all 

all:
	latex hw4.tex
	dvips -R -Poutline -t letter hw4.dvi -o hw4.ps
	ps2pdf hw4.ps
	rm hw4.aux
	rm hw4.dvi
	rm hw4.log
	rm hw4.ps