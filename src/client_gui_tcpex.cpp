
#include <wex.h>
#include "cStarterGUI.h"
#include "cTCPex.h"

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "CLIENT",
              {50, 50, 500, 500}),
          bnConnect(wex::maker::make<wex::button>(fm)),
          lbServerAddress(wex::maker::make<wex::label>(fm)),
          ebServerAddress(wex::maker::make<wex::editbox>(fm)),
          lbServerPort(wex::maker::make<wex::label>(fm)),
          ebServerPort(wex::maker::make<wex::editbox>(fm)),
          lbStatus(wex::maker::make<wex::label>(fm)),
          lbSend(wex::maker::make<wex::label>(fm)),
          ebSend(wex::maker::make<wex::editbox>(fm)),
          bnSend(wex::maker::make<wex::button>(fm))
    {
        bnConnect.move(50, 130, 100, 30);
        bnConnect.text("Connect");
        lbServerAddress.move(50, 50, 100, 30);
        lbServerAddress.text("Server Address");
        ebServerAddress.move(160, 50, 100, 30);
        ebServerAddress.text("127.0.0.1");
        lbServerPort.move(50, 80, 100, 30);
        lbServerPort.text("Server Port");
        ebServerPort.move(160, 80, 100, 30);
        ebServerPort.text("27678");
        lbStatus.move(50, 180, 300, 50);
        lbStatus.text("");
        lbSend.move(50, 250, 70, 30);
        lbSend.text("Message");
        ebSend.move(150, 250, 150, 30);
        ebSend.text("");
        bnSend.move(320, 250, 100, 30);
        bnSend.text("SEND");

        bnConnect.events().click(
            [this]
            {
                if (!tcpex.connect_to_server(
                        ebServerAddress.text(),
                        ebServerPort.text(),
                        std::bind(
                            &eventHandler, this,
                            std::placeholders::_1,
                            std::placeholders::_2,
                            std::placeholders::_3)))
                    status("NOT Connected to server");
                else
                    status("Connected to server");
            });

        bnSend.events().click(
            [this]
            {
                tcpex.send(
                    ebSend.text() + "\n");
            });

        show();
        run();
    }

    void status(const std::string &msg)
    {
        int pc = 100 - msg.size();
        std::string m(msg);
        for (int k = 0; k < pc; k++)
            m.push_back(' ');
        lbStatus.text(m);
        lbStatus.update();
    }
    std::string eventHandler(
        int client,
        raven::set::cTCPex::eEvent type,
        const std::string &msg)
    {
        switch (type)
        {
        case raven::set::cTCPex::eEvent::accept:
            status("Connected to server");
            break;
        case raven::set::cTCPex::eEvent::disconnect:
            status("Disconnected from server");
            break;
        case raven::set::cTCPex::eEvent::read:
            status("server: " + msg);
            break;
        }
        return "";
    }

private:
    wex::button &bnConnect;
    wex::label &lbServerAddress;
    wex::editbox &ebServerAddress;
    wex::label &lbServerPort;
    wex::editbox &ebServerPort;
    wex::label &lbStatus;
    wex::label &lbSend;
    wex::editbox &ebSend;
    wex::button &bnSend;

    raven::set::cTCPex tcpex;
};

main()
{
    cGUI theGUI;
    return 0;
}
