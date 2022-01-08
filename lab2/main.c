#include "main.h"
// Handles Arguments - ensures they are correct and stores them in app struct, returns -1 if not
struct application_params* ​args_check​(​int​ argc, ​char​ ** argv) {

    struct application_params* app=(struct application_params*)malloc(sizeof(struct application_params));
    ​if​(args_check(argc, argv, &app) < 0) ​
        return​ error_handler(​"Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n"​);
​       printf​(​"Initialized App!\n"​); 
       print_app(app);
​       struct​ ​ftp​ ​ftp​;
​       if​ (argc != 2) ​
            return​ error_handler(​"Wrong number of arguments!\n"​);
 ​      
       char​* ftp = strtok(argv[1], ​"/"​); ​
       char​* url = strtok(​NULL​, ​"/"​); ​
       char​* path = strtok(​NULL​, ​""​);
​// ftp:
​// [<user>:<password>@]<host> ​// <url-path>
​       if​ (​strcmp​(ftp, ​"ftp:"​) != 0) ​
            return​ error_handler(​"Argument Error: protocol name has to be 'ftp'\n"​);
​   char​* user = strtok(url, ​":"​); ​
   char​* password = strtok(​NULL​, ​"@"​);
​// anonymous mode
​   if​ (password == ​NULL​) {
        user = ​"anonymous"​; 
        password = ​""​; ​
        strcpy​(app->host, url);
}
​   else​ ​strcpy​(app->host, strtok(​NULL​, ​""​));
    
​   strcpy​(app->user, user); ​
   strcpy​(app->password, password); 
   ​strcpy​(app->url_path, path);
​// getting file name
​   char​ fullpath[256]; 
   ​strcpy​(fullpath, app->url_path); ​
   char​* token = strtok(fullpath, ​"/"​);
​   while​( token != ​NULL​ ) { ​
        strcpy​(app->url_name, token);
​// <user>
​// <password>>

    token = strtok(​NULL​, ​"/"​); 
    }
​// getting full file path without the name of the file at the end ​if​(​strlen​(app->file_name) > 0) {
​   char​ s[256];
​   strcpy​(s, app->url_path);
​   int​ len = ​strlen​(s);
​   int​ file_name_length = ​strlen​(app->url_name); 
   s[len-file_name_length-1] = ​'\0'​; 
   ​strcpy​(app->url_path, s);


​   if​(​strcmp​(app->url_name, app->url_path) == 0) 
        ​strcpy​(app->file_path,"");
​       return​ app; 
}

 




int​ ​main​(​int​ argc, ​char​ *argv[]) {
​      struct​ ​application​ ​app​;
        //Arguments validation
​    if​((app=args_check(argc, argv)) ==NULL) 
        ​return​ error_handler(​"Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n"​);
​    printf​(​"Initialized App!\n"​); 
    print_app(app);
​    struct​ ​ftp​ ​ftp​;
/* ---------- STEP 1: establish TCP connection ---------- */
​// get IP Address
​   char​ ipAddress[MAX_LENGTH];
​   if​ (getIPAddress(ipAddress, app.host_name) < 0) ​return
        error_handler(​"Error getting IP Address\n"​); 
        ​printf​(​"IP Address: %s\n"​, ipAddress);
​}