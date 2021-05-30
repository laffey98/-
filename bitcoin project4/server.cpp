#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>

#include "ecc.c"
#include "sha256.cpp"

#pragma comment(lib, "ws2_32.lib")

#define MAX_pthread 20

SOCKET sClient[MAX_pthread];
pthread_t rev[MAX_pthread];
int p[MAX_pthread];
block buf;
// char revData[40];
int cli = 0, flag = 0;
// char sendData[20] = "hello world!\n";
uint8_t Nonce_2[8];

struct client_ans {
    char Nunce_2[8];
};

// void* func_acc(void* client);
// int create_bl(int a);

//以下是block生成相关操作
key_pair key[10];
data transaction[5];
//block buf;

//生成十个公私钥对
int random_key() {
    int a, i, n;
    uint8_t p_publicKey[33];
    uint8_t p_privateKey[32];
    for (i = 0; i < 10; i++) {
        ecc_make_key(p_publicKey, p_privateKey);
        for (a = 0; a < 32; a++) {
            // printf("%0x", p_privateKey[a]);
            key[i].p_privateKey[a] = p_privateKey[a];
        }
        // printf("\n");
        for (a = 0; a < 33; a++) {
            key[i].p_publicKey[a] = p_publicKey[a];
            // printf("%0x", p_publicKey[a]);
        }
        // printf("\n");
    }
    return 0;
}

//随机生成一个区块的交易
sign signa;
int random_deal() {
    srand(time(NULL));
    int a, i, from;
    for (i = 0; i < 5; i++) {
        a = rand() % 5;
        from = a;
        //随机放入公钥
        for (int n = 0; n < 33; n++) {
            transaction[i].from_addr[n] = key[a].p_publicKey[n];
        }
        do a = rand() % 5;
        while (from == a);
        for (int n = 0; n < 33; n++) {
            transaction[i].to_addr[n] = key[a].p_publicKey[n];
        }
        //随机放入金额(目前数额不整太大，amount<256)
        for (int n = 7; n > 6; n--) {
            a = rand() % 256;
            transaction[i].amount[n] = a;
        }
        //放入签名
        for (int n = 0; n < 33; n++) {
            signa.from_addr[n] = transaction[i].from_addr[n];
            signa.to_addr[n] = transaction[i].to_addr[n];
        }
        for (int n = 0; n < 8; n++) {
            signa.amount[n] = transaction[i].amount[n];
        }
        SHA256_HASH p;
        Sha256Calculate(&signa, sizeof(sign), &p);
        ecdsa_sign(key[from].p_privateKey, p.bytes, transaction[i].signature);
    }
    //打印测试
    /*for (i = 0; i < 5; i++) {
        printf("From_addr:");
        for (int n = 0; n < 33; n++) {
            printf("%0x", transaction[i].from_addr[n]);
        }
        printf("\n");
        printf("To_addr:");
        for (int n = 0; n < 33; n++) {
            printf("%0x", transaction[i].to_addr[n]);
        }
        printf("\n");
        printf("Amount:");
        for (int n = 0; n < 8; n++) {
            printf("%0x", transaction[i].amount[n]);
        }
        printf("\n");
        printf("Signature:");
        for (int n = 0; n < 64; n++) {
            printf("%0x", transaction[i].signature[n]);
        }
        printf("\n");
    }*/
    return 0;
}

//检查一个区块所有交易合法性
int blockcheck() {
    int flag;
    for (int i = 0; i < 5; i++) {
        for (int n = 0; n < 33; n++) {
            signa.from_addr[n] = transaction[i].from_addr[n];
            signa.to_addr[n] = transaction[i].to_addr[n];
        }
        for (int n = 0; n < 8; n++) {
            signa.amount[n] = transaction[i].amount[n];
        }
        SHA256_HASH p;
        Sha256Calculate(&signa, sizeof(sign), &p);
        flag = ecdsa_verify(transaction[i].from_addr, p.bytes,
                            transaction[i].signature);
        // printf("%d", flag);
        if (flag == 0) return 0;
    }
    return 1;
}

//生成一个区块
int create_block(int n) {
    char as[15];
    SHA256_HASH p;
    sprintf(as, "block%d.dat", n);
    FILE* fp = fopen(as, "ab+");
    Sha256Calculate(&buf, sizeof(buf), &p);
    if (check(p) == 0) {
        printf("sha256 is failed!");
    }
    fwrite(&buf, sizeof(block), 1, fp);
    fclose(fp);
    return 0;
}

void pre_send(int n){
    random_deal();
    char as[15];
    sprintf(as, "block%d.dat", n - 1);
    FILE* fpp = fopen(as, "ab+");
    SHA256_HASH p;
    fread(&buf, sizeof(buf), 1, fpp);
    Sha256Calculate(&buf, sizeof(buf), &p);
    for (int i = 0; i < 32; i++) {
        buf.Prev_hash[i] = p.bytes[i];
    }
    buf.chain_version[0] = n;
    for (int i; i < 33; i++) {
        buf.coinbase[i] = key[1].p_publicKey[i];
    }
    for (int i = 0; i < 5; i++) {
        buf.transaction[i] = transaction[i];
    }
    fclose(fpp);
}

int blockdata(int a) {
    if (blockcheck() == 1) {
        printf("block%d check!", a);
    } else
        printf("fail!");
    printf("\n");
    create_block(a);
    return 0;
}

//接收多个客户端传来的信息，并更改多个标志值的线程
void* rev_pthr(void* ss) {
    int* ddd = (int*)ss;
    int s = *ddd;
    int ret = recv(sClient[s], (char*)(&buf.Nonce_2), sizeof(Nonce_2), 0);
    // printf(ret);
    if (ret > 0) {
        // revData[ret] = 0x00;
        /*for (int i = 0; i < 8; i++) {
        printf("%02x",buf.Nonce_2[i]);
    }*/
        // printf((char*)(&buf.Nonce_2));
        // printf("\n");
    }
    // printf("s:%d\n", s);
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
void send_to_cli(int cli1,int n) {
    pre_send(n);
    for (int ss = 0; ss < cli1; ss++) {
        if (send(sClient[ss], (char*)(&buf), sizeof(block), 0) > 0)
            printf("%dsend over\n", ss);
        // sleep(1);
    }
}

//创建block
void create_block1(int i) {
    blockdata(i);
    for (int i = 0; i < 8; i++) {
        printf("%02x", buf.Nonce_2[i]);
    }
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
    random_key();
    for (int i = 1; i < num + 1; i++) {
        sleep(4);
        int cli1 = cli;
        // printf("cli::%d cli1:%d\n", cli, cli1);
        printf("sleep over\n");
        send_to_cli(cli1,i);
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
                create_block1(i);
                // sleep(5);
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