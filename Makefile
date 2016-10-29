TARGET	= glfw3
SOURCES	= $(wildcard *.cpp)
HEADERS	= $(wildcard *.h)
OBJECTS	= $(patsubst %.cpp,%.o,$(SOURCES))
CXXFLAGS	= -g -Wall -std=c++11 -Iinclude
LDLIBS	= -Llib -lglfw3_linux -lGLEW_linux -lGL -lXrandr -lXinerama -lXcursor \
	-lXi -lXxf86vm -lX11 -lpthread -lrt -lm -ldl

.PHONY: clean

$(TARGET): $(OBJECTS)
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(TARGET).dep: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -MM $(SOURCES) > $(TARGET).dep

clean:
	-$(RM) $(TARGET) *.o *~ .*~ a.out core

-include $(TARGET).dep
