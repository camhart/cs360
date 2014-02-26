# Makefile for socket examples

CXX=			g++ $(CCFLAGS)
msgd=			msgd.o Server.o Data.o User.o Message.o Str.o
msg=			msg.o Client.o Str.o
OBJS =			$(msgd) $(msg)

CCFLAGS= -std=c++11 -g

all:	msgd Server Data User Message Str msg Client

server:$(msgd)
	$(CXX) -o msgd $(msgd) 

client:$(msg)
	$(CXX) -o msg $(msg)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d)

realclean:
	rm -f $(OBJS) $(OBJS:.o=.d) msgd msg


# These lines ensure that dependencies are handled automatically.
%.d:	%.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

include	$(OBJS:.o=.d)
