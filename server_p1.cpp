	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <iostream>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fstream>
	#include <sys/wait.h>



	using namespace std;


	class Linker
	{

	public:
	struct Peer{
	string host;
	string port;
	struct Peer *next;
	};

	struct Rfc
	{
	string num;
	string title;
	string host;
	struct rfc *next;
	};
	struct Peer *proot;
	struct Rfc *rroot;

	void init();
	//void fill_peer(string ,string);
	//void display_peer();
	int serve_peer(int);
	void register_peer(int );

	}; // class

	void Linker :: init()
	{
	proot=NULL;
	rroot=NULL;
	}//void


	void refresh_files()
	{
	fstream ref,ref2,ref3;
	ref.open("server_register.txt", fstream::out);
	ref<<"<H>\n";
	ref.close();
	ref2.open("method_peer.txt", fstream::out);
	ref2<<"<H>\n";
	ref2.close();
	ref3.open("rfc_list.txt", fstream::out);
	ref3<<"<H>\n";
	ref3.close();
	}

	void refresh_methods(string buf,string ip) // buf will contain Method HOST: hostnumber
	{
		fstream ref1,ref2;
		int flag=0;
		string transfer,temp;

			ref1.open("method_peer.txt", fstream::in);
			ref2.open("temp.txt", fstream::out);
         while(ref1>>transfer)
         {
        	 //ref1>>transfer;
        	 temp=transfer;
        	 if(transfer.find(buf)!=string::npos)
        	 {
             ref1>>transfer;
             if(transfer.find(ip)!=string::npos)
             {
            	 flag=1;
             }//if
        	 }//if
        	 if(flag==1)
        	 {
        		 ref2<<"DEL"+temp;
        		 ref2<<transfer;
        		 flag=0;
        	 }
        	 else
        	 {
        		 ref2<<transfer;
        	 }

         }//while
         ref1.close();
         ref2.close();
         ref1.open("temp.txt", fstream::in);
         ref2.open("method_peer.txt",fstream::out);
         while(ref1>>transfer)
         {
        	// ref1>>transfer;
        	 if(transfer.find("DEL")!=string::npos)
        	 {
        		 while(strcmp("<n>",transfer.c_str())!=0)
        		 {
        			 ref1>>transfer;
        		 }
        	 }
        	 else
        	 {
        		 ref2<<transfer;
        	 }//else
         }//while
		ref1.close();
		ref2.close();

	}//void

	void service_add(int sockfd,string buf)
	{
		int n=0;
		int rfc_pos=0,ver_pos=0;
		unsigned host_pos=0,port_pos=0,title_pos=0;
		string rfc_check="RFC",ver_check="P2P",host_check="HOST",port_check="PORT",title_check="TITLE";
		string number;
		string host;
		string port;
		string title;
		string value;
		cout<<endl<<buf<<endl;
	// extract the RFC number , title and host and port info
	rfc_pos=buf.find(rfc_check);
	ver_pos=buf.find(ver_check);
	host_pos=buf.find(host_check);
	port_pos=buf.find(port_check);
	title_pos=buf.find(title_check);
	//cout<<"rfc_pos="<<rfc_pos<<" ver_pos="<<ver_pos<<" host pos="<<host_pos<<" port_pos="<<port_pos<<" title_pos="<<title_pos;
	rfc_pos=rfc_pos+4;

	//-- One extra pos to accommodate ':'
	number=buf.substr(rfc_pos,ver_pos-rfc_pos-1);
	host_pos+=6;
	host=buf.substr(host_pos,port_pos-host_pos);
	port_pos+=6;
	port=buf.substr(port_pos,title_pos-port_pos);
	title_pos+=7;
	title=buf.substr(title_pos,buf.length()-title_pos);
	// save this info in the rfc list file
	fstream rfc;
	rfc.open("rfc_list.txt", fstream::out | fstream::app);
	value="\nRFC \n"+number+"\nHOST \n"+host+"\nPORT \n"+port+"\nTITLE\n"+title+"\n";
	rfc<<value;
	//rfc<<endl<<"RFC \n"<<number<<endl<<"HOST \n"<<host<<endl<<"PORT \n"<<port<<"\nTITLE\n"<<title<<endl;
	rfc.close();
	refresh_methods("ADD",host);
	//send back an acknowledgment
	n=write(sockfd,"RFC added",9);
}//void

void service_list(int sockfd,string buf)
	{
	cout<<endl<<buf<<endl;
	int n;
		string incoming;
		string outgoing;
		string host;
int host_pos,port_pos,flag=0;
//finding the host from the msg
host_pos=buf.find("HOST");
port_pos=buf.find("PORT");
host=buf.substr(host_pos+6,port_pos-host_pos-6);
//*************************
     fstream file;
     file.open("rfc_list.txt",fstream::in);
     // Open file rfc_list and prep info packet
     while(file>>incoming)
     {
//file>>incoming;
if(incoming.find("RFC")!= string::npos)
{
	flag=1;
	outgoing+=incoming+":";
	file>>incoming;
	outgoing+=incoming+"\n";
}//if
if(incoming.find("HOST")!= string::npos)
{
outgoing+=incoming+":";
file>>incoming;
outgoing+=incoming+"\n";
}//if
if(incoming.find("PORT")!= string::npos)
{
outgoing+=incoming+":";
file>>incoming;
outgoing+=incoming+"\n";
}//if
     }//while
     refresh_methods("LIST",host);
// Send info packet to the client
     if(flag==1)
n=write(sockfd,outgoing.c_str(),strlen(outgoing.c_str()));
     else
n=write(sockfd,"EMPTY LIST",12);
}//void


