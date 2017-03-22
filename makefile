main:main.o function.o structs.o
	g++ -o singel_cycle main.o function.o structs.o
  
main.o:main.cpp
	g++ -c main.cpp
  
function.o:function.cpp
	g++ -c function.cpp
  
structs.o:structs.cpp
	g++ -c structs.cpp
  
clean:
	rm -f main.o function.o structs.o