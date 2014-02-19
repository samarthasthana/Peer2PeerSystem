CC = g++
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .cc files here (source files, excluding header files)
SIM_CLS = client_p1.cpp
SIM_SER = server_p1.cpp 

# List corresponding compiled object files here (.o files)
SIM_CL = client
SIM_SR = server

#################################

# default rule
      

all: clean client server
	
	

client: $(SIM_OBJ)
	g++ -o $(SIM_CL) $(SIM_CLS)
	

server: $(SIM_OBJ)
	g++ -o $(SIM_SR) $(SIM_SER)	


# rule for making sim

sim: $(SIM_OBJ)
	$(CC) -o sim $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH SIM-----------"


# generic rule for converting any .cc file to any .o file
 
.cc.o:
	$(CC) $(CFLAGS)  -c $*.cc


# type "make clean" to remove all .o files plus the sim binary

clean:
	rm -f client
	rm -f server


# type "make clobber" to remove all .o files (leaves sim binary)

clobber:
	rm -f *.o


