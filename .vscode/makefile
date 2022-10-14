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
	$(ODIR)/cTCPex.o \

OBJ_client_gui = \
	$(ODIR)/client_gui_tcpex.o \
	$(ODIR)/cTCPex.o \

OBJ_server_echo_console_tcpex = \
	$(ODIR)/server_echo_console_tcpex.o \
	$(ODIR)/cTCPex.o 

OBJ_server_echo_gui_tcpex = \
	$(ODIR)/server_echo_gui_tcpex.o \
	$(ODIR)/cTCPex.o \

OBJ_server_job_gui_tcpex = \
	$(ODIR)/server_job_gui_tcpex.o \
	$(ODIR)/cTCPex.o \

OBJ_bbserver = \
	$(ODIR)/bbserver.o \
	$(ODIR)/cTCPex.o \

OBJ_server_relay = \
	$(ODIR)/server_relay_console_tcpex.o \
	$(ODIR)/cTCPex.o \

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++ -c  $< \
	-o $@  $(INCS) $(CCflags)

client_console: $(OBJ_client_console)
	g++ -o bin/client_console.exe \
	$^ $(static_link_flags) $(LIBS)

client_gui: $(OBJ_client_gui) cTCPex.h
	g++ -o bin/client_gui.exe \
	$^ $(static_link_flags) $(LIBS)

server_echo_console: $(OBJ_server_echo_console_tcpex) cTCPex.h
	g++ -o bin/server_echo_console.exe \
	$^ $(static_link_flags) $(LIBS)

server_echo_gui: $(OBJ_server_echo_gui_tcpex) cTCPex.h
	g++ -o bin/server_echo_gui.exe \
	$^ $(static_link_flags) $(LIBS)

server_job_gui: $(OBJ_server_job_gui_tcpex) cTCPex.h
	g++ -o bin/server_job_gui.exe \
	$^ $(static_link_flags) $(LIBS)

bbserver: $(OBJ_bbserver) cTCPex.h
	g++ -o bin/bbserver.exe \
	$^ $(static_link_flags) $(LIBS)

server_relay: $(OBJ_server_relay) cTCPex.h
	g++ -o bin/server_relay.exe \
	$^ $(static_link_flags) $(LIBS)


.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

