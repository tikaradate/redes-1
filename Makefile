CC     = gcc -g -std=c11
CFLAGS = -Wall

    PROG1 = cliente
	PROG2 = servidor
    OBJS = comms.o mensagem.o

.PHONY: clean all

%.o: %.c %.h utils.h
	$(CC) -c $(CFLAGS) $<

$(PROG1) : % :  $(OBJS) %.o
	$(CC) -o $@ $^ $(LFLAGS)

$(PROG2) : % :  $(OBJS) %.o
	$(CC) -o $@ $^ $(LFLAGS)

clean: 
	@rm -f *.o
	@rm -f $(PROG1) $(PROG2)