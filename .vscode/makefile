#source file search paths
VPATH = ./src 

	# compiler include search paths
 INCS = \
 	-I../ravencode/pub/src
# 	-I../raven-set \
# 	-I../boost/boost1_78 

LIBS = \
	-lws2_32 -lwsock32 \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \

	#-mwindows

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_
static_link_flags = -static-libstdc++ -static-libgcc -static $(OPT)

# sources
OBJ_client_console = \
	$(ODIR)/client_console_tcpex.o \
	$(ODIR)/cClient.o \
	$(ODIR)/cTCPex.o \

# sources
OBJ_client_gui = \
	$(ODIR)/client_gui_tcpex.o \
	$(ODIR)/cClient.o \
	$(ODIR)/cTCPex.o \

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++ -c  $< \
	-o $@  $(INCS) $(CCflags)

client_console: $(OBJ_client_console)
	g++ -o bin/client_console.exe \
	$^ $(static_link_flags) $(LIBS)

client_gui: $(OBJ_client_gui)
	g++ -o bin/client_gui.exe \
	$^ $(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

