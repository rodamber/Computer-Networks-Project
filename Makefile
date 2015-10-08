all:
	(cd src/ECP;  make all; cp ECP stats.txt topics.txt ../..)
	(cd src/TES;  make all; cp TES *.txt *.pdf *.py     ../..)
	(cd src/User; make all; cp user *.py                ../..)

zip: clean
	zip proj_9.zip -r *

clean:
	rm -f proj_9.zip
	rm -f ECP stats.txt topics.txt
	rm -f TES startup.txt transactions.txt T*QF*.pdf T*QF*A.txt
	rm -f user
	rm -f *.py
	rm -rf __pycache__
	(cd src/ECP;  make cleand)
	(cd src/TES;  make clean)
	(cd src/User; make clean)
