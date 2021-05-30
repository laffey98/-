struct data {
    uint8_t from_addr[33];
    uint8_t to_addr[33];
    uint8_t amount[8];
    uint8_t signature[32 * 2];
};

struct sign {
    uint8_t from_addr[33];
    uint8_t to_addr[33];
    uint8_t amount[8];
};

struct block {
    uint8_t chain_version[8];
    uint8_t Prev_hash[32];
    uint8_t Nonce_1[8];
    uint8_t Nonce_2[8];
    uint8_t coinbase[32];
    struct data transaction[5];
};

struct key_pair {
    uint8_t p_publicKey[33];
    uint8_t p_privateKey[32];
};
