
EXEC=TES

all: $(EXEC)

$(EXEC): *.py
	zip -r $(EXEC).zip *.py
	echo '#!/usr/bin/env python3' | cat - $(EXEC).zip > $(EXEC)
	rm $(EXEC).zip
	chmod +x $(EXEC)

clean:
	rm $(EXEC)
