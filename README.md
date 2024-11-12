# Final Project - Imperative Programming

## Project Authors
- **Catalina Yelmini** (64133)
- **Francisco Trivelloni** (64141)
- **Lorenzo Pizzuto Beltrán** (64123)

## List of Source and Header Files

- **`parkingTickets.c`**: Main source file containing the logic for processing parking tickets.
- **`parkingTicketsADT.c`**: Source file that implements the ADT (Abstract Data Type) for handling parking tickets.
- **`parkingTicketsADT.h`**: Header file for the `parkingTicketsADT`.
- **`htmlTable.c`**: File responsible for creating HTML files, inserting strings into tables, and closing these files.
- **`htmlTable.h`**: Header file for `htmlTable`.
- **`Makefile`**: Configuration file for compiling the project.

## Commands Needed to Compile the Project

- **`make all`**: Compile the executables by running `make all` in the console. This will generate two executables:
  - **`parkingTicketsNYC`**: Executable for processing New York City parking tickets.
  - **`parkingTicketsCHI`**: Executable for processing Chicago parking tickets.

- **`make clean`**: Delete all generated files by running `make clean`.

## How to Execute the Programs

After compiling, each executable requires two parameters, which must be provided in the specified order:

1. **First parameter**: The path to the tickets file.
2. **Second parameter**: The path to the infractions file.

Use the city-specific program with its corresponding city database. To execute a program, use the following structure:

```bash
./<executableProgram> <ticketFile> <infractionsFile>


