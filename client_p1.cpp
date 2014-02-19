/*
 * client.cpp
 *
 *  Created on: Mar 14, 2013
 *      Author: samarth
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<fstream>
#include <string.h>
#include <time.h>
#include<sstream>

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
//
void c_add(int sockfd,string port, string ip)
{

int n,len;
char val[256];
char a;
string title;
string num,add;
string temp;
bzero(val,256);
cout<<"\n Enter the no of RFC to add\n";
cin>>num;
cout<<"\n Enter the Title of the RFC \n";
while(a!='.')
	{
	cin.get(a);
	title=title+a;
	}
//getline(title);
//cout<<"value of title captured"<<title;
//cin>>temp;


add="ADD RFC "+num+" P2P-CI/1.0\nHOST: "+ip+"\nPORT: "+port+"\nTITLE: "+title;

//cout<<"Value being sent"<<endl<<add<<endl;
//len=sizeof(add);
len=strlen(add.c_str());
n=write (sockfd,add.c_str(),len);
bzero(val,256);
n=read(sockfd,val,255);
cout<<endl<<val<<endl;
}//void
//
//
void c_list(int sockfd,string port,string ip)
{

int n;int len;
string list;
list="LIST P2P-CI/1.0\nHOST: "+ip+"\nPORT: "+port+"\n";
char val[1000];
len=strlen(list.c_str());
n=write(sockfd,list.c_str(),len);
bzero(val,1000);
n=read(sockfd,val,1000);
cout<<endl<<val<<endl;
}
//
//
string c_lookup(int sockfd,string port,string ip)
{
struct sockaddr_in serv_addr;
 struct hostent *server;
string get_method;
int n,len,portno,flag=0;
string lookup,num,title;
int host_pos=0,port_pos=0,title_pos=0,data_pos=0;
char a;
string buf,host,po;
cout<<"\n Enter the RFC to Lookup";
cin>>num;
/*cout<<"\n Enter the Title of the RFC";
while(a!='.')
{
cin.get(a);
title=title+a;
}*/

lookup="LOOKUP RFC "+num+" P2P-CI/1.0\nHOST: "+ip+"\nPort: "+port+"\nTitle: "+title;
len=strlen(lookup.c_str());
char val[256];
n=write(sockfd,lookup.c_str(),len);
bzero(val,256);
n=read(sockfd,val,255);
if(strcmp(val,"NO MATCH FOUND")==0){
	cout<<"P2P-CI/1.0 404 Not Found\nDate: Wed, 12 APR 2013 15:12:05 GMT\nOS: UBUNTU 10.4"<<endl;
}
else
{
	buf=val;
	//extract the host and port info
	host_pos=buf.find("HOST");
			port_pos=buf.find("PORT");
			title_pos=buf.find("TITLE");
  host=buf.substr(host_pos+4,port_pos-host_pos-4);
  po=buf.substr(port_pos+4,title_pos-4-port_pos);
  if(po==port)
  {
	  cout<<"P2P-CI/1.0 400 Bad Request\nDate: Wed, 12 APR 2013 15:12:05 GMT\nOS: UBUNTU 10.4"<<endl;
  }
  else
  {
	// connect to this host and port >> Make another SOCKET :(
  portno = atoi(po.c_str());
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0)
          error("ERROR opening socket");
      server = gethostbyname(host.c_str());

      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy((char *)server->h_addr,
           (char *)&serv_addr.sin_addr.s_addr,
           server->h_length);
      serv_addr.sin_port = htons(portno);
      cout<<endl<<"Connecting to: PORT="<<portno<<" HOST="<<host<<endl;
      if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
          {

    	  cout<<"P2P-CI/1.0 400 Bad Request\nDate: Wed, 12 APR 2013 15:12:05 GMT\nOS: UBUNTU 10.4\nData\nPeer Unresponsive"<<endl;
          }

      else
      {
    	  //cout<<"ABLE TO CONNECT"<<endl;
    	  // send a GET to download the RFC
    	  get_method="GET RFC"+num+" P2P-CI/1.0\n"+"Host:"+ip+" OS: UBUNTU 10.04";
    	  n=write(sockfd,get_method.c_str(),strlen(get_method.c_str()));
         // Receive a status reply
    	  bzero(val,256);
    	  n = read(sockfd,val,255);
    	  buf=val;
         if(buf.find("200 OK")!= string::npos)
         {
        	 cout<<endl<<buf<<endl;
        	 data_pos=buf.find("Data:");
        	 buf=buf.substr(data_pos+5,buf.length()-data_pos-5);
        	 //open a new file and store the rfc as a file locally
         fstream new_file;
         new_file.open(num.c_str(),fstream::out);
         new_file<<buf;
         new_file.close();
         cout<<"FILE DOWNLOAD COMPLETE"<<endl;
         flag=1;
         }
         else
         {
        	 cout<<buf<<endl;
         }
    	  // send an exit once done
         close(sockfd);
      }//else connection made

	//send an add msg to the server
}//else match found
}//
if(flag==1)return num;
else return "0";
}//fn end

