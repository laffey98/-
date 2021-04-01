struct block 
{
    char chain_version[8];
    char Prev_hash[32];
    char Nonce_1[8];
    char Nonce_2[8];
    char coinbase[32];
    char amount[8];
    char data[1024];
};
