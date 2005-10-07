### Compile and Link Command & Options #######################################

CXX	= g++
FLAGS	= -Wall -pedantic -g -ffast-math -DNDEBUG #-Winline

# Include directories [May need to adjust.  See your system administrator]
#INCDIR	=  -I/usr/include/g++/
INCDIR   = 

# Libraries to link
LDLIBS   = 

### Shouldn't need to change below ###########################################

# Compile flags
CXXFLAGS= $(FLAGS)

# Flags durring linking
LDFLAGS	= $(CXXFLAGS)


### Different Builds #########################################################

# executable make
TARGET		= hist_map2

default:  $(TARGET)

### List Source Files in the Project #########################################
SRC		= hist_map2.cc			../../tools/cFile.cc  \
		../../tools/init_file.cc	../../tools/string.cc  \
		../../tools/string_list.cc  ../../tools/string_iterator.cc  \

### List Object Files (for each source file there is a object file) ###########
OBJ	 	= $(SRC:.cc=.o)


### General Make Commands #####################################################

CCC		= $(CXX)		# Compatibility

$(TARGET):	$(OBJ)
		### LINKING ###
		$(CXX) $(LDFLAGS) $(INCDIR) $(OBJ)  $(LDLIBS) -o $(TARGET)
		### FINISHED MAKING "$(TARGET)" ###

.cc.o:
	$(CXX) $(CXXFLAGS) $(INCDIR) -c $< -o $*.o

clean:
		### REMOVING OBJECT FILES AND EXECUTEABLE ###
		rm -f core $(TARGET) $(OBJ)

new:		clean $(TARGET)

install:
	configure
	$(MAKE) new


### Dependency Generation #####################################################

#depend: $(SRC)
#	makedepend -- $(CXXFLAGS) -- $(INCDIR) $(SRC)

depend:  local_depend

local_depend: $(SRC)
	makedepend -Y -- $(CXXFLAGS) -- $(INCDIR) $(SRC)

### Release Packageing ########################################################
# DO NOT DELETE

hist_map2.o: ../../tools/tMatrix.hh ../../tools/tArray.hh
hist_map2.o: ../../tools/string.hh ../../tools/cFile.h
hist_map2.o: ../../tools/init_file.hh ../../tools/string_list.hh
hist_map2.o: ../../tools/tList.hh
../../tools/file.o: ../../tools/cFile.h ../../tools/string.hh
../../tools/file.o: ../../tools/init_file.hh ../../tools/string_list.hh
../../tools/file.o: ../../tools/tList.hh ../../tools/tArray.hh
../../tools/file.o: ../../tools/cGenesis.h ../../tools/string_iterator.hh
../../tools/file.o: ../../tools/tools.hh ../../tools/random.hh
../../tools/init_file.o: ../../tools/init_file.hh ../../tools/string_list.hh
../../tools/init_file.o: ../../tools/tList.hh ../../tools/tArray.hh
../../tools/init_file.o: ../../tools/string_iterator.hh
../../tools/string.o: ../../tools/string.hh
