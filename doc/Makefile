all: main.pdf

build: 
	mkdir -p build/doc

clean: 
	rm -rf build

TEXT=$(wildcard doc/*)

main.pdf: build main.tex $(TEXT)
	pdflatex --output-directory build --shell-escape main.tex
	cp build/main.pdf main.pdf

.PHONY: all build clean
