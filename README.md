# Project Authors

- Francisco Trivelloni (64141)

- Catalina Yelmini (64133)

- Lorenzo Pizzuto Beltrán (64123)

# List of source and header files, with a brief explanation of each one

- *parkingTickets.c*: Main source file containing the logic for processing parking tickets.
- *parkingTicketsADT.c*: Source file that implements the ADT (Abstract Data Type) for handling parking tickets.
- *parkingTicketsADT.h*: Header file for the parkingticketsADT.
- *htmlTable.c*: File that crates html files, introduces strings into tables, and closes said files.
- *htmlTable.h*: Header file for htmlTable.
- *Makefile*: Configuration file for compiling the project.

# Commands needed to compile the project

- *all*: To compile the executables, run the command "make all" in the console. This will generate two executables:
  - parkingTicketsNYC: Executable for processing New York City parking tickets.
  - parkingTicketsCHI: Executable for processing Chicago parking tickets.

- *clean*: To delete all generated files, run the command "make clean".

# How to execute the programs

- After compiling all needed programs, the executable needs two parameters which must be given in this specific order:
- *First parameter*: the path to the tickets file.
- *Second parameter*: the path to the infractions file.
- Use the city specific program with its corresponding city database. 
- To execute a program, use the following structure: ./*executableProgram* *ticketFile* *infractionsFile*
- The order of the files and the number of arguments must be as indicated above.

# Additional project information

- *Queries*: The generated query files (query1.csv, query2.csv, query3.csv) contain the results of the data processing.
- *HTML*: The generated HTML files (query1.html, query2.html, query3.html) provide a visualization of the query results.


