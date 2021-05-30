本项目使用说明
1.先用initial.cpp生成最初的区块block0.dat
2.运行server.exe，并输入你想要生成的区块数，点击回车
3.接下来你有8秒的时间开启任意数量的client1.exe（时间可在代码里调节）
4.每当计算出一个区块，server会自动sleep 4秒钟（时间可在代码里调节，直接将sleep语句删了也可以）
5.sha256_check.cpp可以验算任意文件的sha256结果，并告知前多少bits为零。


备注：
    1.代码思路！=伪代码思路。代码思路绝大部分==伪代码，实现时有所更改。
    2.即使已经开始生成区块了，也可以开启新的client1.exe（比如现在已经在生成第4个区块了，此时运行一个client1.exe,过一会，它会自动加入第五个区块的计算）
    3.一定要在输入你想要生成的区块数，点击回车后，再开启client1.exe。不然client就会连接失败，只能关闭。
    4.编译指令:g++ -o client1 client1.cpp -L ./ -lWS2_32 -lpthread(同理server.cpp)