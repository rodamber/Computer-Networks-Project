EXEC=TES

all: $(EXEC)

run: all
	./$(EXEC)

$(EXEC): *.py
	zip -r $(EXEC).zip $^
	echo '#!/usr/bin/env python3' | cat - $(EXEC).zip > $(EXEC)
	rm $(EXEC).zip
	chmod +x $(EXEC)

clean:
	rm -f $(EXEC)
