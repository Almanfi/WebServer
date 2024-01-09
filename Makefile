TARGET := webserver

SRCS = $(shell find ./src -type f -name "*.cpp")
HEADS = $(shell find ./include -type f -name "*.hpp")

OBJS = $(SRCS:.cpp=.o)

CXX = c++
INCLUDES = -I./include -I./include/config -I./include/http -I./include/utils
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 $(INCLUDES)
LDFLAGS = 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS)

run: all
	@./$(TARGET)

clean :
	$(RM) $(OBJS)

fclean : clean
	$(RM) $(TARGET)

re : fclean all

.PHONY : all clean fclean re