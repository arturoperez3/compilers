lextest: driver.o lex.yy.o errormsg.o util.o
	g++ -std=c++11 -g -o lextest driver.o lex.yy.o errormsg.o util.o

driver.o: driver.cpp tokens.h errormsg.h util.h
	g++ -std=c++11 -w -g -c driver.cpp

errormsg.o: errormsg.cpp errormsg.h util.h
	g++ -std=c++11 -w -g -c errormsg.cpp

lex.yy.o: lex.yy.c tokens.h errormsg.h util.h
	g++ -std=c++11 -w -g -c lex.yy.c

lex.yy.c: tig_scan.l
	flex tig_scan.l

util.o: util.cpp util.h
	g++ -std=c++11 -w -g -c util.cpp

clean: 
	rm -f a.out util.o driver.o lex.yy.o lex.yy.c errormsg.o
