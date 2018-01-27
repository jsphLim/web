//
// Created by yue on 18-1-26.
//

#ifndef YUEWEBSERVER_TASK_H
#define YUEWEBSERVER_TASK_H
#include <cstdio>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/wait.h>

char *loadPath = "/home/yue/CLionProjects/yueWebServer";


class Lwp
{
private:
    int connected;

public:
    Lwp(int id):connected(id){}

    void response(char *msg, int status)  //状态码
    {
        char buffer[512];
        sprintf(buffer, "HTTP/1.1 %d OK\r\nConnection: Close\r\n"  //响应头
                "content-length:%d\r\n\r\n", status, (int)(strlen(msg)));

        sprintf(buffer, "%s%s", buffer, msg);
        write(connected, buffer, strlen(buffer));

    }
    void Fileresponse(int filesize, int status)
    {
        char buffer[256];
        sprintf(buffer, "HTTP/1.1 %d OK\r\nConnection: Close\r\n"
                "content-length:%d\r\n\r\n", status, filesize);
        write(connected, buffer, strlen(buffer));
    }

    void Get(char *filename);

    void Read();
};

void Lwp::Read()
{
    char buf[4096];
    int request;
    read: request = (int)read(connected, buf, 4096);  //读取报文写入buf
    printf("%s", buf);
    if(request > 0)
    {
        char requestMethod[5];
        char file[50];
        int i=0;
        int index=0;
        while(buf[index] != ' ')//获取方法类型（post或者get）
        {
            requestMethod[i++] = buf[index++];
        }
        ++index;
        requestMethod[i] = '\0';
        printf("%s\n",requestMethod);
        i = 0;
        while(buf[index] != ' ' && buf[index] != '\0')//获取文件
        {
            if(buf[index]=='%') break;
            file[i++] = buf[index++];
        }
        file[i] = '\0';
        printf("file:%s\n",file);
        if(strcasecmp(requestMethod, "GET") == 0)  //get
        {
            Get(file);
        }
        else if(strcasecmp(requestMethod, "POST") == 0)  //post
        {
            //TODO
        }
        else  //其他方法
        {
            char error[300];
            sprintf(error, "<html><title>Server Error</title>");
            sprintf(error, "%s<body>\r\n", error);
            sprintf(error, "%s 501\r\n", error);
            sprintf(error, "%s </body>", error);
            response(error, 501);
        }

    }
    else if(request < 0)//读取失败回到重新读取
        goto read;

    sleep(10);
    close(connected);
}

void Lwp::Get(char *filename)
{
    char file[100];
    strcpy(file, loadPath);

    //指引访问到index.html
    if(strcmp(filename, "/") == 0)
        strcat(file, "/index.html");
    else
        strcat(file, filename);


    printf("%s\n", file);
    struct stat statfile;
    int res = stat(file, &statfile);

    if(res < 0 || S_ISDIR(statfile.st_mode)) //读取文件失败
    {
        char error[300];
        sprintf(error, "<html><title>Server Error</title>");
        sprintf(error, "%s<body>\r\n", error);
        sprintf(error, "%s 404\r\n", error);
        sprintf(error, "%s </body>", error);
        response(error, 404);
        return;
    }

        int loadfile = open(file, O_RDONLY);
        Fileresponse(statfile.st_size, 200); //状态骂和请求状态返回
        sendfile(connected, loadfile, 0, statfile.st_size); //
}

#endif //YUEWEBSERVER_TASK_H
