#define MAX_LENGTH 255
struct application_params{
    char user[MAX_LENGTH];
    char password[MAX_LENGTH];
    char host[MAX_LENGTH];
    char url_path[MAX_LENGTH];
    char url_name[MAX_LENGTH];
};

struct​ ​ftp​ {
​   int​ control_socket_fd; ​/**< file descriptor to control socket */
​   int​ data_socket_fd; ​/**< file descriptor to data socket */ };
}