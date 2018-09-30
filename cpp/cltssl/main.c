#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAXBUFF (1024*2)

void showCerts(SSL *ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl);
    if (NULL != cert) {
        printf("digital certificate information:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("certificate: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("issuer: %s\n", line);
        free(line);
        X509_free(cert);
    } else {
        printf("no certificate information.\n");
    }
}

int main(int argc, char *argv[])
{
    int i, j, sockfd, len, fd, size;
    char filename[50], sendfn[20];
    struct sockaddr_in dest;
    char buffer[MAXBUFF+1];
    SSL_CTX *ctx;
    SSL *ssl;

    // ssl 库初始化
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (NULL == ctx) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    // 创建一个 socket 用于 tcp 通信
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(errno);
    } else {
        printf("socket created\n");
    }
    // 初始化服务器端（对方）的地址和商品信息
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));
    if (0 == inet_aton(argv[1], (struct in_addr *)&dest.sin_addr.s_addr)) {
        perror(argv[1]);
        exit(errno);
    } else {
        printf("address created\n");
    }
    // 连接服务器
    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) != 0) {
        perror("connect");
        exit(errno);
    } else {
        printf("server connected\n");
    }

    // 基于 ctx 产生一个新的 ssl
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    // 建立 ssl 连接
    if (-1 == SSL_connect(ssl)) {
        ERR_print_errors_fp(stderr);
        exit(1);
    } else {
        printf("connected with %s encryption\n", SSL_get_cipher(ssl));
        showCerts(ssl);
    }
    // 接收用户输入的文件名，并打开文件
    printf("\n please input the filename of you want to load:\n>");
    scanf("%s", filename);
    // 将输入的文件名，去掉路径信息后，发给服务器
    for (j = 0, i = 0; i <= strlen(filename); ++i) {
        if ('/' == filename[i]) {
            j = 0;
            continue;
        } else {
            sendfn[j] = filename[i];
            ++j;
        }
    }
    len = SSL_write(ssl, sendfn, strlen(sendfn));
    if (len < 0) {
        printf("%s message send failure error code %d \n", sendfn, errno);
    }
    printf("send name:%s[%d]\n", sendfn, strlen(sendfn));
    sleep(1);
    
    len = SSL_write(ssl, sendfn, strlen(sendfn));
    if (len < 0) {
        printf("%s message send failure error code %d \n", sendfn, errno);
    }
    printf("send name:%s[%d]\n", sendfn, strlen(sendfn));
    sleep(1);
    
    // 循环发送文件内容到服务器
    bzero(buffer, MAXBUFF);
    i = 0;
    int szcount = 0;
    if (0 && (fd = open(filename, O_RDONLY, 0666)) < 0) {
        perror("open");
        exit(1);
    }
    while (0 && (size = read(fd, buffer, MAXBUFF))) {
        if (size < 0) {
            perror("read");
            exit(1);
        } else {
            printf("[%2d]send:%s\n", i++, buffer);
            szcount += size;
            len = SSL_write(ssl, buffer, size);
            if (len < 0) {
                printf("xx    %s message send failure error code %d \n", sendfn, errno);
                break;
            }
        }
        bzero(buffer, MAXBUFF+1);
    }
    printf("send complete %d.\n", szcount);
    // 关闭连接
    close(fd);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);

    return 0;
}