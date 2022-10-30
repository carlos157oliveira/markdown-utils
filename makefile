BUILD_DIR = ./build
SRC_DIR = ./src
CCFLAGS = -Wall -O

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

md: $(BUILD_DIR)/lex.yy.o $(OBJS)
	gcc $(CCFLAGS) -o $(BUILD_DIR)/$@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	gcc -c $(CCFLAGS) -o $@ $^

$(BUILD_DIR)/%.yy.o: $(BUILD_DIR)/%.yy.c
	gcc -c $(CCFLAGS) -o $@ $<

$(BUILD_DIR)/lex.yy.c: $(SRC_DIR)/scanner.l
	mkdir -p $(BUILD_DIR)
	flex -o $@ --header-file=$(@:.c=.h) $<

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

.PHONY: test
test:
	@echo Testing...
	@./build/md fmt ./test-data/input.md | diff --color=always - ./test-data/expected.md
	@echo Test Successful.
