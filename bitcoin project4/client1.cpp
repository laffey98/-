#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>

#include "sha256.cpp"
#pragma comment(lib, "ws2_32.lib")

int send_data = 0;
SOCKET sclient;
// char sendData[999] = "1000000000";
block buf;

void calcu(char* a) {
    SHA256_HASH p;
    int x =rand()%600 , y = 0;
    do {
        buf.Nonce_2[y / 256]++;
        y++;
        if (y == 256 * 8 - 1) {
            buf.Nonce_1[x / 256]++;
            x++;
            y = 0;
        }
        if (x == 256 * 8 - 1) {
            printf("sha256 is failed!");
            //pthread_exit(NULL);
        }
        Sha256Calculate(&buf, sizeof(buf), &p);
    } while (check(p) == 0);
    for (int i = 0; i < 2; i++) {
        // printf("%s\n", a);
        printf("123");
        //sleep(1);
        pthread_testcancel();
    }
    //pthread_exit(NULL);
}

void* calculate(void* rev) {
    //char* as = (char*)rev;
    calcu(NULL);  // pthread_testcancel();
    if (send(sclient, (char*)(&(buf.Nonce_2)), sizeof(buf.Nonce_2), 0) > 0)
        printf("send over\n");
    else
        printf("send defealt\n");
    send_data = 1;
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0) {
        return 0;
    }

    sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sclient == INVALID_SOCKET) {
        printf("invalid socket !");
        return 0;
    }

    struct sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    // serAddr.sin_port = htons(1500);
    // serAddr.sin_addr.S_un.S_addr = inet_addr("192.168.190.134");
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    if (connect(sclient, (struct sockaddr*)&serAddr, sizeof(serAddr)) ==
        SOCKET_ERROR) {
        printf("connect error !");
        closesocket(sclient);
        getchar();
        return 0;
    }

    int f_ok = 0, f_pt = 0;
    // char rev[255];
    int ret;
    pthread_t calcu;

    printf("wuhu\n");
    srand(time(NULL));

    for (int i = 0; i < 100; i++) {
        f_ok = 0;
        ret = recv(sclient, (char*)(&buf), sizeof(block), 0);
        printf("rev over\n");
        if (ret > 0) {
            // rev[ret] = 0x00;
            // printf(recData);
            // printf("%s\n", rev);
        } else
            printf("rev defealt\n");
        if (strncmp((char*)(&buf), "ok", 2) == 0) {
            printf("ok rev\n");
            if ((f_pt == 1) && (send_data == 0)) {
                pthread_cancel(calcu);
                printf("cancel\n");
            }
            ret = recv(sclient, (char*)(&buf), sizeof(buf), 0);
            if (ret > 0) {
                printf("revvvvvvvvvv\n");
                // rev[ret] = 0x00;
                // printf(recData);
                // printf("%s\n", rev);
            } else
                printf("rev defealt\n");
        }
        f_pt = 1;
        printf("calcu start\n");
        pthread_create(&calcu, NULL, calculate, NULL);
        send_data = 0;
        /*
        char  sendData[20] = "client";
        printf("%d\n",i);
        send(sclient, sendData, strlen(sendData), 0);
        char recData[255];
        */
        /*int ret = recv(sclient, recData, 255, 0);
        if (ret > 0)
        {

                recData[ret] = 0x00;
                //printf(recData);
                printf("%d%s",i,recData);
        }
        */
    }
    closesocket(sclient);
    WSACleanup();
    getchar();
    return 0;
}