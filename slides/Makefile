all: slides.pdf

.PHONY: slides.pdf

slides.pdf:
	latexmk --pdf slides.tex
	make clean

auto:
	latexmk --pdf -pvc -interaction=nonstopmode slides.tex

clean:
	rm slides.aux slides.fdb_latexmk slides.fls slides.log slides.toc slides.out slides.dvi slides.nav slides.snm	