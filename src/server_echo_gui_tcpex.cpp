// #include <placeholder>
#include <wex.h>
#include "cStarterGUI.h"
#include "cTCPex.h"

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Server echo",
              {50, 50, 500, 500}),
          lbPort(wex::maker::make<wex::label>(fm)),
          ebPort(wex::maker::make<wex::editbox>(fm)),
          bnConnect(wex::maker::make<wex::button>(fm)),
          lbStatus(wex::maker::make<wex::label>(fm))
    {
        lbPort.move(50, 50, 60, 30);
        lbPort.text("Port");
        ebPort.move(120, 50, 100, 30);
        ebPort.text("27678");
        bnConnect.move(70, 100, 100, 30);
        bnConnect.text("Connect");
        lbStatus.move(50, 150, 300, 50);
        lbStatus.text("");

        bnConnect.events().click(
            [this]
            {
                tcpex.start_server(
                    ebPort.text(),
                    std::bind(
                        &eventHandler, this,
                        std::placeholders::_1),
                    1);
                status("Waiting for client connection");
            });

        show();
        run();
    }

private:
    wex::label &lbPort;
    wex::editbox &ebPort;
    wex::button &bnConnect;
    wex::label &lbStatus;

    raven::set::cTCPex tcpex;

    void status(const std::string &msg)
    {
        lbStatus.text(msg);
        lbStatus.update();
    }

    std::string eventHandler(
        const raven::set::cTCPex::sEvent& e)
    {
        switch (e.type)
        {
        case raven::set::cTCPex::eEvent::accept:
            status("Client Connected");
            return "";
        case raven::set::cTCPex::eEvent::disconnect:
            status("Client disconnected");
            return "";
        case raven::set::cTCPex::eEvent::read:
            status("Client: " + e.msg);
            return e.msg;
        default:
            return "";
        }
    }
};

main()
{
    cGUI theGUI;
    return 0;
}