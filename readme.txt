1. block struct.h中定义了区块存储的结构体。
2. btc.cpp会计算一个区块大小，并更改Nonce1，使得区块sha256的结果前8个bits为零，然后存储到good.dat中。
3. sha256_check.cpp可以验算good.dat的sha256结果。