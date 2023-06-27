all:
	g++ -std=c++11 -g -o test  ./src/backend/cpp/main.cpp -L/usr/lib -lppl -lglpk -L./src/backend/lib -lc2e2 -Wl,--rpath-link=.

clean: 
	rm ./src/backend/cpp/*.o ./src/backend/cpp/test