//
//
void c_exit(int sockfd,string port,string ip)
{

int n;
char val[256];
//"EXIT HOST: 12.12.12.12 PORT: 1234"
string temp;
temp="EXIT HOST: "+ip+"PORT: "+port;
n=write(sockfd,temp.c_str(),100);
bzero(val,256);
//n=read(sockfd,val,255);

}
//
void c_register(int sockfd)
{
	char buffer[256];
	int n=0;
	bzero(buffer,256);

FILE *f;

f=fopen("register.txt","r");

fread(&buffer,sizeof(buffer),1,f);
//printf("This is buffer here =%s",buffer);
n=write(sockfd,buffer,strlen(buffer));
while(!feof(f))
{
// to get the register commands from the register file
n=write(sockfd,buffer,strlen(buffer));
if (n < 0)
error("ERROR writing to socket");
bzero(buffer,256);
fgets(buffer,sizeof(buffer),f);
}
bzero(buffer,256);
n = read(sockfd,buffer,255);
 if (n < 0)
 error("ERROR reading from socket");
printf("\n%s\n",buffer);
 fclose(f);
	/*
	printf("Please enter the message: ");
	    bzero(buffer,256);
	    fgets(buffer,255,stdin);
	    n = write(sockfd,buffer,strlen(buffer));
	    if (n < 0)
	         error("ERROR writing to socket");
	    bzero(buffer,256);
	    n = read(sockfd,buffer,255);
	    if (n < 0)
	         error("ERROR reading from socket");
	    printf("%s\n",buffer);
*/
}

void c_listen(int sd_server)
{
	struct   sockaddr_in clin;
	socklen_t clilen;
	int cl_server,pid,n=0,op=0;
    string buf,num,readin,outgoing;
    int rfc_pos=0,host_pos=0,os_pos=0,ver_pos;
    char buffer[256];
    int flag=1;
    bzero(buffer,256);
while(op!=1)
{
	listen(sd_server,1);
				 clilen=sizeof(clin);
				 cl_server=accept(sd_server,(struct sockaddr *) &clin,&clilen);

	 pid=fork();
	 if(pid==0) // when a child
	 {
            close(sd_server);
		 	 n =read(cl_server,buffer,256);
		 	 buf=buffer;
		 	 // write code to find GET method in the received request
		 	 //"GET RFC"+num+" P2P-CI/1.0\n"+"Host:"+ip+" OS: UBUNTU 10.04";
            if(buf.find("GET")!= string::npos)
            {
            cout<<endl<<buf<<endl;
            rfc_pos=buf.find("RFC");
            host_pos=buf.find("Host");
            os_pos=buf.find("OS:");
            ver_pos=buf.find("P2P");
            num=buf.substr(rfc_pos+3,ver_pos-rfc_pos-4);
            //reply with an 200 OK if u have it
            num=num+".txt";
            //cout<<"Received a GET method for File"<<num<<endl;
            ifstream my_file(num.c_str());
            // send the whole file
            if(my_file.good())
            {
            	outgoing="P2P-CI/1.0 200 OK\n Date: Wed, 12 APR 2013 15:12:05 GMT\nOS: UBUNTU 10.4\nContent-Length:255\nContent-Type: text/text \nData:";

            while(my_file>>readin)
            {
            
            outgoing+=(readin+" ");
            }//while
            n=write(cl_server,outgoing.c_str(),outgoing.length());
            cout<<"FILE TRANSFER COMPLETE"<<endl;
            }//if
            // else Send 404 NOT FOUND
            else
            {
            outgoing="P2P-CI/1.0 404 Not Found\nDate: Wed, 12 APR 2013 15:12:05 GMT\nOS: UBUNTU 10.4";
            n=write(cl_server,outgoing.c_str(),outgoing.length());
            }

            }
            // else send 400 BAD REQUEST
            else
            {
            outgoing="P2P-CI/1.0 400 Bad Request\nDate: Wed, 12 APR 2013 15:12:05 GMT\nOS: UBUNTU 10.4";
            n=write(cl_server,outgoing.c_str(),outgoing.length());
            }//else

            exit(0);
	 }//if child
	 else
	 {
		 close(cl_server);
	 }
	 cout<<endl<<"Enter 1 to Exit listen mode"<<endl;
	 cin>>op;
}//while

}//void

