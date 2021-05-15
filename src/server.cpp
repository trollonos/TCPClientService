#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>
#include <csignal>

using namespace std;

int main()
{
    /**
     * При получении сигналов о прекращении работы и о потере соединения
     * вызываются стандартные обработчики сигналов. В обоих случаях это заверешение процесса.
     */
    signal(SIGTERM, SIG_DFL);
    signal(SIGHUP, SIG_DFL);

    /**
     * Создание сокета
     */
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        return -1;
    }

    /**
     * Биндим порт и адрес для сокета
     */
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    listen(listening, SOMAXCONN);

    /**
     * Ожидаем соединения
     */
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    char host[NI_MAXHOST];      // Клиент
    char service[NI_MAXSERV];   // Порт, по которому подключен клиент

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    }

    /**
     * Закрытие сокета
     */
    close(listening);

    char buf[8192];
    ofstream fileHandler;

    /**
     * Принимаем информацию от клиента
     * и записываем в файл
     */
    while (true)
    {
        memset(buf, 0, 8192);

        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        if (bytesReceived == 0)
        {
            cout << "Error" << endl;
            break;
        }

        string receivedStr = string(buf, 0, bytesReceived);
        fileHandler.open("data.txt", fstream::app);
        cout << receivedStr;
        fileHandler << receivedStr << endl;
        fileHandler.close();
    }

    //fileHandler.close();

    /**
     * Закрытие сокета
     */
    close(clientSocket);

    return 0;
}