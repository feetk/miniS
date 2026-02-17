# --- Noms et Compilateur ---
NAME        = minishell
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -g3 

# --- Libft ---
LIBFT_PATH	= ./libft
LIBFT		 = $(LIBFT_PATH)/libft.a

# --- Fichiers ---
SRCS        = main.c \
              lexer.c \
              lexer_2.c \
			  expander.c \
			  env.c \
			  parser.c \
			  quote.c \
			  debug.c

OBJS        = $(SRCS:.c=.o)

LIBS        = -L$(LIBFT_PATH) -lft -lreadline

INCLUDES	= -I ./ -I $(LIBFT_PATH)

HEADER      = minishell.h

# --- Règles ---

all: $(NAME)

$(LIBFT):
	@echo " Compilation de la Libft..."
	@make -C $(LIBFT_PATH)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)
	@echo "✅ Minishell compilé avec succès !"

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean:
	@make clean -C $(LIBFT_PATH)
	rm -f $(OBJS)
	@echo "🧹 Fichiers objets supprimés."

fclean: clean
	@make fclean -C $(LIBFT_PATH)
	rm -f $(NAME)
	@echo "🗑️ Exécutable supprimé."

re: fclean all

.PHONY: all clean fclean re