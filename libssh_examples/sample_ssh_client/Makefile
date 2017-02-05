NAME 	= 	sample_ssh_client

CC 	= 	gcc

SRCS 	=	main.c

OBJS 	= 	$(SRCS:.c=.o)

LIBS 	= 	-lssh


all:		$(NAME)

$(NAME): 	$(OBJS)
		$(CXX) -o $(NAME) $(OBJS) $(LIBS)

clean:
		$(RM) $(OBJS)

fclean:		clean
		$(RM) $(NAME)

re:		fclean all

.PHONY: 	all clean fclean re
