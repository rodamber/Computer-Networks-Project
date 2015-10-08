EXEC=TES

all: $(EXEC)

run: all
	./$(EXEC)

$(EXEC):
	cp __main__.py $(EXEC)
	chmod +x $(EXEC)

clean:
	rm -f $(EXEC)
	rm -rf __pycache__
