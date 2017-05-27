NAME		= MyCode

SRC		= main.c \
		  new.c		\
		  vector_modifiers.c \
		  vector_accessors.c \
		  map_modifiers.c \
		  map_accessors.c \
		  get_opt.h \
		  opt_types.h \
		  pair.c \
		  lexer.c \
		  parser.c \
		  compiler.c \
		  vm.c


CFLAGS		+= -Iinclude/ -IC-modulaire/include -Wall -Wextra -W -std=gnu99 -g3

CC		= gcc

RM		= rm -f

SRCDST		= src

OBJDST		= obj

BINDST		= bin

PROJECT		= $(addprefix  $(BINDST)/, $(NAME))

SRCS		= $(addprefix $(SRCDST)/, $(SRC))

OBJ		= $(addsuffix .o, $(basename $(subst $(SRCDST), $(OBJDST), $(SRCS))))

$(OBJDST)/%.o	: $(SRCDST)/%.c
		  @mkdir -p $(OBJDST)
		  $(CC) $(CFLAGS) -c $< -o $@

$(PROJECT)	: $(OBJ)
		  @mkdir -p $(BINDST)
		  $(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@
		  ln -sf $@ $(NAME)

all		: $(NAME)


$(NAME)		: $(PROJECT)

clean		:
		  $(RM) $(OBJ)

fclean		: clean
		  $(RM) $(PROJECT)
		  $(RM) $(NAME)

re		: fclean all
