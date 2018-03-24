parsetest: parsetest.o parse.o y.tab.o absyn.o symbol.o table.o lex.yy.o errormsg.o util.o
	g++ -std=c++11 -o parsetest parsetest.o parse.o y.tab.o absyn.o symbol.o table.o lex.yy.o errormsg.o util.o

parsetest.o: parsetest.cpp parse.h symbol.h absyn.h table.h errormsg.h util.h
	g++ -std=c++11 -w -c -g parsetest.cpp

parse.o: parse.cpp parse.h symbol.h absyn.h errormsg.h
	g++ -std=c++11 -w -c -g parse.cpp

y.tab.o: y.tab.c symbol.h absyn.h errormsg.h util.h
	g++ -std=c++11 -w -c -g y.tab.c

y.tab.c: tig_parse.y util.h errormsg.h symbol.h absyn.h
	bison -dvy tig_parse.y

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

absyn.o: absyn.cpp absyn.h
	g++ -std=c++11 -w -c -g absyn.cpp

symbol.o: symbol.cpp symbol.h table.h
	g++ -std=c++11 -w -c -g symbol.cpp

table.o: table.cpp table.h
	g++ -std=c++11 -fpermissive -w -c -g table.cpp

errormsg.o: errormsg.cpp errormsg.h util.h
	g++ -std=c++11 -w -c -g errormsg.cpp

lex.yy.o: lex.yy.c y.tab.h absyn.h errormsg.h util.h
	g++ -std=c++11 -w -c -g lex.yy.c

#lex.yy.c: tig_scan.l
#	flex tig_scan.l

util.o: util.cpp util.h
	g++ -std=c++11 -w -c -g util.cpp

clean: 
	rm -f a.out util.o parsetest.o parse.o absyn.o symbol.o table.o lex.yy.o errormsg.o y.tab.c y.tab.h y.tab.o y.output
