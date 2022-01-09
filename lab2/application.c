#include "application.h"


int parseArgs(int argc, char **argv)
{
    if (argc != 2)
    {
        return -1;
    }
    signal(SIGSEGV, segmentation_fault_handler);

    char *ret = strstr(argv[1], "@");
    if (ret == NULL)
    {
        // No user and pass provided
        // ftp://<host>/<url-path>
        // removedHeader will contain <host>/<url-path>
        char *removedHeader = (char *)malloc(strlen(argv[1]) - 6);
        memcpy(removedHeader, &argv[1][6], strlen(argv[1]));

        application_params.host = strtok(removedHeader, "/");
        application_params.url_path = strtok(NULL, ";");
        application_params.user = "anonymous";
        application_params.pass = "anonymous";
    }
    else
    {
        // header will contain ftp://[<user>:<password>@]
        char *header = strtok(argv[1], "@");

        application_params.host = strtok(NULL, "/");
        application_params.url_path = strtok(NULL, ";");

        // user_pass will contain <user>:<passw ord>
        char *user_pass = (char *)malloc(strlen(header) - 6);
        memcpy(user_pass, &header[6], strlen(header));

        application_params.user = strtok(user_pass, ":");
        application_params.pass = strtok(NULL, ":");
    }
    if (application_params.host == NULL || application_params.url_path == NULL)
    {
        return -1;
    }
    application_params.filename = basename(application_params.url_path);
    char msg[MAX_SIZE];
    sprintf(msg, "\n\tuser = %s\n\tpass = %s\n\thost = %s\n\turl path = %s\n\tfile name = %s\n",
            application_params.user,
            application_params.pass,
            application_params.host,
            application_params.url_path,
            application_params.filename);
    logInfo(msg);
    return 0;
}

struct hostent *getip(char *host)
{
    struct hostent *h;

    /**
     * The struct hostent (host entry) with its terms documented

        struct hostent {
            char *h_name;    // Official name of the host.
            char **h_aliases;    // A NULL-terminated array of alternate names for the host.
            int h_addrtype;    // The type of address being returned; usually AF_INET.
            int h_length;    // The length of the address in bytes.
            char **h_addr_list;    // A zero-terminated array of network addresses for the host.
            // Host addresses are in Network Byte Order.
        };

        #define h_addr h_addr_list[0]	The first address in h_addr_list.
    */
    if ((h = gethostbyname(host)) == NULL)
    {
        logError("gethostbyname()");
        exit(-1);
    }

    char msg[MAX_SIZE];
    sprintf(msg, "\n\tHost name  : %s\n\tIP Address : %s\n", h->h_name, inet_ntoa(*((struct in_addr *)h->h_addr)));
    logInfo(msg);

    return h;
}

void segmentation_fault_handler()
{
    logUsage();
    exit(-1);
}

//  type = 0 for user, type = 1 for pass
void sendCredentials(int sockfd, int type)
{
    char buff[MAX_SIZE];
    if (type == 0)
    {
        sprintf(buff, "user %s", application_params.user);
    }
    else
    {
        sprintf(buff, "pass %s", application_params.pass);
    }
    printf(">>>Client:\n%s\n", buff);
    write(sockfd, buff, strlen(buff));
    write(sockfd, "\n", 1);
}
void sendRetr(int sockfd)
{
    char buff[MAX_SIZE];
    sprintf(buff, "retr %s", application_params.url_path);
    printf(">>>Client:\n%s\n", buff);
    write(sockfd, buff, strlen(buff));
    write(sockfd, "\n", 1);
}

int download(int sockfd2)
{
    char buff[2];
    int res;
    FILE * fileFp = fopen(application_params.filename, "wb+");

    printf(">>>Client 2 is Downloading the file:\n");
    res = read(sockfd2, buff, 1);
    fwrite(buff, 1, 1, fileFp);
    while (res > 0)
    {
        res = read(sockfd2, buff, 1);
        fwrite(buff, 1, 1, fileFp);

    }
    fclose(fileFp);
    return 0;
}

