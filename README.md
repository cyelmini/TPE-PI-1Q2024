# Final Project - Imperative Programming

This project is designed to process and analyze parking ticket data for two major cities, New York City and Chicago. The goal is to parse, store, and organize information from parking ticket records and infraction data, and then generate .csv reports summarizing the processed data.

## Students 
- **Catalina Yelmini** (64133)
- **Francisco Trivelloni** (64141)
- **Lorenzo Pizzuto Beltrán** (64123)

## List of source and header files

- **`parkingTickets.c`**: Main source file containing the logic for processing parking tickets.
  
- **`parkingTicketsADT.c`**: Source file that implements the ADT (Abstract Data Type) for handling parking tickets.
  
- **`parkingTicketsADT.h`**: Header file for `parkingTicketsADT`.
  
- **`htmlTable.c`**: File responsible for creating HTML files, inserting strings into tables, and closing these files.
  
- **`htmlTable.h`**: Header file for `htmlTable`.
  
- **`Makefile`**: Configuration file for compiling the project.

## Commands needed to compile

- **`make all`**: Compile the executables by running `make all` in the console. This will generate two executables:
  
  - **`parkingTicketsNYC`**: Executable for processing New York City parking tickets.
    
  - **`parkingTicketsCHI`**: Executable for processing Chicago parking tickets.

- **`make clean`**: Delete all generated files by running `make clean`.

## How to execute 

After compiling, each executable requires two parameters, which must be provided in the specified order:

1. **First parameter**: The path to the tickets file.
2. **Second parameter**: The path to the infractions file.

Use the city-specific program with its corresponding city database. To execute a program, use the following structure:

```bash
./<executableProgram> <ticketFile> <infractionsFile>


