.SUFFIXES : .c .o

include ../../makefile.in

FLAGS ?= -DUNIX -O3 -g -Wall 

INCLUDES = -I../../INC -I../../EXTERNAL   -I ../COMMON

ifeq ($(INTEL), 1)
  OBJ_DIRSOL = evsl_pardiso.o
else
ifeq ($(DIRECTSOL), SUITESPARSE)
  OBJ_DIRSOL = evsl_suitesparse.o
  include ../../EXTERNAL/makefile_suitesparse.in
  LIB_DIRSOL = $(LIB_UMF)
else
  OBJ_DIRSOL = evsl_cxsparse.o
  LIB_DIRSOL = -L../../EXTERNAL/CXSparse/Lib -lcxsparse
  FLAGS += -DCXSPARSE=1
endif
endif

EXTRA =  ../COMMON/io.o ../COMMON/mmio.o ../COMMON/lapl.o
# Object files
OBJS_L  =  LapRLanN.o $(EXTRA) ../../EXTERNAL/$(OBJ_DIRSOL)
OBJS_MM  = MMRLanN.o $(EXTRA) ../../EXTERNAL/$(OBJ_DIRSOL)

LIB = -L../../ -levsl
LIB_EXT = $(LIB_DIRSOL) $(OMP) $(LIBLAPACK) $(LIB0)

ALLEXE = LapRLanN.ex MMRLanN.ex


# Rules
default: LapRLanN.ex

%.o : %.c
	$(CC) $(FLAGS) $(INCLUDES) -o $@ -c -x c $<

# Exe
## polynomial
LapRLanN.ex: $(OBJS_L)
	$(LINK) -o LapRLanN.ex $(OBJS_L) $(LIB)  $(LIB_EXT)


MMRLanN.ex: $(OBJS_MM)
	$(LINK) -o MMRLanN.ex $(OBJS_MM) $(LIB)  $(LIB_EXT)

all: $(ALLEXE)

clean:
	rm -f *.o *.ex

cleanall:
	rm -f *.o *.ex OUT/*
