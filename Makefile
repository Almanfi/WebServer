TARGET := webserver

SRCS = ./src/main.cpp 		\
./src/config/location.cpp	\
./src/config/exception.cpp	\
./src/config/config.cpp		\
./src/config/server.cpp		\
./src/config/parser.cpp		\
./src/http/header.cpp		\
./src/http/request.cpp		\
./src/response/headers.cpp	\
./src/response/util.cpp		\
./src/response/response.cpp	\
./src/response/cgi.cpp		\
./src/socket/client.cpp		\
./src/socket/epoll.cpp		\
./src/socket/serverSocket.cpp \
./src/utils/uniqFile.cpp	\
./src/utils/uuid.cpp		\
./src/utils/sbuffer.cpp		\

HEADS = ./include/config/server.hpp \
./include/config/parser.hpp			\
./include/config/exception.hpp		\
./include/config/location.hpp		\
./include/config/config.hpp			\
./include/http/request.hpp			\
./include/http/header.hpp			\
./include/response/utils.hpp		\
./include/response/response.hpp		\
./include/response/headers.hpp		\
./include/socket/epoll.hpp			\
./include/socket/serverSocket.hpp	\
./include/socket/client.hpp			\
./include/socket/socket.hpp			\
./include/utils/uuid.hpp			\
./include/utils/uniqFile.hpp		\
./include/utils/sbuffer.hpp			\
./include/definition.hpp			\

OBJS = $(SRCS:.cpp=.o)

CXX = c++
INCLUDES = -I./include -I./include/config -I./include/http -I./include/utils -I./include/socket -I./include/response
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 $(INCLUDES) -fsanitize=address -g
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
	$(RM) -r tmp/*

re : fclean all

.PHONY : all clean fclean re