OBJ = main.o
INC = -I "./"

PhysicsRayTracer: $(OBJ)
	g++ $(OBJ) -o PhysicsRayTracer
	rm -f $(OBJ)

main.o:
	g++ -c main.cpp $(INC)

clean:
	rm -f $(OBJ) PhysicsRayTracer