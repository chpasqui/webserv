NAME		=	webserv
DIR_SRC		=	./srcs
DIR_BUILD	=	./build
DIR_INCLUDE =	./include
CXX			=	c++
CXXFLAGS	=	-std=c++98 -Wall -Werror -Wextra

ifdef DEBUG
    CXXFLAGS += -DDEBUG -g
endif

LDINCLUDE	=	-I$(DIR_INCLUDE)

SRCS		=	main.cpp Server.cpp Client.cpp .cpp \
				Request.cpp requestParsingLine.cpp requestParsingHeader.cpp requestParsingBody.cpp debug.cpp \
				requestUtils.cpp 				serverEpoll.cpp serverLoop.cpp \
				listeningSockets.cpp serverUtils.cpp \
				serverHandler.cpp Cgi.cpp cgiHandler.cpp \
				Response.cpp responseBuild.cpp responseHandlers.cpp responseFormat.cpp \
				configuration/ServerConfig.cpp configuration/ParseConfig.cpp configuration/Http.cpp configuration/Location.cpp configuration/AConfig.cpp

SRCS_NAME	=	$(basename $(SRCS))
OBJS		=	$(addsuffix .o, $(addprefix $(DIR_BUILD)/, $(SRCS_NAME)))

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(OBJS) $(CXXFLAGS) -o $@

$(DIR_BUILD)/%.o: $(DIR_SRC)/%.cpp
	@mkdir -p $(dir $@) 
	@$(CXX) $(CXXFLAGS) $(LDINCLUDE) -c $< -o $@

clean:
	@rm -rf $(OBJS)
	@rm -rf $(DIR_BUILD)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
