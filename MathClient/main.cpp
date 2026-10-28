#include "authregform.h"
#include <QApplication>
#include "mytcpclient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Initialize TCP client
    MyTcpClient::getInstance();

    AuthRegForm w;
    w.show();
    return a.exec();
}
