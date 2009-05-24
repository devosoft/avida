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
TARGET		= hist_map

default:  $(TARGET)

### List Source Files in the Project #########################################
SRC		= hist_map.cc			../../tools/cFile.cc  \
		../../tools/cInitFile.cc	../../tools/cString.cc  \
		../../tools/cStringList.cc  ../../tools/cStringIterator.cc  \
		../../tools/cStringUtil.cc ../../tools/cRCObject.cc

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

