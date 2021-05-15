#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>

using namespace std;

int main()
{
    /**
     * Создание сокета и проверка на ошибку
     */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return 1;
    }

    /**
     * Определение порта и адреса
     * Определение информации для сервера, к которому происходит подключение
     */
    int port = 54000;
    string ipAddress = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    /**
     * Подключение к серверу
     */
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        return 1;
    }

    /**
     * Бесконечный цикл, в котором считываем информацию из текстового файла
     * и отправляем это на сервер
     */
    string userInput;
    string txtLine;
    ifstream fileHandler;

    do {

        cout << "File name: ";
        getline(cin, userInput);

        fileHandler.open(userInput);
        if(fileHandler.is_open())
        {
            while(getline(fileHandler, txtLine))
            {
                int sendRes = send(sock, txtLine.c_str(), txtLine.size() + 1, 0);
                if (sendRes == -1)
                {
                    cout << "Error with sending to server\r\n";
                    continue;
                }
                cout << txtLine << endl;
            }

            fileHandler.close();
        }
        else
        {
            cout << "Incorrect file name\n";
        }


    } while(true);

    /**
     * Закрытие сокета
     */
    close(sock);

    return 0;
}
