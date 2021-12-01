CC     = g++ 
CXXFLAGS = -Wall -Wextra -g 

    PROG1 = cliente
	PROG2 = servidor
    OBJS = comms.o mensagem.o

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
	@rm -f $(PROG1) $(PROG2)