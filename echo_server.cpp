#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <vector>

using namespace std;

vector<int> cli_socket;

void usage() {
	cout << "echo-server <port> [-e[-b]]" << endl;
	cout << "sample : echo-server 1234 -e -b" << endl;
}

struct Param {
	bool options[2] = {false, false};
	uint16_t port{0};
	string par[2] = {"-e", "-b"};

	bool parse(int argc, char* argv[]) {
		if(argc < 2 || argc > 4) return false;
		else {
			port = atoi(argv[1]);
			for (int i = 2; i < argc; i++) {
				if (strcmp(argv[i], par[i-2].c_str()) == 0)
					options[i-2] = true;
			}
		}
		return port != 0;
	}
}param;

void recvThread(int sd) {
	cout << "connected" << endl;
	static const int BUFSIZE = 65536;
	char buf[BUFSIZE];

	while(true) {
		ssize_t res = recv(sd, buf, BUFSIZE - 1, 0);
		if (res == 0 || res == -1) {
			cerr << "recv return" << res << endl;
			break;
		}
		buf[res] = '\0';
		cout << buf ;
		
		if (param.options[0]) {
			
			if(param.options[1]) {
				vector<int>::iterator itor = cli_socket.begin();
		
    				for (; itor != cli_socket.end(); itor++) {
					res = send(*itor, buf, res, 0);
					if( res == 0 || res == -1) {
						cerr<< "send return " << res << endl;
						break;
					}
				}
        				
			} else {
				res = send(sd, buf, res, 0);
				if( res == 0 || res == -1) {
					cerr << "send return " << res << endl;
					break;
				}
				

			}
			
		}
	}
	cout << "disconnected" << endl;
	close(sd);

}

int main(int argc, char* argv[]) {
	if(!param.parse(argc,argv)) {
		usage();
		return -1;
	}

	int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("socket");
		return -1;
	}

	int res;
	int optval =1;
	res = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (res == -1) {
		perror("setsockopt");
		return -1;
	}
	

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(param.port);

	ssize_t res2 = bind(sd, (struct sockaddr *)&addr, sizeof(addr));
	if (res2 == -1) {
		perror("bind");
		return -1;
	}
	
	res = listen(sd, 5);
	if (res == -1) {
		perror("listen");
		return -1;
	}

	while(true) {
		struct sockaddr_in cli_addr;
		socklen_t len = sizeof(cli_addr);
		
		int cli_sd = accept(sd, (struct sockaddr *)&cli_addr, &len);
		if (cli_sd == -1) {
			perror("accept");
			break;
		}
		
		cli_socket.push_back(cli_sd);
		thread* t = new thread(recvThread, cli_sd);
		t->detach();
	}
	close(sd);

}
						

