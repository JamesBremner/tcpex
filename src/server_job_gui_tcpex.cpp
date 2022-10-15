// #include <placeholder>
#include <wex.h>
#include "cStarterGUI.h"
#include "cTCPex.h"

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Server job",
              {50, 50, 500, 500}),
          lbPort(wex::maker::make<wex::label>(fm)),
          ebPort(wex::maker::make<wex::editbox>(fm)),
          bnConnect(wex::maker::make<wex::button>(fm)),
          lbStatus(wex::maker::make<wex::label>(fm)),
          lbMaxClient(wex::maker::make<wex::label>(fm)),
          ebMaxClient(wex::maker::make<wex::editbox>(fm)),
          cbSharedThread(wex::maker::make<wex::checkbox>(fm))
    {
        lbPort.move(50, 50, 60, 30);
        lbPort.text("Port");
        ebPort.move(120, 50, 100, 30);
        ebPort.text("27678");
        lbMaxClient.move(250, 50, 100, 30);
        lbMaxClient.text("Max clients");
        ebMaxClient.move(350, 50, 50, 30);
        ebMaxClient.text("2");
        bnConnect.move(70, 100, 100, 30);
        bnConnect.text("Connect");
        lbStatus.move(50, 150, 300, 50);
        lbStatus.text("");
        cbSharedThread.move(200, 100, 200, 30);
        cbSharedThread.text("Shared JobThread");
        cbSharedThread.check();

        bnConnect.events().click(
            [this]
            {
                start_server();
            });

        show();
        run();
    }

private:
    wex::label &lbPort;
    wex::editbox &ebPort;
    wex::button &bnConnect;
    wex::label &lbStatus;
    wex::label &lbMaxClient;
    wex::editbox &ebMaxClient;
    wex::checkbox &cbSharedThread;

    raven::set::cTCPex tcpex;

    void status(const std::string &msg)
    {
        lbStatus.text(msg);
        lbStatus.update();
    }

    std::string eventHandler(
        const raven::set::cTCPex::sEvent &e);

    /// @brief process message received from client
    /// @param client
    /// @param msg
    /// @return
    const std::string processor(
        int client,
        const std::string &msg);

    void start_server();
};

void cGUI::start_server()
{
    bnConnect.enable(false);

    tcpex.sharedProcessingThread(
        cbSharedThread.isChecked());

    tcpex.start_server(
        ebPort.text(),
        std::bind(
            &eventHandler, this,
            std::placeholders::_1),
        atoi(ebMaxClient.text().c_str()));
    status("Waiting for client connection");
}

std::string cGUI::eventHandler(
    const raven::set::cTCPex::sEvent &e)
{
    std::string reply;

    switch (e.type)
    {
    case raven::set::cTCPex::eEvent::accept:
        status("Client Connected");
        return "";
    case raven::set::cTCPex::eEvent::disconnect:
        status("Client disconnected");
        return "";

    case raven::set::cTCPex::eEvent::read:
        reply = "start client " 
            + std::to_string(e.client) 
            + " job " + e.msg;
        status(reply);
        tcpex.send(reply, e.client);

        // process the job
        processor(e.client, e.msg);

        status("end job " + e.msg);
        return "end job " + e.msg;

    default:
        return "";
    }
}

const std::string cGUI::processor(
    int client,
    const std::string &msg)
{
    // specified job duration, default 1 second
    int duration = atoi(msg.c_str());
    if (!duration)
        duration = 1;

    // simulate job of specified duration
    std::this_thread::sleep_for(
        std::chrono::seconds(duration));

    return "end job " + msg;
}

main()
{
    cGUI theGUI;
    return 0;
}