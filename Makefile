TARGET				=	raytracer

CXX					=	g++
LDFLAGS				=	-lSDL2 -lGL -lGLEW -LExternal/OpenImageDenoise/lib -lOpenImageDenoise -ldl -lm -lpthread -lSDL2main -lconfig++
INCLUDES			=	-I. -IRaytracer -IExternal -IExternal/ImGui -IExternal/GL -IExternal/OpenImageDenoise/include
CXXFLAGS			=	-std=gnu++20 -DRAY_DEBUG
DFLAGS				=	-Wall -Wextra

FLAGS				=	$(LDFLAGS) $(INCLUDES) $(CXXFLAGS) $(DFLAGS)

SOURCES				=	$(shell find Raytracer External -type f -iname "*.cpp") \
						$(shell find Raytracer External -type f -iname "*.c") \

OBJECTS				=	$(patsubst %.cpp,%.o,$(filter %.cpp,$(SOURCES))) \
						$(patsubst %.c,%.o,$(filter %.c,$(SOURCES)))

SCENE_NAME			?=	

all: $(TARGET)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(FLAGS)

%.o: %.c
	$(CXX) -c $< -o $@ $(FLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(FLAGS)

clean:
	find -type f -iname "*.o" -delete
	find -type f -iname "*.d" -delete

fclean: clean
	rm -rf $(TARGET)

re: fclean all

run:
	LD_LIBRARY_PATH=External/OpenImageDenoise/lib/:$LD_LIBRARY_PATH ./raytracer $(SCENE_NAME)

run-ajax: SCENE_NAME += -s Assets/ajax.scene
run-ajax: run

.PHONY: clean fclean re