int passiveModeRequest(int sockfd)
{
    char ip_port[MAX_SIZE];
    int res;
    char buff[2];
    printf("Client:\npasv\n");
    write(sockfd, "pasv", 4);
    write(sockfd, "\n", 1);
    res = read(sockfd, buff, 1);
    printf("%c", buff[0]);
    while (res > 0)
    {
        res = read(sockfd, buff, 1);
        printf("%c", buff[0]);
        if (buff[0] == '\n')
        {
            break;
        }
        if (buff[0] == '(')
        {
            int index = 0;
            while (res > 0)
            {
                res = read(sockfd, buff, 1);
                printf("%c", buff[0]);
                if (buff[0] == ')')
                {
                    break;
                }
                ip_port[index] = buff[0];
                index++;
            }
        }
    }
    // Parse the and calculate port number
    int a, b;
    char *temp = strtok(ip_port, ",");

    for (int i = 0; i < 5; i++)
    {
        temp = strtok(NULL, ",");

        if (i == 3)
        {
            a = atoi(temp);
        }
        if (i == 4)
        {
            b = atoi(temp);
        }
    }
    return a * 256 + b;
}

int quit(int sockfd){
    write(sockfd, "quit", 4);
    write(sockfd, "\n", 1);
    int status = getServerResponse(sockfd, 2);
    return status;
}

int getServerResponse(int sockfd, int linesN)
{
    char buff[2];
    int status = 0;
    int res;
    printf(">>>Server Response:\n");
    for (int i = 0; i < 3; i++)
    {
        res = read(sockfd, buff, 1);
        printf("%c", buff[0]);
        status = status * 10 + atoi(&buff[0]);
    }
    // printf("Status code: %d\n", status);
    int count = 0;
    while (res > 0)
    {
        res = read(sockfd, buff, 1);
        printf("%c", buff[0]);
        if (buff[0] == '\n')
        {
            count++;
        }
        if (count == linesN)
        {
            break;
        }
    }
    return status;
}

// int getServer

int main(int argc, char **argv)
{

    if (parseArgs(argc, argv) < 0)
    {
        logUsage();
        exit(-1);
    }
    struct hostent *h = getip(application_params.host);

    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr))); /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(FTP_PORT);                                             /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        logError("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(sockfd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        logError("connect()");
        exit(-1);
    }

    // Server response after connection
    if (getServerResponse(sockfd, 10) != 220)
    {
        logError("getServerResponse() Failed! Unable to connect to server\n");
        exit(-1);
    }
    // Send User
    sendCredentials(sockfd, USER_TYPE);

    // Server response after sending user
    if (getServerResponse(sockfd, 1) != 331)
    {
        logError("getServerResponse() Failed! Wrong user provided\n");

        exit(-1);
    }

    // Send Pass
    sendCredentials(sockfd, PASS_TYPE);
    if (getServerResponse(sockfd, 1) != 230)
    {
        logError("getServerResponse() Failed! Wrong password provided\n");
        exit(-1);
    }
    int portNum = passiveModeRequest(sockfd);
    // printf("portNum = %d\n", portNum);

    int sockfd2;
    struct sockaddr_in server_addr2;

    /*For the second client to receive the file data*/
    bzero((char *)&server_addr2, sizeof(server_addr2));
    server_addr2.sin_family = AF_INET;
    server_addr2.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr))); /*32 bit Internet address network byte ordered*/
    server_addr2.sin_port = htons(portNum);                                              /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        logError("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(sockfd2,
                (struct sockaddr *)&server_addr2,
                sizeof(server_addr2)) < 0)
    {
        logError("connect()");
        exit(-1);
    }

    sendRetr(sockfd);
    download(sockfd2);

    if(quit(sockfd) != 221){
        logError("quit() Failed! Unable to disconnect\n");
        exit(-1);
    }

    if (close(sockfd)<0) {
        perror("close()");
        exit(-1);
    }
    if (close(sockfd2)<0) {
        perror("close()");
        exit(-1);
    }

    return 0;
}
