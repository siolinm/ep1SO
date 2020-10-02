DIRNAME    = ep1-lucas-marcos
SOBJS      = bccsh.o
EPOBJS     = ep1.o util.o fcfs.o srtn.o round_robin.o
CFLAGS     = -Wall -g -pedantic -Wno-unused-result
LIBS_BCCSH = -lreadline
LIBS_EP1   = -lpthread

default: bccsh ep1 gera_tracer

bccsh: $(SOBJS)
	gcc $(CFLAGS) $(SOBJS) -o bccsh $(LIBS_BCCSH)

ep1: $(EPOBJS)
	gcc $(CFLAGS) $(EPOBJS) -o ep1 $(LIBS_EP1)

gera_tracer:
	gcc gera_tracer.c -o gera_tracer

%.o: %.c # %.h
	gcc $(CFLAGS) -c $<

tar:
	rm -Rf ../$(DIRNAME)
	rm -f ../$(DIRNAME).tar.gz
	mkdir ../$(DIRNAME)

	# copia os slides, o LEIAME e o Makefile
	cp ../LEIAME ../$(DIRNAME)/LEIAME
	cp Makefile ../$(DIRNAME)/Makefile
	cp ../slides/slides.pdf ../$(DIRNAME)/slides.pdf

	# copia o codigo fonte
	for a in *.c; do \
		cp $$a ../$(DIRNAME)/$$a; \
	done

	# (se houverem .h)
	# for a in *.h; do \
	# 	cp $$a ../$(DIRNAME)/$$a; \
	# done

	# gera ../$(DIRNAME).tar.gz
	tar -czvf ../$(DIRNAME).tar.gz ../$(DIRNAME)
	rm -Rf ../$(DIRNAME)

clean:
	rm *.o bccsh ep1 gera_tracer *~
