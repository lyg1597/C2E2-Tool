test: ./backend/cpp/annotation.o ./backend/cpp/point.o ./backend/cpp/initial-set.o ./backend/cpp/rep-point.o ./backend/cpp/cover-stack.o ./backend/cpp/linear-set.o ./backend/cpp/reach-tube.o ./backend/cpp/checker.o ./backend/cpp/simulator.o ./backend/cpp/model.o ./backend/cpp/main.o
	g++ -std=c++11 ./backend/cpp/annotation.o ./backend/cpp/point.o ./backend/cpp/initial-set.o ./backend/cpp/rep-point.o ./backend/cpp/cover-stack.o ./backend/cpp/linear-set.o ./backend/cpp/reach-tube.o ./backend/cpp/checker.o ./backend/cpp/simulator.o ./backend/cpp/model.o ./backend/cpp/main.o -g -I /usr/include/python3.5m -lpython3.5m -lppl -lglpk -o test -lgmp -ldl

#
annotation.o: ./backend/cpp/annotation.cpp ./backend/cpp/annotation.hpp
	g++ -std=c++11 -c ./backend/cpp/annotation.cpp -g

#
point.o: ./backend/cpp/point.cpp ./backend/cpp/point.hpp
	g++ -std=c++11 -c ./backend/cpp/point.cpp -g

#
initial-set.o: ./backend/cpp/initial-set.cpp ./backend/cpp/initial-set.hpp
	g++ -std=c++11 -c ./backend/cpp/initial-set.cpp -g

#
rep-point.o: ./backend/cpp/rep-point.cpp ./backend/cpp/rep-point.hpp
	g++ -std=c++11 -c ./backend/cpp/rep-point.cpp -g

#
cover-stack.o: ./backend/cpp/cover-stack.cpp ./backend/cpp/cover-stack.hpp
	g++ -std=c++11 -c ./backend/cpp/cover-stack.cpp -g

#
linear-set.o: ./backend/cpp/linear-set.cpp ./backend/cpp/linear-set.hpp
	g++ -std=c++11 -c ./backend/cpp/linear-set.cpp -g

#
reach-tube.o: ./backend/cpp/reach-tube.cpp ./backend/cpp/reach-tube.hpp
	g++ -std=c++11 -lppl -c ./backend/cpp/reach-tube.cpp -g 

#
checker.o: ./backend/cpp/checker.cpp ./backend/cpp/checker.hpp
	g++ -std=c++11 -c ./backend/cpp/checker.cpp -g

#
simulator.o: ./backend/cpp/simulator.cpp ./backend/cpp/simulator.hpp ./backend/cpp/aliases.hpp
	g++ -std=c++11 -I /usr/include/python3.5m -lglpk -lpython3.5m -c ./backend/cpp/simulator.cpp -g 

#
model.o: ./backend/cpp/model.cpp ./backend/cpp/model.hpp
	g++ -std=c++11 -I /usr/include/python3.5m -lpython3.5m -c ./backend/cpp/model.cpp -g

#
main.o: ./backend/cpp/main.cpp
	g++ -std=c++11 -lppl -lglpk -c ./backend/cpp/main.cpp -g

clean: 
	rm ./backend/cpp/*.o test