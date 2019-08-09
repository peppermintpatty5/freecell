CC		= TCC
OBJS	= card.obj cascade.obj extra.obj freecell.obj graphics.obj main.obj
TARGET	= freecell.exe

$(TARGET) : $(OBJS)
	$(CC) -e$(TARGET) $(OBJS)

.c.obj :
	$(CC) -c $.
