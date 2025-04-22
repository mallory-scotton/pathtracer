TARGET				=	raytracer

CXX					=	g++
LDFLAGS				=	-lSDL2 -lGL -lGLEW -LExternal/OpenImageDenoise/lib -lOpenImageDenoise -ldl -lm -lpthread
INCLUDES			=	-I. -IRaytracer -IExternal -IExternal/ImGui -IExternal/GL -IExternal/OpenImageDenoise/include
CXXFLAGS			=	-std=c++20 -Wall -Wextra -DRAY_DEBUG
DFLAGS				=

FLAGS				=	$(LDFLAGS) $(INCLUDES) $(CXXFLAGS) $(DFLAGS)

SOURCES				=	$(shell find Raytracer External -type f -iname "*.cpp") \
						$(shell find Raytracer External -type f -iname "*.c") \

OBJECTS				=	$(SOURCES:.cpp=.o)

all: $(TARGET)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(FLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(FLAGS)

clean:
	find $(DIRECTORIES) -type f -iname "*.o" -delete
	find $(DIRECTORIES) -type f -iname "*.d" -delete

fclean: clean
	rm -rf $(TARGET)

re: fclean all

.PHONY: clean fclean re
