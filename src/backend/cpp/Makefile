test: annotation.o point.o initial-set.o rep-point.o cover-stack.o linear-set.o reach-tube.o checker.o simulator.o model.o main.o
	g++ -std=c++11 annotation.o point.o initial-set.o rep-point.o cover-stack.o linear-set.o reach-tube.o checker.o simulator.o model.o main.o -g -I /usr/include/python3.5m -lpython3.5m -lppl -lglpk -o test -lgmp -ldl

#
annotation.o: annotation.cpp annotation.hpp
	g++ -std=c++11 -c annotation.cpp -g

#
point.o: point.cpp point.hpp
	g++ -std=c++11 -c point.cpp -g

#
initial-set.o: initial-set.cpp initial-set.hpp
	g++ -std=c++11 -c initial-set.cpp -g

#
rep-point.o: rep-point.cpp rep-point.hpp
	g++ -std=c++11 -c rep-point.cpp -g

#
cover-stack.o: cover-stack.cpp cover-stack.hpp
	g++ -std=c++11 -c cover-stack.cpp -g

#
linear-set.o: linear-set.cpp linear-set.hpp
	g++ -std=c++11 -c linear-set.cpp -g

#
reach-tube.o: reach-tube.cpp reach-tube.hpp
	g++ -std=c++11 -lppl -c reach-tube.cpp -g 

#
checker.o: checker.cpp checker.hpp
	g++ -std=c++11 -c checker.cpp -g

#
simulator.o: simulator.cpp simulator.hpp aliases.hpp
	g++ -std=c++11 -I /usr/include/python3.5m -lglpk -lpython3.5m -c simulator.cpp -g 

#
model.o: model.cpp model.hpp
	g++ -std=c++11 -I /usr/include/python3.5m -lpython3.5m -c model.cpp -g

#
main.o: main.cpp
	g++ -std=c++11 -lppl -lglpk -c main.cpp -g

clean: 
	rm *.o test