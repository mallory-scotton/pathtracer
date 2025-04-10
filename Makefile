TARGET				=	raytracer
DIRECTORIES			=	Raytracer External/ImGui

CXX					=	g++
LDFLAGS				=	-lsfml-graphics -lsfml-window -lsfml-system -lGL -lGLEW
INCLUDES			=	$(addprefix -I, $(DIRECTORIES))
CXXFLAGS			=	-std=c++20 -Wall -Wextra -DRAY_DEBUG
DFLAGS				=	

FLAGS				=	$(LDFLAGS) $(INCLUDES) $(CXXFLAGS) $(DFLAGS)

EXCLUDE_IMGUI		=	$(findstring -DRAY_NO_IMGUI, $(DFLAGS))

ifneq ($(EXCLUDE_IMGUI),)
	SOURCES			=	$(shell find Raytracer -type f -iname "*.cpp")
else
	SOURCES			=	$(shell find $(DIRECTORIES) -type f -iname "*.cpp")
endif

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
