#ifndef _PROXYNB_H_
#define _PROXYNB_H_



#include <exception>
#include <fstream>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ctime>
#include <string>
#include <cstring>
#include <unordered_map>
#include <list>
#include <utility>
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>



void proxy_run(int);
void handle_CONNECT(int );

#endif