// server(receive)
crea server sock;
acc pthr {
    loop {
        acc sock;  // blocking
        // rev pthr{};
        cli++
    }
}
// crea block
loop {
    send to each sock {
        loop {
            send to one;
            rev pthr {
                rev nonce2;
                flag = 1
            }
        }
    }

    flag = 0;
    loop {  // blocking
        loop {
            shut rev pthr;
            send ok
        }
        crea block;
        // shut rev pthr;
        break;
    }
}
shut client sock;
shut acc pthr;
shut server;

// client
crea client sock;
f_ok = 0;
f_pt = 0;
loop {
    f_ok = 0;
    rece;  // blocking
    if (ok) {
        if (f_pt == 1) cancle calcu pthr;
        rece;
    }
    f_pt = 1;
    calcu pthr {
        calcu;
        send;
    }
}
shut client;