void service_lookup(int sockfd,string buf)
	{
	cout<<endl<<buf<<endl;
	// Extract info abt the RFC num , title
	        int n=0,flag=0,i=0;
			int rfc_pos=0,ver_pos=0;
			int host_pos=0,port_pos=0,title_pos=0;
			string incoming;
			string outgoing;
			string number;
			string host;
			string port;
			string title;
	// extract the RFC number , title and host and port info
		rfc_pos=buf.find("RFC");
		ver_pos=buf.find("P2P");
		host_pos=buf.find("HOST");
		port_pos=buf.find("PORT");
		title_pos=buf.find("TITLE");
		rfc_pos=rfc_pos+4;
	// Search if the rfc exists in the file rfc_list file
		//-- One extra pos to accommodate ':'
			number=buf.substr(rfc_pos,ver_pos-rfc_pos-1); // -1 to remove the space between number and version
			host_pos+=6;
			host=buf.substr(host_pos,port_pos-host_pos);
			port_pos+=6;
			port=buf.substr(port_pos,title_pos-port_pos);
			title_pos+=7;
			title=buf.substr(title_pos,buf.length()-title_pos);
    fstream file;
    file.open("rfc_list.txt", fstream::in);
while(file>>incoming)
{
if(incoming.find("RFC")!=string::npos)
{
	file>>incoming;
if(incoming.find(number.c_str())!=string::npos)
{
outgoing="RFC";
outgoing.append(incoming);
for(int k=0;k<6;k++)
{
	file>>incoming;
	outgoing.append(incoming);
}//for
flag=1;
break;
}//if
}//if
}//while
if (flag==0)outgoing="NO MATCH FOUND";
n=write(sockfd,outgoing.c_str(),outgoing.length());

refresh_methods("LOOKUP",host);
	//send the information to the client

}//void

