CLANG_FORMAT ?= clang-format
SRC_FILES := $(wildcard src/*.c src/*.cpp src/*.h src/*.hpp)

.PHONY: format format-check

format:
	$(CLANG_FORMAT) -i $(SRC_FILES)

format-check:
	$(CLANG_FORMAT) --dry-run --Werror $(SRC_FILES)