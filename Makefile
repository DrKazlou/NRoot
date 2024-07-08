########################################################################

ROOTINCL  := $(shell root-config --incdir)
ROOTLIB  := $(shell root-config --libdir)


LIBS          := -L -lpthread -lutil -lusb-1.0 -L $(ROOTLIB) -lGeom -lRGL -lGed -lTreePlayer -lCore -lHist -lGraf -lGraf3d -lMathCore -lGpad -lTree -lRint -lRIO -lPostscript -lMatrix -lPhysics -lMinuit -lGui -lASImage -lASImageGui -pthread -lm -ldl -rdynamic -lstdc++
#CFLAGS        = -g -O2 -Wall -Wuninitialized -fno-strict-aliasing -I./include -I/usr/local/include -I $(ROOTINCL) -DLINUX -fPIC -lCAENDigitizer
#CXXFLAGS        = -std=c++17 -g -O2 -Wall -Wuninitialized -fno-strict-aliasing -I./include -I/usr/local/include -I $(ROOTINCL) -DLINUX -fPIC -lCAENDigitizer


#CXX           = g++

#FLAGS =  -fPIC -DLINUX -I. -I./include

#DEPLIBS	=	-lCAENN957 -lm 


CXX           = g++ -fPIC -DLINUX -O2 -I./include




.PHONY: all clean
	
all: clean NRoot
	
clean:
		@rm -rf NRoot *.o *.cxx *.so

guiDict.cxx : include/NRoot.h include/NFrame.h include/LinkDef.h
	@rootcling -f $@ $^
	$(info [-10%]  Dictionary)	

NRoot: NFunc.o  NFrame.o NRoot.o 
		$(info [70%]  Linking)
		@$(CXX) -o NRoot src/NRoot.o guiDict.cxx src/NFrame.o  src/NFunc.o $(LIBS) -L. -lCAENN957 `root-config --cflags --glibs`
		$(info [100%] Built target NRoot)
		
NFrame.o:  src/NFrame.c
		$(info [40%] Generating NFrame.o)
		@$(CXX) -o src/NFrame.o -c src/NFrame.c `root-config --cflags --glibs`
		
NFunc.o:  src/NFunc.c
		$(info [10%] Generating NFunc.o)
		@$(CXX) -o src/NFunc.o -c src/NFunc.c	`root-config --cflags --glibs`	
		
#DTReadout.o:  src/DTReadout.c
#		$(info [15%] Generating DTReadout.o)
#		@$(CXX) -o src/DTReadout.o -c src/DTReadout.c	`root-config --cflags --glibs`			

NRoot.o: src/NRoot.c guiDict.cxx
		$(info [30%] Generation NRoot.o)
		@$(CXX) -o src/NRoot.o -c src/NRoot.c `root-config --cflags --glibs`
		
