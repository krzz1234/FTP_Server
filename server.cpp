//=======================================================================================================================
//ACTIVE FTP SERVER Start-up Code for Assignment 1 (WinSock 2)

//This code gives parts of the answers away but this implementation is only IPv4-compliant. 
//Remember that the assignment requires a full IPv6-compliant FTP server that can communicate with a built-in FTP client either in Windows 10 or Ubuntu Linux.
//You must change parts of this program to make it IPv6-compliant (replace all data structures and functions that work only with IPv4).

//Hint: The sample TCP server codes show the appropriate data structures and functions that work in both IPv4 and IPv6. 

//=======================================================================================================================

#include <stdio.h> 
#include <string.h>
#include <string>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>


#define DEFAULT_PORT "1234"
#define USE_IPV6 true
#define WSVERS MAKEWORD(2,2)
WSADATA wsadata;

//********************************************************************
//MAIN
//********************************************************************
int main(int argc, char *argv[]) {
//********************************************************************
// INITIALIZATION
//********************************************************************
	     int err = WSAStartup(WSVERS, &wsadata);

		 if (err != 0) {
		      WSACleanup();
			  // Tell the user that we could not find a usable WinsockDLL
		      printf("WSAStartup failed with error: %d\n", err);
		      exit(1);
		 }
		 struct sockaddr_storage localaddr,remoteaddr;  //ipv4 only, needs replacing
		 struct sockaddr_in local_data_addr_act;   //ipv4 only, needs replacing
		 SOCKET s,ns;
		 SOCKET ns_data, s_data_act;
		 char send_buffer[300],receive_buffer[300];
		 char clientHost[NI_MAXHOST]; //IP address
		 char clientService[NI_MAXSERV]; //port address
		 char portNum[NI_MAXSERV];
		 struct addrinfo *result = NULL;
		 struct addrinfo hints;
		 int iResult;
		 bool type_ascii = true;

		
         ns_data=INVALID_SOCKET;

		 int active=0;
		 int n,bytes,addrlen;
		 
		 printf("\n===============================\n");
		 printf("     159.342 FTP Server");
		 printf("\n===============================\n");
		 printf("   submitted by: Mark Lee    ");
		 printf("           date:     ");
		 printf("\n===============================\n");
	
		 
		 memset(&localaddr,0,sizeof(localaddr));//clean up the structure
		 memset(&remoteaddr,0,sizeof(remoteaddr));//clean up the structure
		 
//********************************************************************
//SOCKET
//********************************************************************
		 

		 


		memset(&hints, 0, sizeof(struct addrinfo));

		if(USE_IPV6){
		   hints.ai_family = AF_INET6;  
		}	 else { //IPV4
		   hints.ai_family = AF_INET;
		}	 
		// hints.ai_family = AF_UNSPEC;

		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		if(argc==2){	 
			iResult = getaddrinfo(NULL, argv[1], &hints, &result); //converts human-readable text strings representing hostnames or IP addresses 
			                                                        //into a dynamically allocated linked list of struct addrinfo structures
																	//IPV4 & IPV6-compliant
			sprintf(portNum,"%s", argv[1]);
			printf("\nargv[1] = %s\n", argv[1]); 	
		} else {
		    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result); //converts human-readable text strings representing hostnames or IP addresses 
			                                                            //into a dynamically allocated linked list of struct addrinfo structures
																	    //IPV4 & IPV6-compliant
			sprintf(portNum,"%s", DEFAULT_PORT);
			printf("\nUsing DEFAULT_PORT = %s\n", portNum); 
		}

		if (iResult != 0) {
		    printf("getaddrinfo failed: %d\n", iResult);
		    WSACleanup();
		    return 1;
		}	 

		s = INVALID_SOCKET;

		s = socket(result->ai_family, result->ai_socktype, result->ai_protocol); 
		if (s == INVALID_SOCKET) {
		   printf("Error at socket(): %d\n", WSAGetLastError());
		   freeaddrinfo(result);
		   WSACleanup();
		   exit(1);//return 1;
		}
		 
		 
//********************************************************************
//BIND
//********************************************************************
		iResult = bind( s, result->ai_addr, (int)result->ai_addrlen);
	    if (iResult == SOCKET_ERROR) {
	        printf("bind failed with error: %d\n", WSAGetLastError());
	        freeaddrinfo(result);
			 
	        closesocket(s);
	        WSACleanup();
	        return 1;
	    }
		 
		 freeaddrinfo(result); 
		 
