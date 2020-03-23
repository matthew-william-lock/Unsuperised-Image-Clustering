CC=g++
LIBS= -std=c++11

clusterer: clusterer.o 
	$(CC) -o clusterer clusterer.o $(LIBS)

clusterer.o: clusterer.cpp
	$(CC) -c -o clusterer.o clusterer.cpp  $(LIBS)	

clean:
	rm -f ./*.o
	rm clusterer