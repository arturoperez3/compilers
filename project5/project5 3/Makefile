# Makefile for Tiger compiler

parsetest: parsetest.o parse.o y.tab.o absyn.o symbol.o table.o lex.yy.o errormsg.o util.o typecheck.o env.o types.o
	g++ -std=c++11 -w -g -o parsetest parsetest.o parse.o y.tab.o absyn.o symbol.o table.o lex.yy.o errormsg.o util.o typecheck.o env.o types.o

parsetest.o: parsetest.cpp parse.h symbol.h absyn.h table.h errormsg.h util.h
	g++ -std=c++11 -w -g -c parsetest.cpp

parse.o: parse.cpp parse.h symbol.h absyn.h errormsg.h
	g++ -std=c++11 -w -g -c parse.cpp

y.tab.o: y.tab.c symbol.h absyn.h errormsg.h util.h
	g++ -std=c++11 -w -g -c y.tab.c

y.tab.c: tig_parse.y util.h errormsg.h symbol.h absyn.h
	bison -dvy tig_parse.y

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

absyn.o: absyn.cpp absyn.h
	g++ -std=c++11 -w -g -c absyn.cpp

symbol.o: symbol.cpp symbol.h table.h
	g++ -std=c++11 -w -g -c symbol.cpp

table.o: table.cpp table.h
	g++ -std=c++11 -fpermissive -w -g -c table.cpp

types.o: types.cpp types.h util.h symbol.h
	g++ -std=c++11 -w -g -c types.cpp

typecheck.o: typecheck.cpp typecheck.h env.h absyn.h types.h symbol.h
	g++ -std=c++11 -w -g -c typecheck.cpp

env.o: env.cpp env.h types.h symbol.h util.h
	g++ -std=c++11 -w -g -c env.cpp

errormsg.o: errormsg.cpp errormsg.h util.h
	g++ -std=c++11 -w -g -c errormsg.cpp

lex.yy.o: lex.yy.c y.tab.h absyn.h errormsg.h util.h
	g++ -std=c++11 -w -g -c lex.yy.c

#lex.yy.c: tig_scan.l
#	flex tig_scan.l

util.o: util.cpp util.h
	g++ -std=c++11 -w -g -c util.cpp

clean:
	rm -f a.out *.o y.tab.c y.tab.h y.tab.o y.output *~
