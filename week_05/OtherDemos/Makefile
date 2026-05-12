
all: false_sharing parallel

false_sharing: 
	g++ -std=c++20 -O3 false_sharing.cpp -o false_sharing

parallel:
	g++ -std=c++20 -O3  par.cpp -o parallel_algos -ltbb 

clean:
	rm false_sharing
	rm parallel_algos


