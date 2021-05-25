#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

#define MAX_pthread 20

SOCKET sClient[MAX_pthread];
pthread_t rev[MAX_pthread];
int p[MAX_pthread];
char revData[40];
int cli = 0, flag = 0;

struct client_ans {
    char Nunce_2[8];
};

// void* func_acc(void* client);
// int create_bl(int a);

//接收多个客户端传来的信息，并更改多个标志值的线程
void* rev_pthr(void* ss) {
    int* ddd = (int*)ss;
    int s = *ddd;
    int ret = recv(sClient[s], revData, 255, 0);
    // printf(ret);
    if (ret > 0) {
        revData[ret] = 0x00;
        printf(revData);
        printf("\n");
    }
    //printf("s:%d\n", s);
    p[s] = 1;
    flag = 1;
    pthread_exit(NULL);
}

//与多个客户端产生连接的线程
void* acc_pthr(void* s) {
    while (1) {
        SOCKET* slisten = (SOCKET*)s;
        struct sockaddr_in remoteAddr[MAX_pthread];
        int nAddrlen = sizeof(sockaddr_in);
        sClient[cli] = accept(*slisten, (SOCKADDR*)&remoteAddr[cli], &nAddrlen);
        if (sClient[cli] == INVALID_SOCKET) {
            printf("accept error !");
        }
        int ap = cli;
        pthread_create(&rev[cli], NULL, rev_pthr, &ap);
        cli++;
        printf("cli:%d\n", cli);
    }
}

//向所有客户端发送信息
void send_to_cli(int cli1) {
    char sendData[20] = "hello world!\n";
    for (int ss = 0; ss < cli1; ss++) {
        if (send(sClient[ss], sendData, strlen(sendData), 0) > 0)
            /*printf("%dsend over\n", ss)*/;
        // sleep(1);
    }
}

//创建block
void create_block(int i) {
    printf(revData);
    printf("%d\n", i);
}

int main(int argc, char* argv[]) {
    //初始化WSA
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0) {
        return 0;
    }

    int num;
    printf("how many block: ");
    scanf("%d", &num);

    //创建套接字
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET) {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    // sin.sin_port = htons(1500);
    // sin.sin_addr.S_un.S_addr = inet_addr("192.168.190.134");
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
        printf("bind error !");
    }

    //开始监听
    if (listen(slisten, 5) == SOCKET_ERROR) {
        printf("listen error !");
        return 0;
    }

    printf("server is running...\n");

    pthread_t acc;
    pthread_create(&acc, NULL, acc_pthr, &slisten);
    sleep(4);
    for (int i; i < num; i++) {
        sleep(4);
        int cli1 = cli;
        //printf("cli::%d cli1:%d\n", cli, cli1);
        printf("sleep over\n");
        send_to_cli(cli1);
        flag = 0;
        while (1) {
            if (flag == 1) {
                // printf("cli1:%d p[0]:%d\n", cli1, p[0]);
                for (int ss = 0; ss < cli1; ss++) {
                    if (p[ss] == 1) {
                        pthread_create(&rev[ss], NULL, rev_pthr, &ss);
                        printf("%drev create\n", ss);
                        p[ss] = 0;
                    }
                    send(sClient[ss], "ok", strlen("ok"), 0);
                }
                create_block(i);
                //sleep(5);
                break;
            }
        }
        // sleep(4);
    }
    /*sClient = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
    if (sClient == INVALID_SOCKET) {
        printf("accept error !");
    }
    printf("accept a connection:%s \r\n", inet_ntoa(remoteAddr.sin_addr));*/
    /*pthread_t th[MAX_pthread];
    for (int i; i < num; i++) {
        client respon;
        respon.flag = 0;
        for (int c = 0; c < MAX_pthread; c++) {
            // char revData[255];
            // printf("waiting...\n");
            /*
            //connect many client
            sClient = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
            if (sClient == INVALID_SOCKET) {
                printf("accept error\n");
                continue;  //继续等待下一次连接
            }

            sClient[c] = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
            pthread_create(&th[c], NULL, func_acc, &respon);
            if (respon.flag == 1) break;
            /*
            //接收数据
            int ret = recv(sClient, revData, 255, 0);
            // printf(ret);
            if (ret > 0) {
                revData[ret] = 0x00;
                printf(revData);
            }

            //发送数据
            // char  sendData[20] = "hello world！\n";
            // send(sClient, sendData, strlen(sendData), 0);
        }
        if (create_bl(i) == 1)
            printf("block%d has created!", i);  // create a new block
        else
            printf("block%d is failed to create!", i);
    }
    */
    printf("ooooooooooooover\n");
    sleep(10);
    getchar();
    for (int i = 0; i < cli; i++) {
        closesocket(sClient[i]);
    }
    closesocket(slisten);
    WSACleanup();
    return 0;
}