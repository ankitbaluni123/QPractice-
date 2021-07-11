// Client.c
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<bits/stdc++.h>
#include <arpa/inet.h>

#define MAXIN 50
#define MAXOUT 20
#define HEADERTYPE 12
#define HEADERLEN 4

using namespace std;

bool flag = true;
int userId;
typedef struct Questions {
  string question;
  int type;
  string answer;
  string explanation;
} questions;

typedef struct msgHeaders {
  string type;
  string length;
} msgHeader;

msgHeader makeHeader (string typeOfMsg, string lenOfPayload) {
  msgHeader header;
  header.type = typeOfMsg;
  for (auto i = typeOfMsg.length(); i < HEADERTYPE; i++) {
    header.type += "x";
  }
  header.length = lenOfPayload;
  for (auto i = lenOfPayload.length(); i < HEADERLEN; i++) {
    header.length += "x";
  }
  return header;
}

void writeHeader (
  int sockfd, msgHeader header) {
  char* char_arr;
  char_arr = &header.type[0];
  write(sockfd, char_arr, 12);

  char_arr = &header.length[0];
  write(sockfd, char_arr, 4);
}

string convertToString(char* a, int size)
{
  int i;
  string s = "";
  for (i = 0; i < size; i++) {
    s = s + a[i];
  }
  return s;
}

string inputString(int consockfd) {
  char mode[12], len[4];
  int n;
  string s;

  memset(mode, 0, 12);
  n = read(consockfd, mode, 12);
  while (n <= 0) {
    n = read(consockfd, mode, 12);
  }
  n = read(consockfd, len, 4);
  while (n <= 0) {
    n = read(consockfd, len, 4);
  }

  char val[atoi(len)];
  n = read(consockfd, val, atoi(len));
  while (n <= 0) {
    n = read(consockfd, val, atoi(len));
  }

  s = convertToString(val, atoi(len));
  return s;
}

char *getreq(char *inbuf, int len) {
  /* Get request char stream */
  printf("REQ: ");              /* prompt */
  memset(inbuf, 0, len);        /* clear for good measure */
  return fgets(inbuf, len, stdin); /* read up to a EOL */
}

int invert = 1;
void* read_from_server(void* x) {
  char rcvbuf[MAXOUT];
  int sockfd = *(int*)x;
  while (1) {
    if (invert) {
      string s = inputString(sockfd);
      if (s == "khatam") {
        close(sockfd);
        exit(0);
      }
      cout << s << "\n";
    }
  }
  return NULL;
}

void* write_to_server(void* sock) {
  int sockfd = *(int*)sock;
  while (1) {
    if (invert) {
      string buff;
      getline(cin, buff);
      if (buff[0] == '@') {
        msgHeader header = makeHeader("chat", to_string(buff.size()));
        writeHeader(sockfd, header);
        char* char_arr = &buff[0];
        write(sockfd, char_arr, buff.size());
      } else {
        msgHeader header = makeHeader("input", to_string(buff.size()));
        writeHeader(sockfd, header);
        char* char_arr = &buff[0];
        write(sockfd, char_arr, buff.size());
      }

    }
  }
  return NULL;
}

void client(int sockfd, int userId) {
  pthread_t t;
  int *sock = &sockfd;
  pthread_create(&t, NULL, read_from_server, (void* )sock);
  pthread_t f;
  pthread_create(&f, NULL, write_to_server, (void*)sock);
  pthread_join(t, NULL);
  pthread_join(f, NULL);
  return;
}

// Server address
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr,
                                char *serverIP, int portno) {

  struct hostent *server;

  /* Construct an address for remote server */
  memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  inet_aton(serverIP, &(serv_addr->sin_addr));
  serv_addr->sin_port = htons(portno);

  return server;
}


// 6 digit number~
int genUserId() {
  srand(time(0));
  int userId = 1;
  for (int i = 1; i < 6; i++) {
    userId *= 10;
    userId += (rand() % 9);
  }
  return userId;
}

int main() {
  //Client protocol
  char serverIP[] = "127.0.0.1";
  int sockfd, portno = 4440;
  struct sockaddr_in serv_addr;

  buildServerAddr(&serv_addr, serverIP, portno);

  /* Create a TCP socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  /* Connect to server on port */
  connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  userId = genUserId();

  printf("Welcome %d to QPractice..\n", userId);

  msgHeader header = makeHeader("myId", to_string(6));
  writeHeader(sockfd, header);

  char str[4];
  sprintf( str, "%d", userId );
  write(sockfd, str, strlen(str));

  /* Carry out Client-Server protocol */
  while (flag) {
    client(sockfd, userId);
  }

  header = makeHeader("remove", to_string(6));
  writeHeader(sockfd, header);

  str[4];
  sprintf( str, "%d", userId );
  write(sockfd, str, strlen(str));
  /* Clean up on termination */
  close(sockfd);
  return 0;
}