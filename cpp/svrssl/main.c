#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAXBUFF (1024*2)

int main(int argc, char *argv[])
{
    printf("hello ssl\n");
    int sockfd = -1, new_fd = -1, fd = -1;
    socklen_t len = 0;
    struct sockaddr_in my_addr, their_addr;
    unsigned int myport, lisnum;
    char buf[MAXBUFF+1] = {0};
    char new_filename[50] = "recvfile/";
    SSL_CTX *ctx;
    mode_t mode;
    char pwd[100];
    char *temp;
    //mkdir("", mode);
    if (argv[1]) {
        myport = atoi(argv[1]);
    } else {
        myport = 7777;
        argv[2] = argv[3] = NULL;
    }
    if (argv[2]) {
        lisnum = atoi(argv[2]);
    } else {
        lisnum = 2;
        argv[3] = NULL;
    }

    // ssl 库初始化
    SSL_library_init();
    // 载入所有 ssl 算法
    OpenSSL_add_all_algorithms();
    // 载入所有 ssl 错误信息
    SSL_load_error_strings();
    // 以 ssl v2 和 v3 标准兼容方式产生一个 SSL_CTX, 即 SSL Content Text
    ctx = SSL_CTX_new(SSLv23_server_method());
    // 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 v2 或 v3 标准
    if (NULL == ctx) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    // 载入用户的数字证书，此证书用来发送给客户端，证书里包含有公钥
    getcwd(pwd, 100);
    if (1 == strlen(pwd)) {
        pwd[0] = '\0';
    }
    if (SSL_CTX_use_certificate_file(ctx, temp=strcat(pwd, "/cert.pem"), SSL_FILETYPE_PEM) < 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    // 载入用户私钥
    getcwd(pwd, 100);
    if (1 == strlen(pwd)) {
        pwd[0] = '\0';
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, temp=strcat(pwd, "/key.pem"), SSL_FILETYPE_PEM) < 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    // 检查用户私钥是否正确
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    // 开启一个 socket 监听
    if (-1 == (sockfd = socket(PF_INET, SOCK_STREAM, 0))) {
        perror("socket");
        exit(1);
    } else {
        printf("socket created\n");
    }
    bzero(&my_addr, sizeof(my_addr));
    bzero(&their_addr, sizeof(their_addr));
    my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(myport);
    if (argv[3]) {
        my_addr.sin_addr.s_addr = inet_addr(argv[3]);
    } else {
        my_addr.sin_addr.s_addr = INADDR_ANY;
    }
    if (-1 == bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))) {
        perror("bind");
        exit(1);
    } else {
        printf("binded\n");
    }
    if (-1 == listen(sockfd, lisnum)) {
        perror("listen");
        exit(1);
    } else {
        printf("begin listen\n");
    }

    while (1) {
        SSL *ssl;
        len = sizeof(struct sockaddr);
        // 等待客户端连上来
        if (-1 == (new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &len))) {
            perror("accept");
            exit(errno);
        } else {
            printf("svr: got connection from %s, port %d, socket %d\n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);
        }
        // 基于 ctx 产生一个新的 ssl
        ssl = SSL_new(ctx);
        // 将连接用户的 socket 加入到 ssl
        SSL_set_fd(ssl, new_fd);
        // 建立 ssl 连接
        if (-1 == SSL_accept(ssl)) {
            perror("ssl accpet");
            close(new_fd);
            break;
        }
        // 接受客户端所传文件的文件名并在特定目录创建空文件
        bzero(buf, MAXBUFF+1);
        bzero(new_filename+9, 42);
        len = SSL_read(ssl, buf, MAXBUFF);
        if (0 == len) {
            printf("receive complete.\n");
        } else if (len < 0) {
            printf("failure to receive message. error code is %d, error message are '%s'\n", errno, strerror(errno));
        }
        printf("%s", new_filename);
        printf("====>(%d)%s\n", len, buf);
        //scanf("%d", len);
        if (0 && (fd = open(strcat(new_filename, buf), O_CREAT|O_TRUNC|O_RDWR, 0666)) < 0) {
            perror("open");
            exit(1);
        }
        // 接收客户端的数据并写入文件中
        int i = 0;
        while (1) {
            bzero(buf, MAXBUFF+1);
            len = SSL_read(ssl, buf, MAXBUFF);
            if (0 == len) {
                printf("receive complete...\n");
                break;
            } else if (len < 0) {
                printf("ffffffailure to receive message error code is %d\n",  errno);
                exit(1);
            } else {
                printf("[%2d]recv(%d):%s\n", i++, len, buf);
                if (0 && write(fd, buf, len) < 0) {
                    perror("write");
                    exit(1);
                }
            }
        }
        // 关闭文件
        close(fd);
        // 关闭 ssl 连接
        SSL_shutdown(ssl);
        // 释放 ssl
        SSL_free(ssl);
        // 关闭 socket
        close(new_fd);
    }
    // 关闭监听的 socket
    close(sockfd);
    // 释放 ctx
    SSL_CTX_free(ctx);
    return 0;
}