void fin_con(int sockfd)
{
	close(sockfd);
}

int reg_file(int port, string ip)
{

fstream reg;
reg.open("register.txt", fstream::out);
reg<<"Register \n"<<"Host: "<<ip<<endl<<"Port: "<<port<<endl;
reg.close();
return 0;
}

int main(int argc, char *argv[])
{
string ip,num;
//finding the ip address of the localhost
FILE * fp = popen("ifconfig", "r");
        if (fp) {
                char *p=NULL, *e; size_t n;
                while ((getline(&p, &n, fp) > 0) && p) {
                   if (p = strstr(p, "inet addr:")) {
                        p+=10;
                        if (e = strchr(p, ' ')) {
                             *e='\0';
                        ip=p;
                        }
                   }
                }
        }
        pclose(fp);
// ip has the ip address of the localhost and can be assigned
    time_t timer;
    timer=time(0);
    struct tm *tm;
    tm=localtime(&timer);

    int sockfd, portno, n,options=0;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    // creating client socket for peer server
    int portno2;

portno2=6000+(tm->tm_sec+tm->tm_min);
struct   sockaddr_in sin,clin;
socklen_t clilen;
int addrlen,port,sd_server;
addrlen=sizeof(sin);
memset(&sin, 0, sizeof(sin));
sin.sin_family = AF_INET;
sin.sin_addr.s_addr=INADDR_ANY;
sin.sin_port=htons(portno2); // so that the kernel reserves a unique port for us
sd_server = socket(PF_INET, SOCK_STREAM, 0);
bind(sd_server, (struct sockaddr *) &sin, sizeof(sin));
//getsockname(sd_server,(struct sockaddr*)&sin,&addrlen);
port=ntohs(sin.sin_port);
// port has the port value used by this client
stringstream out;
out<<port;
string p;
p=out.str();
// creating the register command with the required values

reg_file(port,ip);

// creating normal socket for hearing the server

    string agm1,agm2;
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"Standard arguments taken, Host=localhost, Server Port= 7734");
      // exit(0);
       agm1="localhost";
       agm2="7734";
    }
    else{
    agm1=argv[1];
    agm2=argv[2];}
    //DEFAULT values for arguments
    portno = atoi(agm2.c_str());
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(agm1.c_str());
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
   c_register(sockfd);
   options=0;
   while(options<6)
      {

   cout<<"\n Enter 1-ADD 2-LIST 3-LOOKUP 4-LISTEN 5-EXIT";
   cin>>options;

	   if(options==1)c_add(sockfd,p,ip);
	   if(options==2)c_list(sockfd,p,ip);
	   if(options==3){num=c_lookup(sockfd,p,ip);}
	   if(options==4){c_listen(sd_server);}
	   if(options==5){c_exit(sockfd,p,ip);
	   fin_con(sockfd);options=6;}

   }
    return 0;
}

