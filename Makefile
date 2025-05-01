TARGET				=	raytracer

CXX					=	g++
LDFLAGS				=	-lSDL2 \
						-lGL \
						-lGLEW \
						-LExternal/OpenImageDenoise/lib \
						-lOpenImageDenoise \
						-ldl \
						-lm \
						-lpthread \
						-lSDL2main \
						-lconfig++
INCLUDES			=	-I. \
						-IRaytracer \
						-IExternal \
						-IExternal/ImGui \
						-IExternal/GL \
						-IExternal/OpenImageDenoise/include
CXXFLAGS			=	-std=gnu++20 -DRAY_DEBUG
DFLAGS				=	-Wall -Wextra

FLAGS				=	$(LDFLAGS) $(INCLUDES) $(CXXFLAGS) $(DFLAGS) -rdynamic

PLUGIN_FLAGS		=	-fPIC -fno-gnu-unique

SOURCES				=	$(shell find Raytracer -type f -iname "*.cpp" \
							-not -path "Raytracer/Plugins/*") \
						$(shell find Raytracer -type f -iname "*.c" \
							-not -path "Raytracer/Plugins/*") \
						$(shell find External -type f -iname "*.cpp") \
						$(shell find External -type f -iname "*.c")

OBJECTS				=	$(patsubst %.cpp,%.o,$(filter %.cpp,$(SOURCES))) \
						$(patsubst %.c,%.o,$(filter %.c,$(SOURCES)))

PLUGIN_DIRS			=	$(shell find Raytracer/Plugins -mindepth 1 \
							-maxdepth 1 -type d)
PLUGIN_NAMES		=	$(notdir $(PLUGIN_DIRS))
PLUGIN_TARGETS		=	$(patsubst %,%.rplugin,$(PLUGIN_NAMES))

SCENE_NAME			?=	

all: $(TARGET) plugins

%.o: %.cpp
	$(CXX) -c $< -o $@ $(FLAGS)

%.o: %.c
	$(CXX) -c $< -o $@ $(FLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(FLAGS)

%.rplugin:
	mkdir -p Plugins
	@$(eval PLUGIN_DIR := Raytracer/Plugins/$(basename $@))
	@$(eval PLUGIN_SOURCES := \
		$(shell find $(PLUGIN_DIR) -type f -iname "*.cpp" -o -iname "*.c"))
	@$(eval PLUGIN_OBJECTS := \
		$(patsubst %.cpp,%.o,$(filter %.cpp,$(PLUGIN_SOURCES))) \
		$(patsubst %.c,%.o,$(filter %.c,$(PLUGIN_SOURCES))))
	@for src in $(PLUGIN_SOURCES); do \
		$(CXX) $(PLUGIN_FLAGS) -c $$src -o $${src%.*}.o $(FLAGS); \
	done
	$(CXX) -shared -o Plugins/$@ $(PLUGIN_OBJECTS) $(FLAGS)

plugins: $(PLUGIN_TARGETS)

clean:
	find -type f -iname "*.o" -delete
	find -type f -iname "*.d" -delete

fclean: clean
	rm -rf $(TARGET)
	rm -rf Plugins/

re: fclean all

run:
	LD_LIBRARY_PATH=External/OpenImageDenoise/lib/:$LD_LIBRARY_PATH \
		./raytracer $(SCENE_NAME)

run-ajax: SCENE_NAME += -s Assets/ajax.scene
run-ajax: run

.PHONY: clean fclean re
