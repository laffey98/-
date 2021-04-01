1. block struct.h中定义了区块存储的结构体。
2. initial.cpp会初始化一个区块，并保存到block0.dat中。
3. btc.cpp可以计算任意区块数，保证其sha256的结果前8位bits为0，保存至block1.dat，block2.dat，block3.dat.......blockn.dat。
4. sha256_check.cpp可以验算任意文件的sha256结果。
