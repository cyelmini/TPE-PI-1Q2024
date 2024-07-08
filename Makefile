CC = gcc
CFLAGS = -pedantic -std=c99 -Wall -fsanitize=address
OBJECTS = parkingTickets.c parkingTicketsADT.c htmlTable.c 
QUERYS = query1.csv query2.csv query3.csv
HTML = query1.html query2.html query3.html
OUT = parkingTicketsNYC parkingTicketsCHI

all: 
	$(CC) $(OBJECTS) -o $(OUT) $(CFLAGS)

clean: 
	rm -f $(QUERYS) $(OUT) $(HTML)

