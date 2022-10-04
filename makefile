BUILD_DIR = ./build
SRC_DIR = ./src
CCFLAGS = -Wall -O -o

md: $(SRC_DIR)/main.c lex.yy.o md-fmt.o md-toc.o
	gcc $(CCFLAGS) $(BUILD_DIR)/md $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/md-fmt.o $(BUILD_DIR)/md-toc.o $(SRC_DIR)/main.c

md-toc.o: $(SRC_DIR)/md-toc.c
	gcc -c $(CCFLAGS) $(BUILD_DIR)/md-toc.o $(SRC_DIR)/md-toc.c

md-fmt.o: $(SRC_DIR)/md-fmt.c
	gcc -c $(CCFLAGS) $(BUILD_DIR)/md-fmt.o $(SRC_DIR)/md-fmt.c

lex.yy.o: lex.yy.c
	gcc -c -ll $(CCFLAGS) $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/lex.yy.c

lex.yy.c: $(SRC_DIR)/scanner.l
	mkdir -p $(BUILD_DIR)
	flex -o $(BUILD_DIR)/lex.yy.c --header-file=$(BUILD_DIR)/lex.yy.h $(SRC_DIR)/scanner.l

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
