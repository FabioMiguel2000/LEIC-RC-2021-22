#include "application.h"

int parseArgs(int argc, char **argv)
{
    if (argc != 2)
    {
        return -1;
    }
    // signal(SIGSEGV, segmentation_fault_handler);

    // char *host, *url_path, *user, *pass;
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
    printf("user = %s\n", application_params.user == NULL? "anonymous": application_params.user);
    printf("pass = %s\n", application_params.pass == NULL? "anonymous": application_params.pass);
    printf("host = %s\n", application_params.host);
    printf("url path = %s\n", application_params.url_path);
    return 0;
}

void segmentation_fault_handler()
{
    printf("Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
    exit(-1);
}

int main(int argc, char **argv)
{

    if (parseArgs(argc, argv) < 0)
    {
        printf("Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(-1);
    }

    return 0;
}
