TARGET=hw3
HTML=main_html
SRC={hw3}

default: all 

all:
	latex hw3.tex
	dvips -R -Poutline -t letter hw3.dvi -o hw3.ps
	ps2pdf hw3.ps
	rm hw3.aux
	rm hw3.dvi
	rm hw3.log
	rm hw3.ps