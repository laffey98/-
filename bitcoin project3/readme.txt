1. block struct.h中定义了区块存储的结构体。
2. ecc.c,ecc.h中储存了ECC算法相关函数。
3. btc.h,sha256.cpp是project2里的文件，存有sha256相关函数。
4. 先运行initial.cpp会初始化一个区块，并保存到block0.dat中。
5. 然后运行main.cpp，可以输入想要生成的区块数量，然后生成对应区块。
6. main.cpp中的子函数都有用于测试的代码块，可以打印区块数据，签名，签名检测等等数据。
7. sha256_check.cpp可以验算任意文件的sha256结果，并告知前多少bits为零。
