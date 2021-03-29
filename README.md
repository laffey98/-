# -
blockchain practise 1.
约定的要求：
1.  自行随机构造一个1MB的数据区。作为模拟用的区块数据（blockchain.dat）。（数据量并未到1M）
2.  在上述1MB数据里，任意选择一个128bits（也就是2个 64bits 宽度的随机数）的数据区，作为随机数（nonce_1, nonce_2）
3.  使用sha256对区块数据，计算出256bits 的结果。
4.  在1MB区块数据中，通过改变nonce_1 和 nonce_2 的数值（其它区块数据不变），来使得sha256的结果（big endian排列），最前面开头是连续8个bits是0 （也就是第一个字节数值是0）。
5.  把1MB区块数据，nonce_1, nonce_2 的数值记录下来，存储到二进制文件里（good.dat），之后便于再次验算。
