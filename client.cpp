#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <single_include/nlohmann/json.hpp>
#include <iostream>
#include <vector>

using json = nlohmann::json;
using namespace std;

// #define HOST 
#define PORT 8080

int main(int argc, char *argv[])
{
    char* message;
    char* response;
    char HOST[14] = "34.254.242.81";
    char* token = NULL;
    char* cookie = NULL;
    int sockfd;

    string command;
    bool command_ok = false;

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    while (1) {
        command_ok = false; //check if command is valid
        getline(cin, command);
        string username, password;
        json payload;
        if (strcmp(command.c_str(), "register") == 0) {
            command_ok = true;

            if (cookie != NULL) {
                printf("Already logged in\n");
                continue;
            }

            printf("username=");
            //read username
            getline(cin, username);
            if (username.size() == 0) {
                printf("Invalid username\n");
                continue;
            }
            if (username.find(' ') != string::npos) {
                printf("Invalid username\n");
                continue;
            }

            printf("password=");
            getline(cin, password);
            if (password.size() == 0) {
                printf("Invalid password\n");
                continue;
            }
            if (password.find(' ') != string::npos) {
                printf("Invalid password\n");
                continue;
            }
            
            //create payload
            
            payload["username"] = username;
            payload["password"] = password;

            open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request(HOST, (char*) "/api/v1/tema/auth/register", 
            (char*) "application/json", payload.dump(), NULL, 0);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //check response
            if (strstr(response, "HTTP/1.1 201 Created") != NULL) {
                printf("OK \n");
            }
            else {
                printf("User already exists\n");
            }
            close_connection(sockfd);
        }
        if (strcmp (command.c_str() , "login") == 0) {
            if (cookie != NULL) {
                printf("Already logged in\n");
                continue;
            }
            command_ok = true;

            printf("username=");
            getline(cin, username);

            if (username.size() == 0) {
                printf("Invalid username\n");
                continue;
            }
            if (username.find(' ') != string::npos) {
                printf("Invalid username\n");
                continue;
            }

            printf("password=");
            getline(cin, password);

            if (password.size() == 0) {
                printf("Invalid password\n");
                continue;
            }
            if (password.find(' ') != string::npos) {
                printf("Invalid password\n");
                continue;
            }

            payload["username"] = username;
            payload["password"] = password;

            open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request(HOST, (char*) "/api/v1/tema/auth/login",
            (char*) "application/json", payload.dump(), NULL, 0);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("OK \n");
                //take cookie
                cookie = strstr(response, "connect.sid");
                cookie += 12;
                char* aux = cookie;
                while (*aux != ';') {
                    aux++;
                }
                *aux = '\0';
              
            }
            else {
                printf("Username or password incorrect\n");
            }
            close_connection(sockfd);
        } 
        if (strcmp(command.c_str(), "enter_library") == 0) {
            if (cookie == NULL) {
                printf("Not logged in\n");
                continue;
            }
            if (token != NULL) {
                printf("Already entered\n");
                continue;
            }

            command_ok = true;

            open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, (char *)"/api/v1/tema/library/access", NULL, NULL, cookie);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            //search for token
            token = strstr(response, "token");
            token += 8;
            char* aux = token;
            while (*aux != '"') {
                aux++;
            }
            *aux = '\0';
            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("OK \n");
            }
            close_connection(sockfd);
        }
        if (strcmp(command.c_str(), "get_books") == 0){
            if (cookie == NULL) {
                printf("Not logged in\n");
                continue;
            }
            if (token == NULL) {
                printf("Not authorized, enter library first\n");
                continue;
            }

            command_ok = true;
            
            open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, (char *)"/api/v1/tema/library/books", NULL, token, cookie);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("OK \n");

                response = strstr(response, "[");
                char* aux = response;
                while (*aux != ']') {
                    aux++;
                }
                aux++;
                *aux = '\0';
                
                json j = json::parse(response);
                for (json::iterator it = j.begin(); it != j.end(); ++it) {
                    printf("id=%s\n", it.value()["id"].dump().c_str());
                    printf("title=%s\n", it.value()["title"].dump().c_str());
                    printf("\n");
                }
                if (j.size() == 0) {
                    printf("No books in library\n");
                }

            } else {
                printf("Not authorized, enter library first\n");
            }
            close_connection(sockfd);

        }
        if (strcmp(command.c_str(), "get_book") == 0) {
            if (cookie == NULL) {
                printf("Not logged in\n");
                continue;
            }
            if (token == NULL) {
                printf("Not authorized, enter library first\n");
                continue;
            }

            command_ok = true;

            string id;
            printf("id=");
            getline(cin, id);
            if (id.size() == 0) {
                printf("Invalid id\n");
                continue;
            }
            if (id.find_first_not_of("0123456789") != string::npos) {
                printf("Invalid id\n");
                continue;
            }

            open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, (char *)"/api/v1/tema/library/books/", (char*) id.c_str(), token, cookie);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("OK \n");
                response = strstr(response, "{");
                string aux = "[";
                aux += response;
                aux += "]";
                response = (char*) aux.c_str();

                json j = json::parse(response);
                for (json::iterator it = j.begin(); it != j.end(); ++it) {
                    printf("id=%s\n", it.value()["id"].dump().c_str());
                    printf("title=%s\n", it.value()["title"].dump().c_str());
                    printf("author=%s\n", it.value()["author"].dump().c_str());
                    printf("genre=%s\n", it.value()["genre"].dump().c_str());
                    printf("publisher=%s\n", it.value()["publisher"].dump().c_str());
                    printf("page_count=%s\n", it.value()["page_count"].dump().c_str());
                    printf("\n");
                }
            } else {
                printf("Book not found\n");
            }
            close_connection(sockfd);
        }
        if (strcmp(command.c_str(), "add_book") == 0) {
            if (cookie == NULL) {
                printf("Not logged in\n");
                continue;
            }
            if (token == NULL) {
                printf("Not authorized, enter library first\n");
                continue;
            }

            command_ok = true;

            string title, author, genre, publisher, page_count;
            printf("title=");
            getline(cin, title);
            if (title.size() == 0) {
                printf("Invalid title\n");
                continue;
            }

            printf("author=");
            getline(cin, author);
            if (author.size() == 0) {
                printf("Invalid author\n");
                continue;
            }

            printf("genre=");
            getline(cin, genre);
            if (genre.size() == 0) {
                printf("Invalid genre\n");
                continue;
            }

            printf("publisher=");
            getline(cin, publisher);
            if (publisher.size() == 0) {
                printf("Invalid publisher\n");
                continue;
            }

            printf("page_count=");
            getline(cin, page_count);
            if (page_count.size() == 0) {
                printf("Invalid page_count\n");
                continue;
            }
            if (page_count.find_first_not_of("0123456789") != string::npos) {
                printf("Invalid page_count\n");
                continue;
            }

            payload["title"] = title;
            payload["author"] = author;
            payload["genre"] = genre;
            payload["publisher"] = publisher;
            payload["page_count"] = page_count;

            open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request(HOST, (char*) "/api/v1/tema/library/books",
            (char*) "application/json", payload.dump(), token, cookie);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("OK \n");
            } else {
                printf("ERROR!\n");
            }
            close_connection(sockfd);
            

        }
        if (strcmp(command.c_str(), "delete_book") == 0) {
            if (cookie == NULL) {
                printf("Not logged in\n");
                continue;
            }
            if (token == NULL) {
                printf("Not authorized, enter library first\n");
                continue;
            }
            command_ok = true;

            string id;
            printf("id=");
            getline(cin, id);
            if (id.size() == 0) {
                printf("Invalid id\n");
                continue;
            }
            if (id.find_first_not_of("0123456789") != string::npos) {
                printf("Invalid id\n");
                continue;
            }

            open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_delete_request(HOST, (char *)"/api/v1/tema/library/books/", 
            (char*) id.c_str(), token, cookie);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strstr(response, "OK") != NULL) {
                printf("OK \n");
            } else {
                printf("Book not found\n");
            }
            close_connection(sockfd);

        }
        if (strcmp(command.c_str(), "logout") == 0) {
            if (cookie == NULL) {
                printf("Not logged in\n");
                continue;
            }

            command_ok = true;

            open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, (char*)"/api/v1/tema/auth/logout", NULL, token, cookie);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strstr(response, "HTTP/1.1 200 OK") != NULL) {
                printf("OK \n");
                token = NULL;
                cookie = NULL;
            }
            else {
                printf("ERROR\n");
            }
            close_connection(sockfd);

        }
        if (strcmp(command.c_str(), "exit") == 0) {
            break;
        }
        if (command_ok == false) {
            printf("Invalid command\n");
        }
    }

    close_connection(sockfd);

    return 0;
}

