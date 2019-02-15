##
## LINUX barebone makefile for c++ examples : good for all platforms
##
## Simply run make or make clean
##
## Intend this makefile only as a "batch examples updater" after library modification.
##

Libs     := -lpthread -L/opt/xdaq/lib -ldim
Inc	 := -I/opt/xdaq/include/dim 

#Libs     := -L/cmsnfshome0/nfshome0/kaminsky/dim/dim_v19r7/linux -ldim
#Inc	 := -I/cmsnfshome0/nfshome0/kaminsky/dim/dim_v19r7/dim  

CXX      := g++
CC       := gcc
# CXXFLAGS :=-O3
CFLAGS   :=

.PHONY: all clean

all : sdmon1.cpp
			$(CXX) $(CXXFLAGS) $(Inc) -o sdmon1 sdmon1.cpp  $(Libs)

#all: sdmon1
# 	$(CXX) $(CXXFLAGS) -o t1 t1.cpp $(Wrapper) $(Libs)
# 	$(CXX) $(CXXFLAGS) -o server ../server.cpp ../$(Wrapper) $(Libs)
# 	$(CXX) $(CXXFLAGS) -o srv_resourceless ../srv_resourceless.cpp ../$(Wrapper) $(Libs)
# 	$(CXX) $(CXXFLAGS) -o apartner ../apartner.cpp ../$(Wrapper) $(Libs)
# 	$(CXX) $(CXXFLAGS) -o ppartner ../ppartner.cpp ../$(Wrapper) $(Libs)

clean:
	$(RM) sdmon1.o sdmon1
