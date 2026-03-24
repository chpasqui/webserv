NAME		= webserv
CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98 -Iinclude

BUILD_DIR	= build/
SRCS_DIR	= srcs/
SRCS_FILES	= main.cpp server.cpp setupListenSocket.cpp
OBJS		= $(addprefix $(BUILD_DIR), $(SRCS_FILES:.cpp=.o))

all: $(BUILD_DIR) $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(BUILD_DIR) :
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)%.o: $(SRCS_DIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re