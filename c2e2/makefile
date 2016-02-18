# first testing make file

#build all the sources here
#-include sourcemake.mk

#-include ../depmake.mk

USER_OBJS := 

LIBS := -lglpk

CXX=g++

RM := rm -rf

CPP_SRCS += \
./simuverif.tab.c \
./lex.yy.c \
./Annotation.cpp \
./RepPoint.cpp \
./CoverStack.cpp \
./Checker.cpp \
./InitialSet.cpp \
./main.cpp \
./Point.cpp \
./Polyhedron.cpp \
./Simulator.cpp \
./Vector.cpp \
./Visualizer.cpp \
./ReachTube.cpp \
./LinearSet.cpp \
./lex.yy.o \
#./y.tab.o \

OBJS += \
./Annotation.o \
./RepPoint.o \
./CoverStack.o \
./Checker.o \
./InitialSet.o \
./main.o \
./Point.o \
./Polyhedron.o \
./Simulator.o \
./Vector.o \
./Visualizer.o \
./ReachTube.o \
./LinearSet.o \
./lex.yy.o\

CPP_DEPS += \
./y.tab.d \
./lex.yy.d \
./Annotation.d \
./RepPoint.d \
./CoverStack.d \
./Checker.d \
./InitialSet.d \
./main.d \
./Point.d \
./Polyhedron.d \
./Simulator.d \
./Vector.d \
./Visualizer.d \
./ReachTube.d \
./LinearSet.d \

MISC := ./stack.hh

all: C2E2
C2E2: $(OBJS) $(USER_OBJS) $(MISC)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CXX)  -o"C2E2" $(OBJS) $(USER_OBJS) $(LIBS) -I/usr/include/python2.7/ -lpython2.7 -lstdc++
	@echo 'Finished building target: $@'
	@echo ' '


Annotation.o: ./Annotation.cpp ./Annotation.h
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"Annotation.d" -MT"Annotation.d" -o"Annotation.o" "$<" 
	@echo 'Finished building: $<'
	@echo ' '

Checker.o: ./Checker.cpp ./Checker.h
	$(CXX) -lglpk -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"Checker.d" -MT"Checker.d" -o"Checker.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

InitialSet.o: ./InitialSet.cpp ./InitialSet.h
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"InitialSet.d" -MT"InitialSet.d" -o"InitialSet.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

RepPoint.o: ./RepPoint.cpp ./RepPoint.o
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"RepPoint.d" -MT"RepPoint.d" -o"RepPoint.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CoverStack.o: ./CoverStack.cpp ./CoverStack.o
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"CoverStack.d" -MT"CoverStack.d" -o"CoverStack.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.o: ./main.cpp ./lex.yy.cc ./simuverif.tab.c ./simuverifScanner.h ./simuverifParser.h ./stack.hh
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"main.d" -MT"main.d" -o"main.o" "$<" -I/usr/include/python2.7/ -lpython2.7 -lstdc++
	@echo 'Finished building: $<'
	@echo ' '

Point.o: ./Point.cpp ./Point.h
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"Point.d" -MT"Point.d" -o"Point.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Polyhedron.o: ./Polyhedron.cpp ./Polyhedron.h
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"Polyhedron.d" -MT"Polyhedron.d" -o"Polyhedron.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Simulator.o: ./Simulator.cpp ./Simulator.h
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"Simulator.d" -MT"Simulator.d" -o"Simulator.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Vector.o: ./Vector.cpp ./Vector.h
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"Vector.d" -MT"Vector.d" -o"Vector.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Visualizer.o: ./Visualizer.cpp ./Visualizer.h
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"Visualizer.d" -MT"Visualizer.d" -o"Visualizer.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

ReachTube.o: ./ReachTube.cpp ./ReachTube.h
	$(CXX) -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"ReachTube.d" -MT"ReachTube.d" -o"ReachTube.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

LinearSet.o: ./LinearSet.cpp ./LinearSet.h
	$(CXX) -lglpk -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"LinearSet.d" -MT"LinearSet.d" -o"LinearSet.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Each subdirectory must supply rules for building sources it contributes
lex.yy.o: ./lex.yy.cc ./simuverif.tab.h
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"lex.yy.d" -MT"lex.yy.d" -o"lex.yy.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

#y.tab.o: ./y.tab.c
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C++ Compiler'
#	$(CXX) -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"y.tab.d" -MT"y.tab.d" -o"y.tab.o" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '

./stack.hh:
./simuverif.tab.h:
./simuverif.tab.c: ./simuverif.y
	bison -o ./simuverif.tab.h -o ./simuverif.tab.c ./simuverif.y

./lex.yy.cc: ./simuverif.l
	flex -o ./lex.yy.cc ./simuverif.l

clean:
	-$(RM) $(OBJS)$(CPP_DEPS)

