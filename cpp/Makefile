CC     = g++ 
CXXFLAGS = -Wall

    PROG1 = cliente
	PROG2 = servidor
    OBJS = misc.o mensagem.o funcoes_cliente.o funcoes_servidor.o

.PHONY: clean all

%.o: %.c %.h utils.h
	$(CC) -c $(CFLAGS) $<

all: $(PROG1) $(PROG2)

$(PROG1) : % :  $(OBJS) %.o
	$(CC) -o $@ $^ $(LFLAGS)

$(PROG2) : % :  $(OBJS) %.o
	$(CC) -o $@ $^ $(LFLAGS)

clean: 
	@rm -f *.o
	
purge: clean
	@rm -f $(PROG1) $(PROG2)