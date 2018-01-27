#include "head/Lwp.h"
#include "head/pool.h"

int main()
{

    int sock, connection;
    struct sockaddr_in server, client;
    int port = 2525;  //设置
    printf("%d\n",port);
    //服务的的地址 地址是一个结构体 里面有三个参数
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    //INADDR_ANY （0,0,0,0） 代表监听所有
    /**
     *  hton1() host--->network Long
     *  ntoh1() network-->host long
     *  htons() host-->network short
     *  ntohs() network-->host short
     */
    //创建socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    //三个参数分别代表协议（AF_INET： IPV4 AF_INET6：IPV6 服务类型（SOCK——STREAM：服务流/TCP协议 SOCK——UGRAM:数据包 udp协议 ） 协议(基本设为0 代表默认协议即可）
    if(sock < 0)
    {
        printf("socket创建失败\n");
        return 1;
    }

    int ret = bind(sock, (struct sockaddr *)&server, sizeof(server)); //把server的地址分配给sock 第三个参数则是server的长度
    if(ret < 0)
    {
        printf("bind分配失败\n");
        return 1;
    }
    //监听
    ret = listen(sock, 10);
    if(ret < 0)
    {
        printf("listen失败\n");
        return 1;
    }
    //创建线程池
    pool<Lwp> pools(20);
    pools.start();  //启动线程池
    //保持监听
    while(true)
    {
        socklen_t len = sizeof(client);
        connection = accept(sock, (struct sockaddr *)&client, &len);
        Lwp *ta = new Lwp(connection);
        pools.append(ta);
    }
    return 0;
}