#include "cClient.h"

main(int argc, char *argv[])
{
    cClient C;
    
    C.parse_command_line_options(argc,argv);

    C.connect_to_server();

    C.run();

    return 0;
}
