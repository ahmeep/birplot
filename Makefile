TARGET := birplot
BUILD_DIR := ./build

SRCS := utils/text.c utils/texture.c utils/shader.c utils/data.c utils/file.c birplot.c
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INCLUDE = . ./vendor

CFLAGS := $(shell pkg-config --cflags cglm glfw3) $(addprefix -I,$(INCLUDE)) -g -Wall -Wextra
LDFLAGS := $(shell pkg-config --libs cglm glfw3) -lm

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
