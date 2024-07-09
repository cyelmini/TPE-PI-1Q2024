# Flag
CC = gcc
CFLAGS = -pedantic -std=c99 -Wall -fsanitize=address

# Files
OBJECTS = parkingTickets.c parkingTicketsADT.c htmlTable.c

# Output files
QUERYS = query1.csv query2.csv query3.csv
HTML = query1.html query2.html query3.html

#
OUT1 = parkingTicketsNYC
OUT2 = parkingTicketsCHI

# Targets
all: $(OUT1) $(OUT2)

# Build
$(OUT1): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(OUT1) $(CFLAGS)

$(OUT2): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(OUT2) $(CFLAGS)

# Clean
clean:
	rm -f $(QUERYS) $(OUT1) $(OUT2) $(HTML)