//********************************************************************
//LISTEN
//********************************************************************
		 if (listen( s, SOMAXCONN ) == SOCKET_ERROR ) {
		     printf( "Listen failed with error: %d\n", WSAGetLastError() );
		     closesocket(s);
		     WSACleanup();
		     exit(1);
		   } else {
				printf("\n<<<SERVER>>> is listening at PORT: %s\n", portNum);
			}
		
//********************************************************************
//INFINITE LOOP
//********************************************************************
		 
		 //====================================================================================
		 while (1) {//Start of MAIN LOOP
		 //====================================================================================
			    addrlen = sizeof(remoteaddr);
			    ns = INVALID_SOCKET;
//********************************************************************
//NEW SOCKET newsocket = accept  //CONTROL CONNECTION
//********************************************************************
			 printf("\n------------------------------------------------------------------------\n");
			 printf("SERVER is waiting for an incoming connection request...");
			 printf("\n------------------------------------------------------------------------\n");
			 ns = accept(s,(struct sockaddr *)(&remoteaddr),&addrlen); 
			 if (ns == INVALID_SOCKET) {
				 printf("accept failed: %d\n", WSAGetLastError());
				 closesocket(s);
				 WSACleanup();
				 exit(1); //return 1;
			} else {
				printf("\nA <<<CLIENT>>> has been accepted.\n");
				
				//strcpy(clientHost,inet_ntoa(clientAddress.sin_addr)); //IPV4
				//sprintf(clientService,"%d",ntohs(clientAddress.sin_port)); //IPV4
				
				memset(clientHost, 0, sizeof(clientHost));
				memset(clientService, 0, sizeof(clientService));

		      getnameinfo((struct sockaddr *)&remoteaddr, addrlen,
		                    clientHost, sizeof(clientHost),
		                    clientService, sizeof(clientService),
		                    NI_NUMERICHOST);
				
		      printf("\nConnected to <<<Client>>> with IP address:%s, at Port:%s\n",clientHost, clientService);
			}	
//********************************************************************
//Respond with welcome message
//*******************************************************************
			 sprintf(send_buffer,"220 FTP Server ready. \r\n");
			 bytes = send(ns, send_buffer, strlen(send_buffer), 0);

			//********************************************************************
			//COMMUNICATION LOOP per CLIENT
			//********************************************************************

			 while (1) {
				 
				 n = 0;

				 
				 while (1) {
//********************************************************************
//RECEIVE MESSAGE AND THEN FILTER IT
//********************************************************************
				     bytes = recv(ns, &receive_buffer[n], 1, 0);//receive byte by byte...

					 if ((bytes < 0) || (bytes == 0)) break;
					 
					 if (receive_buffer[n] == '\n') { /*end on a LF*/
						 receive_buffer[n] = '\0';
						 break;
					 }

					 if (receive_buffer[n] != '\r') n++; /*Trim CRs*/
				 	
				 } 
				 
                 if(bytes == 0) printf("\nclient has gracefully exited.\n"); //2022

				 if ((bytes < 0) || (bytes == 0)) break;

				 printf("<< DEBUG INFO. >>: the message from the CLIENT reads: '%s\\r\\n' \n", receive_buffer);

//********************************************************************
//PROCESS COMMANDS/REQUEST FROM USER
//********************************************************************				 
				 if (strncmp(receive_buffer,"USER",4)==0)  {
				 	if (strncmp(receive_buffer,"USER napoleon",13)==0)  {
						 printf("Logging in... \n");
						 sprintf(send_buffer,"331 Password required\r\n");
						 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
						 if (bytes < 0) break;
						}
					else{ break;}
				 }
				 //---
				 if (strncmp(receive_buffer,"PASS",4)==0)  {
				 	if (strncmp(receive_buffer,"PASS 342",8)==0){
					 
						 sprintf(send_buffer,"230 Public login sucessful \r\n");
						 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
						 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
						 if (bytes < 0) break;
					} else { break;}
				 }
				 //---
				 if (strncmp(receive_buffer,"SYST",4)==0)  {
					 printf("Information about the system \n");
					 sprintf(send_buffer,"215 Windows 64-bit\r\n");
					 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
					 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer,"OPTS",4)==0)  {
					 printf("unrecognised command \n");
					 sprintf(send_buffer,"550 command not implemented\r\n");
					 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
					 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 if (bytes < 0) break;
				 }
				 //---
				 if (strncmp(receive_buffer,"QUIT",4)==0)  {
					 printf("Quit \n");
					 sprintf(send_buffer,"221 Connection close by client\r\n");
					 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
					 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 if (bytes < 0) break;
					 // closesocket(ns);
				 }
				 //--
				  if (strncmp(receive_buffer,"TYPE A",6)==0) {
				  	type_ascii = true;
				  	sprintf(send_buffer,"200 Switching to ASCII mode\r\n");
					 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
					 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 if (bytes < 0) break;
				  }
				   if (strncmp(receive_buffer,"TYPE I",6)==0) {
				  	type_ascii = false;
				  	sprintf(send_buffer,"200 Switching to Binary mode\r\n");
					 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
					 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 if (bytes < 0) break;
				  }
				  //--
				 if(strncmp(receive_buffer,"EPRT |2",7)==0){
				 	char *ip[10];
				 	char *token = strtok(receive_buffer, "|");
				 	int i = 0;
				 	while (token != NULL){
				 		printf("%s\n", token);
				 		ip[i] = token;
				 		i++;
				 		token = strtok(NULL, "|");
				 	}
				 	printf("%s,%s\n", ip[2], ip[3]);
				 	sprintf(portNum,"%s", ip[3]);
	   				iResult = getaddrinfo(ip[2], portNum, &hints, &result);
	   				if (iResult != 0) {
						 printf("getaddrinfo failed: %d\n", iResult);
						 WSACleanup();
						 return 1;
				   }	 
	
					s_data_act = socket(result->ai_family, result->ai_socktype, result->ai_protocol); 
					if (s_data_act == INVALID_SOCKET) {
				      printf("socket failed\n");
						freeaddrinfo(result);
						WSACleanup();
				   	exit(1);
				   }
  
					 active=1;
					 if (connect(s_data_act, result->ai_addr, result->ai_addrlen) != 0) {
					 	sprintf(send_buffer, "522 Connect failed\r\n");
					 	bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 	printf("connect failed\n");
						freeaddrinfo(result);
						WSACleanup();
				   		exit(1);
					 }
					 else {
						 sprintf(send_buffer, "200 EPRT Command successful\r\n");
						 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
						 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
						 printf("Connected to client\n");
					 }
				 }
				 //---
				 if(strncmp(receive_buffer,"PORT",4)==0) {
					 s_data_act = socket(AF_INET, SOCK_STREAM, 0);
					 //local variables
					 //unsigned char act_port[2];
					 int act_port[2];
					 int act_ip[4], port_dec;
					 char ip_decimal[40];
					 printf("===================================================\n");
					 printf("\n\tActive FTP mode, the client is listening... \n");
					 active=1;//flag for active connection
					 //int scannedItems = sscanf(receive_buffer, "PORT %d,%d,%d,%d,%d,%d",
					 //		&act_ip[0],&act_ip[1],&act_ip[2],&act_ip[3],
					 //     (int*)&act_port[0],(int*)&act_port[1]);
					 
					 int scannedItems = sscanf(receive_buffer, "PORT %d,%d,%d,%d,%d,%d",
							&act_ip[0],&act_ip[1],&act_ip[2],&act_ip[3],
					      &act_port[0],&act_port[1]);
					 
					 if(scannedItems < 6) {
		         	    sprintf(send_buffer,"501 Syntax error in arguments \r\n");
						printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
						bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					    //if (bytes < 0) break;
			            break;
		             }
					 
					 local_data_addr_act.sin_family=AF_INET;//local_data_addr_act  //ipv4 only
					 sprintf(ip_decimal, "%d.%d.%d.%d", act_ip[0], act_ip[1], act_ip[2],act_ip[3]);
					 printf("\tCLIENT's IP is %s\n",ip_decimal);  //IPv4 format
					 local_data_addr_act.sin_addr.s_addr=inet_addr(ip_decimal);  //ipv4 only
					 port_dec=act_port[0];
					 port_dec=port_dec << 8;
					 port_dec=port_dec+act_port[1];
					 printf("\tCLIENT's Port is %d\n",port_dec);
					 printf("===================================================\n");
					 local_data_addr_act.sin_port=htons(port_dec); //ipv4 only
					 if (connect(s_data_act, (struct sockaddr *)&local_data_addr_act, (int) sizeof(struct sockaddr)) != 0){
						 sprintf(send_buffer, "425 Something is wrong, can't start active connection... \r\n");
						 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
						 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
						 closesocket(s_data_act);
					 }
					 else {
						 sprintf(send_buffer, "200 PORT Command successful\r\n");
						 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
						 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
						 printf("Connected to client\n");
					 }
				 }
				 //--
				 if (strncmp(receive_buffer,"RETR",4)==0){
				 	FILE *fin;
				 	char *str[10];
				 	char *token = strtok(receive_buffer, " ");
				 	int i = 0;
				 	while (token != NULL){
				 		printf("%s\n", token);
				 		str[i] = token;
				 		i++;
				 		token = strtok(NULL, " ");
				 	}
				 	//system("get > tmp.txt");
				   	if(type_ascii == true){
				   		printf("ascii mode\n");
				   		fin=fopen(str[1],"r");//open tmp.txt file
				   		 if (fin == NULL){
						 	sprintf(send_buffer, "522 File not exist\r\n");
						 	 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
						 	printf("File not exist\n");
						 	if (bytes < 0) break;
						 }
					 
				   		sprintf(send_buffer,"150 Opening data connection... \r\n");
						 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
						 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
						 char temp_buffer[80];
						 while (!feof(fin)){
							 fgets(temp_buffer,78,fin);
							 sprintf(send_buffer,"%s",temp_buffer);
							 if (active==0) send(ns_data, send_buffer, strlen(send_buffer), 0);
							 else send(s_data_act, send_buffer, strlen(send_buffer), 0);
						 }
						 fclose(fin);
				   	}
				   	else {
				   		printf("binary mode\n");
				   		fin=fopen(str[1],"rb");
				   		 if (fin == NULL){
						 	sprintf(send_buffer, "522 File not exist\r\n");
						 	 bytes = send(ns, send_buffer, strlen(send_buffer), 0);

						 	printf("File not exist\n");
						 	if (bytes < 0) break;
						 }
					 
				   		sprintf(send_buffer,"150 Opening data connection... \r\n");
						 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
						 bytes = send(ns, send_buffer, strlen(send_buffer), 0);

						size_t n;
						char buff[8192];
						do {
						    n = fread(buff, 1, sizeof buff, fin);
						    if (active==0) bytes = send(ns_data, buff, sizeof buff, 0);
							else bytes = send(s_data_act, buff, sizeof buff, 0);
						} while (n > 0);
						 fclose(fin);
					 }
					
					 //sprintf(send_buffer,"250 File transfer completed... \r\n");
					 sprintf(send_buffer,"226 File transfer complete. \r\n");
					 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
					 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 if (active==0 )closesocket(ns_data);
					 else closesocket(s_data_act);
					}	 
				 //---				 
				 //technically, LIST is different than NLST,but we make them the same here
				 if ( (strncmp(receive_buffer,"LIST",4)==0) || (strncmp(receive_buffer,"NLST",4)==0))   {
					 //system("ls > tmp.txt");//change that to 'dir', so windows can understand
					 system("dir > tmp.txt");
					 FILE *fin=fopen("tmp.txt","r");//open tmp.txt file
					 if (fin == NULL){
					 	sprintf(send_buffer, "522 File not exist\r\n");
					 	printf("File not exist\n");
					 	exit(1);
					 }
					 //sprintf(send_buffer,"125 Transfering... \r\n");
					 sprintf(send_buffer,"150 Opening ASCII mode data connection... \r\n");
					 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
					 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 char temp_buffer[80];
					 while (!feof(fin)){
						 fgets(temp_buffer,78,fin);
						 sprintf(send_buffer,"%s",temp_buffer);
						 if (active==0) send(ns_data, send_buffer, strlen(send_buffer), 0);
						 else send(s_data_act, send_buffer, strlen(send_buffer), 0);
					 }
					 fclose(fin);
					 //sprintf(send_buffer,"250 File transfer completed... \r\n");
					 sprintf(send_buffer,"226 File transfer complete. \r\n");
					 printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
					 bytes = send(ns, send_buffer, strlen(send_buffer), 0);
					 if (active==0 )closesocket(ns_data);
					 else closesocket(s_data_act);
						 
					 
					 //OPTIONAL, delete the temporary file
					 //system("del tmp.txt");
				 }
                 //---			    
			 //=================================================================================	 
			 }//End of COMMUNICATION LOOP per CLIENT
			 //=================================================================================
			 
//********************************************************************
//CLOSE SOCKET
//********************************************************************
			 closesocket(ns);
			 //sprintf(send_buffer, "221 Bye bye, server close the connection ... \r\n");
			 //printf("<< DEBUG INFO. >>: REPLY sent to CLIENT: %s\n", send_buffer);
			 //bytes = send(ns, send_buffer, strlen(send_buffer), 0);
			 printf("\ndisconnected from <<<Client>>> with IP address:%s, Port:%s\n",clientHost, clientService);
			 printf("=============================================");
			 
		 //====================================================================================
		 } //End of MAIN LOOP
		 //====================================================================================
		 
		 printf("\nSERVER SHUTTING DOWN...\n");
		 closesocket(s);
		 WSACleanup();
		 
}

