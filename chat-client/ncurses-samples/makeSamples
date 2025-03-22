#
# this makefile will compile and and all source
# found in the ~/develop/ncurses-samples/src directory and place the object files
# into the ~/develop/ncurses-samples/obj directory.
#
# After all is compiled, the linker is invoked to create the final
# executable (which will be stored in the ~/develop/ncurses-samples/bin directory
# 
# =======================================================
#                  compile sample code
# =======================================================
# FINAL BINARY Target
all : ./bin/nc-01 ./bin/nc-02 ./bin/nc-03

./bin/nc-01 : ./obj/nc-01.o
	cc ./obj/nc-01.o -lncurses -o ./bin/nc-01

./bin/nc-02 : ./obj/nc-02.o
	cc ./obj/nc-02.o -lncurses -o ./bin/nc-02

./bin/nc-03 : ./obj/nc-03.o
	cc ./obj/nc-03.o -lncurses -o ./bin/nc-03

#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/nc-01.o : ./src/ncurses-01.c
	cc -c ./src/ncurses-01.c -o ./obj/nc-01.o

./obj/nc-02.o : ./src/ncurses-02.c
	cc -c ./src/ncurses-02.c -o ./obj/nc-02.o

./obj/nc-03.o : ./src/ncurses-03.c
	cc -c ./src/ncurses-03.c -o ./obj/nc-03.o
#
# =======================================================
# Other targets
# =======================================================                     
clean:
	rm -f ./bin/nc-01
	rm -f ./bin/nc-02
	rm -f ./bin/nc-03
	rm -f ./obj/nc-01.o
	rm -f ./obj/nc-02.o
	rm -f ./obj/nc-03.o
	rm -f ./src/ncurses-01.c~
	rm -f ./src/ncurses-02.c~
	rm -f ./src/ncurses-03.c~



