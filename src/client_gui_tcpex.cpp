
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
                            std::placeholders::_1)))
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

        //Regular check for status update request from another thread
        myUpdateTimer = new wex::timer(fm, 200);
        fm.events().timer([this](int id)
                          { StatusUpdate(); });

        show();
        run();
    }

    /// @brief update status display
    /// @param msg 
    void status(const std::string &msg);

    /// @brief threadsafe status display update request
    /// @param msg 
    void statusAdd(const std::string &msg)
    {
        std::lock_guard<std::mutex> lck(mtxStatus);
        myStatusReady = msg;
    }

    /// @brief threadsafe update status display
    void StatusUpdate()
    {
        std::lock_guard<std::mutex> lck(mtxStatus);
        if( !myStatusReady.empty() ) {
            status( myStatusReady );
            myStatusReady.clear();
        }
    }
    
    /* @brief Update status after an event on the tcp socket
    /// @param e 
    /// @return ""

    The event handler is called from a thread in the tcp socket wrapper
    It must not update the GUI from that thread
    So it adds a request for a display update
    The GUI thread will check reqularly and updates the display as required
   */
    std::string eventHandler(
        const raven::set::cTCPex::sEvent &e)
    {
        switch (e.type)
        {
        case raven::set::cTCPex::eEvent::accept:
            statusAdd("Connected to server");
            return "";
        case raven::set::cTCPex::eEvent::disconnect:
            statusAdd("Disconnected from server");
            return "";
        case raven::set::cTCPex::eEvent::read:
            statusAdd("server: " + e.msg);
            return "";
        default:
            return "";
        }
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

    std::mutex mtxStatus;
    std::string myStatusReady;
    wex::timer * myUpdateTimer;
};

void cGUI::status(const std::string &msg)
{
    int pc = 100 - msg.size();
    std::string m(msg);
    for (int k = 0; k < pc; k++)
        m.push_back(' ');
    lbStatus.text(m);
    lbStatus.update();
}

main()
{
    cGUI theGUI;
    return 0;
}
