
parsetest: parsetest.o y.tab.o lex.yy.o errormsg.o util.o
	g++ -std=c++11 -o parsetest parsetest.o y.tab.o lex.yy.o errormsg.o util.o

parsetest.o: parsetest.cpp errormsg.h util.h
	g++ -std=c++11 -g -w -c parsetest.cpp

y.tab.o: y.tab.c
	g++ -std=c++11 -g -w -c y.tab.c

y.tab.c: tig_parse.y
	bison -dvy tig_parse.y

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

errormsg.o: errormsg.cpp errormsg.h util.h
	g++ -std=c++11 -g -w -c errormsg.cpp

lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h
	g++ -std=c++11 -g -w -c lex.yy.c

#lex.yy.c: tig_scan.l
#	flex tig_scan.l

util.o: util.cpp util.h
	g++ -std=c++11 -g -w -c util.cpp

clean: 
	rm -f a.out util.o parsetest.o lex.yy.o errormsg.o y.tab.c y.tab.h y.tab.o
