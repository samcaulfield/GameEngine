ifeq ($(OS),Windows_NT)
LIBS=-lopengl32 -lglew32 -lglfw3 -lm
else
LIBS=-lGL -lglfw -lGLEW -lm
endif

all: *.c
	gcc *.c $(LIBS) -o Demo