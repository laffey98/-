#include <stdio.h>
#include <time.h>

#include "ecc.c"
#include "sha256.cpp"
key_pair key[10];
data transaction[5];
block buf;

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
    sprintf(as, "block%d.dat", n);
    FILE* fp = fopen(as, "ab+");
    int x = 0, y = 0;
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
            return 0;
        }
        Sha256Calculate(&buf, sizeof(buf), &p);
    } while (check(p) == 0);
    fwrite(&buf, sizeof(block), 1, fp);
    fclose(fp);
    fclose(fpp);
    return 0;
}

int main() {
    int x;
    printf("how many block: ");
    scanf("%d",&x);
    random_key();
    for (int a = 1; a < x+1; a++) {
        random_deal();
        if (blockcheck() == 1) {
            printf("block%d check!",a);
        } else
            printf("fail!");
        printf("\n");
        create_block(a);
    }
    //打印测试
    /*for (int i = 0; i < 5; i++) {
        printf("From_addr:");
        for (int n = 0; n < 33; n++) {
            printf("%0x", buf.transaction[i].from_addr[n]);
        }
        printf("\n");
        printf("To_addr:");
        for (int n = 0; n < 33; n++) {
            printf("%0x", buf.transaction[i].to_addr[n]);
        }
        printf("\n");
        printf("Amount:");
        for (int n = 0; n < 8; n++) {
            printf("%0x", buf.transaction[i].amount[n]);
        }
        printf("\n");
        printf("Signature:");
        for (int n = 0; n < 64; n++) {
            printf("%0x", buf.transaction[i].signature[n]);
        }
        printf("\n");
        for (int n = 0; n < 32; n++) {
            printf("%0x", buf.Prev_hash[n]);
        }
    }*/
    return 0;
}
