EXEC=user

all: $(EXEC)

run: all
	./$(EXEC) 12345

$(EXEC):
	cp __main__.py $(EXEC)
	chmod +x $(EXEC)

clean:
	rm -f $(EXEC)
	rm -rf __pycache__
