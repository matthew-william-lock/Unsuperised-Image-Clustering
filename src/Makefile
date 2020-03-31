CC=g++
LIBS= -std=c++11 -g

clusterer: clusterer.o Cluster.o PPM.o
	$(CC) -o clusterer clusterer.o Cluster.o PPM.o $(LIBS)

clusterer.o: clusterer.cpp
	$(CC) -c -o clusterer.o clusterer.cpp $(LIBS)	

Cluster.o: Cluster.cpp Cluster.h 
	$(CC) -c -o Cluster.o Cluster.cpp $(LIBS)
	
PPM.o: PPM.cpp PPM.h 
	$(CC) -c -o PPM.o PPM.cpp $(LIBS)	

clean:
	rm -f ./*.o
	rm clusterer