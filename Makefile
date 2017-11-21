CC=	cc
GCC=	gcc
CXX=	g++

obj=	Read_Elf.o simulator.o

simulator: $(obj)
	$(CXX) -o simulator $(obj)
Read_Elf.o:
	$(CXX) -c Read_Elf.cpp
simulator.o:
	$(CXX) -c simulator.cpp

clean:
	rm -f simulator
	rm -f *.txt
	rm -f *.o *.obj