//**************************************SERVICE_EXIT*************************************
void service_exit(int sockfd,string buf)
{

	int host_pos,port_pos;
	int flag=0;
	string p;
	string host;
	string port;
	string temp,temp1,temp2,temp3,temp4,temp5,temp6,temp7;
	fstream fin,fout,pri;

	fin.open("rfc_list.txt",fstream::in);
	fout.open("rfc_temp.txt",fstream::out);
	host_pos=buf.find("HOST");
    port_pos=buf.find("PORT");
//cout<<buf<<endl<<host_pos<<endl<<port_pos<<endl;
    host=buf.substr(host_pos+6,port_pos-host_pos-7);
    port=buf.substr(port_pos+6,buf.length()-port_pos);
    //"GET RFC"+num+" P2P-CI/1.0\n"+"Host:"+ip+" OS: UBUNTU 10.04";
    cout<<endl<<"EXIT P2P-CI/1.0\nHost:"+host+" Port:"+port+"\n OS: UBUNTU 10.04"<<endl;
//cout<<host+"l"<<endl<<port+"L"<<"\nend\n";
    // now we have the ip and port nos
    // remove entries in the rfc_list for this peer
while(fin>>temp)
{
//fin>>temp;
if(temp.find("RFC")!=string::npos)
{
	//rfc<<endl<<"RFC \n"<<number<<endl<<"HOST \n"<<host<<endl<<"PORT \n"<<port<<"\nTITLE\n"<<title<<endl;
//cout<<"found1"<<endl;
fin>>temp1;
fin>>temp2;
if(temp2.find("HOST")!= string::npos)
{
//cout<<"found2"<<endl;
fin>>temp3;
if(temp3.find(host.c_str())!= string::npos)
{
//cout<<"found3"<<endl;
fin>>temp4;
if(temp4.find("PORT")!= string :: npos)
{
//cout<<"found4"<<endl;
fin>>temp5;
if(temp5.find(port.c_str())!=string :: npos)
{
fin>>temp;
fin>>temp;
}//if temp5
else{
fout<<temp<<endl;
fout<<temp1<<endl;
fout<<temp2<<endl;
fout<<temp3<<endl;
fout<<temp4<<endl;
fout<<temp5<<endl;

}
}//if temp4
else
{
	fout<<temp<<endl;
	fout<<temp1<<endl;
	fout<<temp2<<endl;
	fout<<temp3<<endl;
	fout<<temp4<<endl;

}//else
}//if temp3
else
{
	    fout<<temp<<endl;
		fout<<temp1<<endl;
		fout<<temp2<<endl;
		fout<<temp3<<endl;

}//else
}//if temp2
else
{
	        fout<<temp<<endl;
			fout<<temp1<<endl;
			fout<<temp2<<endl;

}//else
}//if temp
else
{
	            fout<<temp<<endl;

}//else

}//while


fin.close();
fout.close();

fstream rin,rout;
rin.open("rfc_temp.txt",fstream::in);
rout.open("rfc_list.txt",fstream::out);
while(rin>>temp)
{
rout<<temp<<endl;
}//while
rin.close();
rout.close();

/*fstream ge;
ge.open("rfc_list.txt",fstream::in);
while(ge>>p)
{
cout<<p<<endl;
}
ge.close();*/
refresh_methods("EXIT",host);
}//void


	// *******************************ERROR**************************
	void error(const char *msg)
	{
		perror(msg);
		exit(1);
	}
	//**********************************REGISTER PEER*************************************8
	int Linker:: serve_peer(int newsockfd)
	{
	int n=0,i=0;
	char buffer[256];
	string buf;
	int flag=1;
	bzero(buffer,256);
	 n = read(newsockfd,buffer,255);
	//cout<<"Value received :"<<buffer;
	fstream file;
	file.open("method_peer.txt", fstream::app | fstream::out);
	 // Checking the method call sent
	 file<<"<n>\n"<<buffer;
	 file.close();

	 fstream in;
	 in.open("method_peer.txt", fstream::in);
	 while(in>>buf)
	 {
	 //in>>buf;
	 n=buf.find("ADD");
	 if(n!= string::npos)i=1;
	 else
	 {n=buf.find("LOOKUP");
	 if(n!=string::npos)i=2;
	 else
	 {
	 n=buf.find("LIST");
	 if(n!=string::npos)i=3;
	 else
	 {n=buf.find("EXIT");
	 if(n!=string::npos)i=4;
	 }//else
	 }//else

	 }//else
	 if (i!=0)break;
	 }//while
	 in.close();
    // cout<<"Value of selector i="<<i<<endl;
	 if(i==1){cout<<"\n ADD REQUEST RECEIVED \n";
	 service_add(newsockfd,buffer);
	 }
	 if(i==2){cout<<"\n LOOKUP REQUEST RECEIVED \n";
	 service_lookup(newsockfd,buffer);
	 }
	 if(i==3){cout<<"\n LIST REQUEST RECEIVED \n";
	 service_list(newsockfd,buffer);
	 }
	 if(i==4){cout<<"\n EXIT REQUEST RECEIVED";
	 service_exit(newsockfd,buffer);
	}
	 return i;
	}// void

	// ***************************************************DO STUFF************************************
	void Linker::register_peer(int newsockfd)
	{
	int n,i=0;
	char buffer[256];
	string buf;
	int flag=1;
	bzero(buffer,256);
	 n = read(newsockfd,buffer,255);
	  if (n < 0) error("ERROR reading from socket");
	  cout<<endl<<buffer<<endl;
	fstream file;
	file.open("server_register.txt", fstream::app | fstream::out);
	file<<"<n>\n";
	file<<buffer;
	n=write(newsockfd,"Registration successful",23);
	file.close();

	}
	//***************************************INT MAIN*************************************
	int main(int argc, char *argv[])
	{
	static Linker obj;
	int sel=0,status;
	int sockfd, newsockfd, portno,pid;
		 socklen_t clilen;
		 char buffer[256];
		string val;
		refresh_files(); // refreshes all the server files
		 obj.init();
		 struct sockaddr_in serv_addr, cli_addr;
		 int n,flag=0,opt=0;
		 string agm;
		 if (argc < 2) {
			 fprintf(stderr,"Standard Port 7734 taken\n");
			 //exit(1);
			 agm="7734";
		 }
		 else{
		 agm=argv[1];}
		 sockfd = socket(AF_INET, SOCK_STREAM, 0);
		 if (sockfd < 0)
			error("ERROR opening socket");
		 bzero((char *) &serv_addr, sizeof(serv_addr));
		 portno = atoi(agm.c_str());
		 serv_addr.sin_family = AF_INET;
		 serv_addr.sin_addr.s_addr = INADDR_ANY;
		 serv_addr.sin_port = htons(portno);
		 if (bind(sockfd, (struct sockaddr *) &serv_addr,
				  sizeof(serv_addr)) < 0)
				  error("ERROR on binding");
		 listen(sockfd,5);
		 clilen = sizeof(cli_addr);
		 while (1)
		 {

		 newsockfd = accept(sockfd,
					 (struct sockaddr *) &cli_addr,
					 &clilen);
		 if (newsockfd < 0)
			  error("ERROR on accept");
		 pid=fork();
		 if(pid==0)
		 {
		 close(sockfd);
		 obj.register_peer(newsockfd);
		 while(sel!=4){
		 sel=obj.serve_peer(newsockfd);}




	// serves the client till the client exits the connecction

		//
		 exit(0);
		// break;
		 }//if
		 else{

		 //waitpid(pid,NULL,0);
		  close(newsockfd);
		 }//else
		 }//while

		return 0;

	}

