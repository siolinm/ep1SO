DIRNAME=ep1-lucas-marcos
SOBJS  = algo.o algo1.o
EPOBJS = outroalgo.o outroalgo1.o
CFLAGS = -Wall -ansi -g -pedantic -Wno-unused-result

bccsh: $(SOBJS)
	gcc $(CFLAGS) $(SOBJS) -o bccsh

ep1: $(EPOBJS)
	gcc $(CFLAGS) $(EPOBJS) -o ep1

%.o: %.c # %.h
	gcc $(CFLAGS) -c $<

tar:	
	rm -Rf $(DIRNAME)
	rm -f $(DIRNAME).tar.gz
	mkdir $(DIRNAME)

	# copia os slides, o LEIAME e o Makefile
	cp LEIAME $(DIRNAME)/LEIAME
	cp Makefile $(DIRNAME)/Makefile
	cp slides/slides.pdf $(DIRNAME)/slides.pdf

	# copia o codigo fonte
	for a in *.c; do \
		cp $$a $(DIRNAME)/$$a; \
	done

	# (se houverem .h)
	# for a in *.h; do \
	# 	cp $$a $(DIRNAME)/$$a; \
	# done

	# gera $(DIRNAME).tar.gz
	tar -czvf $(DIRNAME).tar.gz $(DIRNAME)
	rm -Rf $(DIRNAME)