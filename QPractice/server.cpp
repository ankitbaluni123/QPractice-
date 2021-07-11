// Server.c
#include<bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include<sys/wait.h>
#define MAXREQ 30
#define MAXQUEUE 5
#define HEADERTYPE 12
#define HEADERLEN 4


using namespace std;

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

map<int, int> users;
map<int, string> text;
map<int, int> engagedUsers;
map<int, vector<questions>> dataset;

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

void writeHeader (int sockfd, msgHeader header) {
	char* char_arr;
	char_arr = &header.type[0];
	write(sockfd, char_arr, 12);

	char_arr = &header.length[0];
	write(sockfd, char_arr, 4);
}

void writeString (int sockfd, string headers, string send) {
	msgHeader header = makeHeader(headers, to_string(send.size()));
	writeHeader(sockfd, header);

	char* char_arr;
	char_arr = &send[0];
	write(sockfd, char_arr, send.size());
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

bool isNumber(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

// 6 digit number
int randQues() {
	srand(time(0));
	int userId = 0;
	for (int i = 0; i < 6; i++) {
		userId *= 10;
		userId += (rand() % 10);
	}
	return userId;
}

string inputString(int consockfd, int userId) {
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

	char val[atoi(len) + 1];
	n = read(consockfd, val, atoi(len));
	while (n <= 0) {
		n = read(consockfd, val, atoi(len));
	}
	s = convertToString(val, atoi(len));
	text[userId] = s;

	cout << "mode:" << mode << "\n";
	cout << "val:" << val << "\n";
	cout << "len:" << atoi(len) << "\n";
	cout << "Text:" << text[userId] << "\n\n";

	return s;
}

vector<string> types {"Cricket", "Hockey",
	"Boxing", "Baseball", "Football"
	"Chess", "Kabaddi", "Icehockey",
	"Volleyball", "Long jump",
	"Swimming", "Shooting",
	"Gymnastics" , "Badminton", "Tennis", "Table Tennis",
	"Squash"};




bool existUser(int userid) {
	for (auto i : users) {
		if (i.first == userid)
			return true;
	}
	return false;
}

bool isEngaged(int userid) {
	for (auto i : engagedUsers) {
		if (i.first == userid || i.second == userid)
			return true;
	}
	return false;
}

void * server(void* consockf) {
	int consockfd = *(int *)consockf;
	free(consockf);
	char mode[12], len[4];
	string soln, expln;
	int n = 0;
	int userid;
	bool firstTime = true;
	while (1) {
		bool modeflag = true;
		memset(mode, 0, MAXREQ);
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

		if (firstTime) {
			string send = "\nSelect Mode:";
			send += "  Press I for Solo practicing..\n";
			send += "  Press G for practicing in Groups..\n";
			send += "  Press A for Administrator Mode\n";	

			writeString(consockfd, "newques", send);

		} else {
			text[userid] = val;
		}

		cout << "mode:" << userid << ": " << mode << "\n";
		cout << "val:" << userid << ": " << val << "\n";
		cout << "len:" << userid << ": " << atoi(len) << "\n";
		cout << "Text:" << text[userid] << "\n\n";

		if (strcmp(mode, "myIdxxxxxxxx") == 0) {
			users.insert({stoi(val), consockfd});
			userid = stoi(val);

			firstTime = false;
		}

		else if (strcmp(mode, "chatxxxxxxxx") == 0) {
			string msgtext = convertToString(val, atoi(len));

			string respid = msgtext.substr(1, 6);
			int sendid = stoi(respid);
			string msg = msgtext.substr(8, msgtext.size() - 1);

			writeString(users[sendid], "chatresp", "\nfrom @" + respid + ":");
			writeString(users[sendid], "chatresp", msg + "\n");
		}

		else if (strcmp(mode, "inputxxxxxxx") == 0) {
			if (strcmp(val, "I") == 0) {
				while (modeflag) {
					string s;
					s += "\nOK Pick a Topic from the following:\n";
					s += "1. Cricket\n";
					s += "2. Hockey\n";
					s += "3. Boxing\n";
					s += "4. Baseball\n";
					s += "5. Football\n";
					s += "6. Chess\n";
					s += "7. Kabaddi\n";
					s += "8. Icehockey\n";
					s += "9. Volleyball\n";
					s += "10. Long Jump\n";
					s += "11. Swimming\n";
					s += "12. Shooting\n";
					s += "13. Gymnastics\n";
					s += "14. Badminton\n";
					s += "15. Tennis\n";
					s += "16. Table Tennis\n";
					s += "17. Squash\n";

					writeString(consockfd, "newques", s);

					string top = inputString(consockfd, userid);
					int topic = stoi(top);

					int quesNo = randQues();
					int total = dataset[topic].size();
					questions ques = dataset[topic][quesNo % total];

					writeString(consockfd, "newques", ques.question);

					soln = ques.answer;
					expln = ques.explanation;

					string resp = inputString(consockfd, userid);
					transform(soln.begin(), soln.end(), soln.begin(), ::tolower);
					transform(resp.begin(), resp.end(), resp.begin(), ::tolower);
					if (soln == resp) {
						s = "Correct. The explanation is given below:";
						writeString(consockfd, "newques", s);
					}
					else {
						s = "\nWrong. The correct answer is:" + soln + "\nThe explanation is given below:";
						writeString(consockfd, "newques", s);
					}

					writeString(consockfd, "newques", expln);

					resp = "\nTo attempt another question press 'n'; press 'q' to quit; press 'r' to return to main menu\n";
					msgHeader header = makeHeader("newques", to_string(resp.size()));
					writeHeader(consockfd, header);

					char* char_arr = &resp[0];
					write(consockfd, char_arr, resp.size());

					string response = inputString(consockfd, userid);

					if (response[0] == 'n') {
						modeflag = true;
					}
					else if (response[0] == 'r') {
						cout << "return\n";
						modeflag = false;

						string send = "Select Mode:";
						send += "  Press I for Solo practicing..\n";
						send += "  Press G for practicing in Groups..\n";
						send += "  Press A for Administrator Mode\n";	

						writeString(consockfd, "newques", send);

					}
					else if (response[0] == 'q') {
						cout << "Quit\n";
						users.erase(userid);

						string status = "khatam";
						writeString(consockfd, "quit", status);

						close(consockfd);
					}
				}
			}

			else if (strcmp(val, "G") == 0) {
				while (modeflag) {
					string ids;
					ids = "\nActive users are:\n";
					for (auto i : users) {
						if (consockfd != i.second) {
							ids += to_string(i.first);
							ids += " ";
						}
					}
					ids += "\nSpecify the userId with whom you want to collaborate:\n";

					writeString(consockfd, "group", ids);

					ids = inputString(consockfd, userid);
					int secondUser = stoi(ids);
					int secondfd = users[secondUser];

					if (!existUser(secondUser) && secondUser == userid) {
						ids = "\nInvalid User Id Specified.\n";
						writeString(consockfd, "group", ids);

					}
					else {
						if (isEngaged(secondUser)) {
							ids = "\nThe Userid is engaged with other user.\n";
							writeString(consockfd, "group", ids);

						}
						else {
							ids = "Request to collaborate from " + to_string(userid);
							ids += "\nPress 'Y' to affirm, 'N' to reject.\n";

							writeString(secondfd, "group", ids);

							string response = "";
							bool quesflag = true;
							while (response != "Y" && response != "N") {
								response = text[secondUser];
							}

							if (response[0] == 'Y') {
								engagedUsers[userid] = secondUser;

								ids += "\nCollaboration establising..\nPlease wait for it to begin.";
								ids += "To send a message: @userId:msg";
								writeString(secondfd, "group", ids);

								ids = "\nCollaboration Established.\n";
								writeString(consockfd, "group", ids);

								while (quesflag) {
									string s;
									s += "\nOK pick a Topic from the following:\n";
									s += "1. Cricket\n";
									s += "2. Hockey\n";
									s += "3. Boxing\n";
									s += "4. Baseball\n";
									s += "5. Football\n";
									s += "6. Chess\n";
									s += "7. Kabaddi\n";
									s += "8. Icehockey\n";
									s += "9. Volleyball\n";
									s += "10. Long Jump\n";
									s += "11. Swimming\n";
									s += "12. Shooting\n";
									s += "13. Gymnastics\n";
									s += "14. Badminton\n";
									s += "15. Tennis\n";
									s += "16. Table Tennis\n";
									s += "17. Squash\n";

									writeString(consockfd, "newques", s);

									string top = inputString(consockfd, userid);
									int topic = stoi(top);

									int quesNo = randQues();
									int total = dataset[topic].size();
									questions ques = dataset[topic][quesNo % total];

									writeString(consockfd, "newques", ques.question);

									s = "\nThe Question is:";
									writeString(secondfd, "newques", s);
									writeString(secondfd, "newques", ques.question);

									soln = ques.answer;
									expln = ques.explanation;

									string resp = inputString(consockfd, userid);
									while (resp[0] == '@') {
										string msg = resp.substr(8, resp.size() - 1);
										writeString(secondfd, "chatresp", "\nfrom @" + resp.substr(1, 6) + ":");

										writeString(secondfd, "newques", msg + "\n");
										resp = inputString(consockfd, userid);
									}

									transform(soln.begin(), soln.end(), soln.begin(), ::tolower);
									transform(resp.begin(), resp.end(), resp.begin(), ::tolower);
									if (soln == resp) {
										s = "Correct. The explanation is given below:";
										writeString(consockfd, "newques", s);

										writeString(secondfd, "newques", s);

									}
									else {
										s = "Wrong. The correct answer is:" + soln + "\nThe explanation is given below:";
										writeString(consockfd, "newques", s);
										writeString(secondfd, "newques", s);
									}

									writeString(consockfd, "newques", expln);
									writeString(secondfd, "newques", expln);

									resp += "\nTo attempt another question press 'n'; press 'q' to quit; press 'r' to return to main menu\n";
									writeString(consockfd, "newques", resp);

									string response = inputString(consockfd, userid);

									if (response[0] == 'n') {
										quesflag = true;
									}
									else if (response[0] == 'r') {
										cout << "return\n";
										quesflag = false;
										modeflag = false;

										string send = "Select Mode:";
										send += "  Press I for Solo practicing..\n";
										send += "  Press G for practicing in Groups..\n";
										send += "  Press A for Administrator Mode\n";	

										writeString(consockfd, "newques", send);

									}
									else if (response[0] == 'q') {
										cout << "Quit\n";
										users.erase(userid);

										string status = "khatam";
										msgHeader header = makeHeader("quit", to_string(status.size()));
										writeHeader(consockfd, header);

										char* char_arr = &status[0];
										write(consockfd, char_arr, status.size());

										close(consockfd);
									}
								}

							}
							else if (response[0] == 'N') {
								ids += "\nCollaboration request successfully rejected.\n";
								msgHeader header = makeHeader("group", to_string(ids.size()));
								writeHeader(secondfd, header);

								char* char_arr;
								char_arr = &ids[0];
								write(secondfd, char_arr, ids.size());

								string resp = "\nCollaboration Not Accepted.";
								resp += "\n\nTo attempt another request press 'n'; press 'q' to quit; press 'r' to return to main menu\n";
								header = makeHeader("newques", to_string(resp.size()));
								writeHeader(consockfd, header);

								char_arr;
								char_arr = &resp[0];
								write(consockfd, char_arr, resp.size());

								string response = inputString(consockfd, userid);

								if (response[0] == 'n') {
									modeflag = true;
								}
								else if (response[0] == 'r') {
									cout << "return\n";
									modeflag = false;
									quesflag = false;

									string send = "Select Mode:";
									send += "  Press I for Solo practicing..\n";
									send += "  Press G for practicing in Groups..\n";
									send += "  Press A for Administrator Mode\n";	

									msgHeader header = makeHeader("newques", to_string(send.size()));
									writeHeader(consockfd, header);

									char* char_arr;
									char_arr = &send[0];
									write(consockfd, char_arr, send.size());
								}
								else if (response[0] == 'q') {
									cout << "Quit\n";
									users.erase(userid);

									string status = "khatam";
									header = makeHeader("quit", to_string(status.size()));
									writeHeader(consockfd, header);

									char* char_arr = &status[0];
									write(consockfd, char_arr, status.size());

									close(consockfd);
								}
							}
						}
					}
				}


			}

			else if (strcmp(val, "A") == 0) {
				while (modeflag) {
					string send = "Enter the question in the Assignment format:\n";
					send += "Question_Topic;Question_Type(0 for MCQ, 1 for Fill in the blanks);Question_test;Question_Answer;Question_Explaination \n";
					string userq, temp, topic, questiontext, answer, explanation, questiontype;

					msgHeader header = makeHeader("newques", to_string(send.size()));
					writeHeader(consockfd, header);

					char* char_arr;
					char_arr = &send[0];
					write(consockfd, char_arr, send.size());

					userq = inputString(consockfd, userid);

					cout << "userinput: " << userq << "\n";
					stringstream s(userq);
					vector<string> vec;
					while (getline(s, temp, ';')) {
						vec.push_back(temp);
					}
					if (vec.size() < 5 || !isNumber(vec[1])) {
						send = "Wrong format: Please enter the question in specified format . \n";

						msgHeader header = makeHeader("newques", to_string(send.size()));
						writeHeader(consockfd, header);
						char* char_arr;
						char_arr = &send[0];
						write(consockfd, char_arr, send.size());

						continue;
					}
					topic = vec[0];
					questiontext = vec[2];
					explanation = vec[4];
					questiontype = vec[1];
					answer = vec[3];

					questions ques;
					ques.question = questiontext;
					ques.type = stoi(questiontype);
					ques.explanation = explanation;
					ques.answer = answer;

					auto it = find(types.begin(), types.end(), topic);
					int ind = it - types.begin();
					if (ind == types.size()) {
						string s = "Wrong topic selected. It should be from one of the following:";
						s += "\nOK pick a Topic from the following:\n";
						s += "1. Cricket\n";
						s += "2. Hockey\n";
						s += "3. Boxing\n";
						s += "4. Baseball\n";
						s += "5. Football\n";
						s += "6. Chess\n";
						s += "7. Kabaddi\n";
						s += "8. Icehockey\n";
						s += "9. Volleyball\n";
						s += "10. Long Jump\n";
						s += "11. Swimming\n";
						s += "12. Shooting\n";
						s += "13. Gymnastics\n";
						s += "14. Badminton\n";
						s += "15. Tennis\n";
						s += "16. Table Tennis\n";
						s += "17. Squash\n";

						writeString(consockfd, "resp", s);
					}
					else {
						dataset[ind + 1].push_back(ques);

						send = "Sucessfully Inputed Question\n";
						writeString(consockfd, "newques", send);
					}

					send = "\nTo enter another question press 'n'; press 'q' to quit; press 'r' to return to main menu\n";
					writeString(consockfd, "newques", send);

					n = read(consockfd, mode, 12);
					while (n <= 0) {
						n = read(consockfd, mode, 12);
					}
					n = read(consockfd, len, 4);
					while (n <= 0) {
						n = read(consockfd, len, 4);
					}

					char val[atoi(len) + 1];
					n = read(consockfd, val, atoi(len));
					while (n <= 0) {
						n = read(consockfd, val, atoi(len));
					}

					if (val[0] == 'n') {
						modeflag = true;
					}
					else if (val[0] == 'r') {
						cout << "return\n";
						modeflag = false;

						string send = "Select Mode:";
						send += "  Press I for Solo practicing..\n";
						send += "  Press G for practicing in Groups..\n";
						send += "  Press A for Administrator Mode\n";	
						writeString(consockfd, "quit", send);

					}
					else if (val[0] == 'q') {
						cout << "Quit\n";
						users.erase(atoi(val));

						send = "khatam";
						writeString(consockfd, "quit", send);

						close(consockfd);
					}
				}

			}
		}

		else if (strcmp(mode, "groupxxxxxxx") == 0) {
			string ids;

			for (auto i : users) {
				if (consockfd != i.second) {
					ids += to_string(i.first);
					ids += " ";
				}    cout << "User left:" << userid;
			}

			msgHeader header = makeHeader("group", to_string(ids.size()));
			writeHeader(consockfd, header);

			char* char_arr;
			char_arr = &ids[0];
			write(consockfd, char_arr, ids.size());

		}
		memset(val, 0, atoi(len));
	}

	close(consockfd);


	return NULL;
}


int main() {

	questions sample_ques;
	sample_ques.question = "This is a sample question.";
	sample_ques.answer = "No Answer";
	sample_ques.explanation = "First go to admin mode and input your questions :-)"; 
	vector<questions> quesarr;
	quesarr.push_back(sample_ques);

	for (int i = 1; i < types.size(); i++) {
		dataset.insert({i, quesarr});
	}

	int lstnsockfd, consockfd, portno = 4440, cstatus, x;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family      = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port        = htons(portno);

	// Server protocol
	/* Create Socket to receive requests*/
	lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Bind socket to port */
	x = bind(lstnsockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	while (x) {
		x = bind(lstnsockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	}
	printf("Bounded to port\n");

	/* Listen for incoming connections */
	x = listen(lstnsockfd, MAXQUEUE);
	while (x) {
		x = listen(lstnsockfd, MAXQUEUE);
	}

	while (1) {
		printf("Listening for incoming connections\n");


		/* Accept incoming connection, obtaining a new socket for it */
		consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,
		                   &clilen);
		printf("Accepted connection\n");

		int *pclient = (int *)malloc(sizeof(int));
		*pclient = consockfd;
		pthread_t t;
		pthread_create(&t, NULL, server, pclient);
	}

	close(lstnsockfd);
	return 0;
}
