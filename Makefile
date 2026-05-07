NAME        =   webserv
DIR_SRC     =   ./srcs
DIR_BUILD   =   ./build
DIR_INCLUDE =   ./include
CXX         =   c++
CXXFLAGS    =   -std=c++98 -Wall -Werror -Wextra -g

ifdef DEBUG
	CXXFLAGS += -DDEBUG -g
endif
LDINCLUDE   =   -I$(DIR_INCLUDE)

SRCS        =   main.cpp \
				Client.cpp \
				debug.cpp \
				fdUtils.cpp \
				server/Server.cpp \
				server/serverEpoll.cpp \
				server/serverUtils.cpp \
				server/listeningSockets.cpp \
				request/Request.cpp \
				request/requestParsingLine.cpp \
				request/requestParsingHeader.cpp \
				request/requestParsingBody.cpp \
				request/requestUtils.cpp \
				response/Response.cpp \
				response/responseBuild.cpp \
				response/responseHandlers.cpp \
				response/responseFormat.cpp \
				response/responseSession.cpp \
				Session.cpp \
				cgi/Cgi.cpp \
				cgi/cgiUtils.cpp \
				configuration/AConfig.cpp \
				configuration/Http.cpp \
				configuration/Location.cpp \
				configuration/ParseConfig.cpp \
				configuration/ServerConfig.cpp

OBJS        =   $(SRCS:%.cpp=$(DIR_BUILD)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CXX) $(OBJS) $(CXXFLAGS) -o $@
	@echo "Built $(NAME)"

$(DIR_BUILD)/%.o: $(DIR_SRC)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(LDINCLUDE) -c $< -o $@

clean:
	@rm -rf $(DIR_BUILD)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re