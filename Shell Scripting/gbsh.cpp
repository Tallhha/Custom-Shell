#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<unistd.h>
#include<string.h>
#include <dirent.h>
#include<fcntl.h>
#include <sys/wait.h> 
#include<sys/types.h>
#include<sys/stat.h>
#include <signal.h> 
using namespace std;

//Function to Convert String to Char Array
char* StringToCharArr(string s){
	char *str = new char[s.size() + 1];
	s.copy(str, s.size() + 1);
	str[s.size()] = '\0';
	return str;
}

// Function to Change Descriptor to Write in File if( > ).
bool filewrite(string input){

	bool redirect = false;
	int  j = 0;
	string fname = "";
	for(j = 0; input[j] != '\0'; j++){
		if(input[j] == '&'){
			break;
		}
		if(input[j] == '>'){
			redirect = true;
			j += 1;
			for(j; input[j] != '\0'; j++){
				if(input[j] == ' '){
					continue;	
				}
				fname += input[j];
			}
			break;
		}
	}

	int old = dup(1);				
	if(redirect){
		
		char* str = StringToCharArr(fname);
		creat(str, S_IRUSR | S_IWUSR);
		int file = open(str, O_WRONLY);
	
		dup2(file,1);
	}
	return redirect;
}

//Part G Functions for Pipes(Single of Multiple).
struct command {
	char **argv;
	void set_c(int s, char** rhs){
		argv = new char*[s];
		int i = 0;
		while(rhs[i] != NULL){
			argv[i] = rhs[i];
			i++;
		}
	argv[i] = NULL;
	}	
};

int part_g2 (int desc1, int desc2, struct command *arr) {

	pid_t pid = fork();
	if (pid == 0) {
		if (desc1 != 0) {
			dup2 (desc1, 0);
		        close (desc1);
      		}
      		if (desc2 != 1) {
         		dup2 (desc2, 1);
         		close (desc2);
      		}
//^^Redirecting cmd values(Input of First, Output of Second^^
	return execvp (arr->argv [0], (char* const *)arr->argv);
	}
	return pid;
}

int part_g1 (int size, struct command *arr) {
  
	int fd [2];
	int i, desc1 = 0;

	for(i = 0; i < size - 1; i++) {
	      pipe (fd);
	      part_g2(desc1, fd[1], arr + i);

	      close(fd[1]);
	      desc1 = fd[0];
 	}
	if (desc1 != 0){
		dup2(desc1, 0);
	}
//^^For Last cmd redirect for output on Shell Terminal^^
	return  execvp (arr[i].argv[0], (char* const*)arr[i].argv);
}

//BONUS
void sigintHandler(int sig_num) { 
    signal(SIGINT, sigintHandler); 
} 

int main(int argc, char *argv[]) {
	
	char host[100], cwd[100], user[100], path[100];
	string input = "";

//Getting host,cwd,user Names for Promt(Part 1a)
	gethostname(host,sizeof(host));
	getcwd(cwd,sizeof(cwd));
	getlogin_r(user,sizeof(user));

//Changing Shell Path to our shell(Part c), (Shell value is displayed somewhere in middle when we call environ.)
	strcpy( path, cwd );
	strcat( path, "/gbsh" );	
	setenv("SHELL", path, 1);

//Bonus (Part (h)
    	signal(SIGINT, sigintHandler); 
	while(true){

		getcwd(cwd,sizeof(cwd));
//Display Prompt
		cout<<user<<"@"<<host<<" "<<cwd<<" ";
		getline(cin,input);

//Store cmd entered in temp for check
		string temp = "";
		for(int i = 0; input[i] != 0; i++){
			if(input[i] == ' '){
				break;
			}
			temp += input[i];
		}

//Ampercent Check(Part f)
		bool amp = false;
		for(int i = 0; input[i] != 0; i++){
			if(input[i] == '&' && ((input[i+1] == ' ') || (input[i+1] == '\0'))){
				amp = true;
				break;
			}
		}
//Pipes Check(Part G)
		bool fw = false;
		for(int i = 0; input[i] != 0; i++){
			if(input[i] == '|' && temp != "environ"){
				fw = true;
				break;
			}
		}
//If Pipes
		if(fw){
			goto write;
		}
//If Ampercent, call fork
		pid_t pid; 		
		if(amp){
			pid = fork();
		}
//Else No fork
		else{
			goto noamp;
		}

		if(pid > 0){
			sleep(0);
		}

		else if(pid == 0){

noamp:
//Part 1C(pwd)
		if(input[0] == 'p' && input[1] == 'w' && input[2] == 'd'){
			int old = dup(1);				
			bool redirect = filewrite(input); //If write in file

			cout<< get_current_dir_name() <<endl;

			if(redirect){	
				dup2(old, 1);	
			}			
			close(old);	
		}
//Part 1D(clear)	
		else if(input == "clear"){
			printf("\e[1;1H\e[2J"); 		
		}
//Part 1B(exit)	
		else if(input == "exit"){
			exit(0);
		}	
//Part 2
//Part (a), LS, Works for multiple given directories.		
		else if(input[0] == 'l' && input[1] == 's'){

			int old = dup(1);				
			bool redirect = filewrite(input); //If write in file
			int  j = 0, length = 0;
			
			if(redirect){
				for(j = 0; input[j] != '\0'; j++){
					if(input[j] == '>'){
						j += 1;
						length += 1;
						for(j; input[j] != '\0'; j++){
							length++;
						}
					break;
					}
				}
			}

			bool two = false;	
			int i = 2;
back:
			string s = "", temp = "";
start:
			while(input[i] != '\0'){
		
				if(input[i] == '&'){
					break;
				}
				if(input[i] == '>'){
					i += length;
					length = 0;
					if(input[i] == '\0'){					
						break;
					}
				}
				if(input[i] == ' '){
					if(input[i+1] == ' '){
						i++;						
						goto start;
					}
					else if(i > 2 && temp != ""){
						i++;					
						goto print;								
					}
				}
				else{				
					s += input[i];
					temp += input[i];
				}	
				i++;
			}
print:
			if(s == ""){
				s = ".";
			}	

			char* str = StringToCharArr(s);
//If Directory Given
  			struct dirent *de;  
  			DIR *dr = opendir(str); 
  		
			if (dr == NULL) { 
//If no directory, Display Current Directory
     				DIR *dr = opendir("."); 
                	        bool test = true;
   	
				while ((de = readdir(dr)) != NULL){ 
//Hides Hidden Directory.
					if(de->d_name[0] != '.'){
						if(strcmp(de->d_name, str) == 0){
        			    			printf("%s  ", de->d_name);
                                    			test = false;
                                		}	 
                               		}
  				}
				cout<<endl;
    				closedir(dr);   
//If directory doesn't exist.
                  	        if(test == true){
					if(two){
						cout<<s<<": "<<endl;
					}
 				      cout<<"Could not open current directory"<<endl;
				 }
 
 			} 
  			else{
				if(two){
					cout<<s<<": "<<endl;
				}	
   				while ((de = readdir(dr)) != NULL){ 
					if(de->d_name[0] != '.'){
        				    printf("%s   ", de->d_name);
                        	        } 
  				}
				cout<<endl;
    				closedir(dr);   
 			}
//If more than one directory is given, displays for all.			
			if(input[i + length] != '\0' && !amp){ 
				two = true;
				goto back;
			}
//redirect cout to terminal if file writing was done.
			if(redirect){	
				dup2(old, 1);	
			}			
			close(old);

		}
//Part (b) CD	
		else if(input[0] == 'c' && input[1] == 'd'){
			
			string s = "";
			bool flag = false;
			bool one = false;
			for(int i = 2; input[i] != '\0'; i++){
				if(input[i] == '&'){
					break;
				}
				if(input[i] == ' '){
					if(one){
//If more than one arguments.
						cout<<"Too many arguments."<<endl;
						flag = true;
						break;	
					}
					else{
						continue;
					}
				}
				else{
					s += input[i];
					one = true;
				}
			}
	
			if(!flag){
	
				char* str = StringToCharArr(s);
				if(s == ""){
					chdir(getenv("HOME"));
  					chdir("..");
				}	
				else{
					char temp[100];
					getcwd(temp,sizeof(temp));

					chdir(str);

					getcwd(cwd,sizeof(cwd));

					if(strcmp(cwd, temp) == 0){
						cout<<"No such file or directory."<<endl;
					}
				}
			}
		}
//Part(c) SETENV	
		else if(temp == "setenv"){

			string s1 = "";
			string s2 = "";
			bool val = false, one = false;
			for(int i = 7; input[i] != '\0'; i++){
				if(input[i] == '&'){
					break;
				}
				if(input[i] == ' ' && input[i+1] == ' '){
					continue;
				}
				if(input[i] == ' ' && one){
					val = true;
				}
				else{
					if(!val){
						if(input[i] == ' ')
							continue;
						s1 += input[i];
						one = true;
					}
					else{
						s2 += input[i];
					}
				}
			}
			
			bool def = false;
			string cmp = "";
			extern char** environ;
			for(int i = 0; environ[i] != NULL; i++){
				for(int k = 0; environ[i][k] != '\0'; k++){
					if(environ[i][k] == '='){
						break;
					}
					cmp += environ[i][k];
				}

				if(cmp == s1){
					cout<<"Variable already defined."<<endl;
					def = true;
					break;
				} 
				cmp = "";
			}
//^^Check if variable already exists.^^	
			if(!def){
	
				char* str1 = StringToCharArr(s1);
				char* str2 = StringToCharArr(s2);
				setenv(str1,str2,0);
			}
		}

//Part(c) UNSETENV
		else if(temp == "unsetenv"){
	
			string s = "";
			for(int i = 9; input[i] != '\0'; i++){
				if(input[i] == '&'){
					break;
				}
				if(input[i] == ' '){
					continue;
				}
				else{
					s += input[i];
				}
			}
	
			bool del = false;
			
			string cmp = "";
			extern char** environ;
			for(int i = 0; environ[i] != NULL; i++){
				for(int k = 0; environ[i][k] != '\0'; k++){
					if(environ[i][k] == '='){
						break;
					}
					cmp += environ[i][k];
				}

				if(cmp == s){
					cout<<"Variable is undefinded."<<endl;
					del = true;
					break;
				} 
				cmp = "";
			}
//^^Check if Variable is undefined.^^
			if(del){

				char* str = StringToCharArr(s);
				unsetenv(str);
			}
		}

//Part(c) ENVIRON
		else if(temp == "environ"){

			int old = dup(1);
			bool redirect = filewrite(input);
//Print environment variables.
	   	        extern char** environ;
			for(int i = 0; environ[i]!=NULL; i++){
				printf("%s\n",environ[i]); 
			}
		
			if(redirect){	
				dup2(old, 1);	
			}			
			close(old);

		}
		
		else{
write: 
//Part(g) IF PIPES			
			if(fw){

				int size = 0;
				for(int i = 0; input[i] != '\0'; i++){
					if(input[i] == '|'){
						size++;
					}
				}
			
				struct command arr[size];
				char* args[5] = {NULL};
				int ind = 0, ind1 = 0;
				string s = "";
//Storing all cmds to call in array.
				for(int i = 0; input[i]!= '\0'; i++){
					if(input[i] == '|'){
						args[ind] = NULL;
						arr[ind1].set_c(ind,args);
						ind1++;
						i++;
						ind = 0;
					}
					else if(input[i] == ' ' && input[i+1] == ' '){
						continue;
					}
					else if(input[i] == ' '){
					
						char* str = StringToCharArr(s);
						args[ind] = str;
						ind++;

						s = "";
					}
					else{
						s += input[i];
						if(input[i+1] == '\0'){

						char* str = StringToCharArr(s);
						args[ind] = str;
						ind++;
						s = "";
						args[ind] = NULL;
						arr[ind1].set_c(ind,args);
						
						break;

						}
					}				
				}

				pid_t pid = fork();
				if(pid > 0){
					wait(NULL);
				}
				else if(pid == 0){
//Calling Pipe Function	
					part_g1(ind1+1, arr);
					exit(0);
				}
			}

//Part(d),(e) I/O Redirection, Fork calls
			else{
				int size = 1;
				for(int i = 0; input[i]!= '\0'; i++){

					if(input[i+1] == '>'){
						break;
					}		
					if(input[i] == ' ' && input[i+1] == ' '){
						continue;
					}
					if(input[i] == ' '){
						size++;
					}		
				}
//^^Number of cmds entered
				char* args[size + 1];
				int ind = 0, i = 0;
				string fname = "", iname = "", s = "";
				bool last = false;

				for(i = 0; input[i]!= '\0'; i++){
					if(input[i] == '&'){
						break;
					}
					if(input[i] == ' ' && input[i+1] == ' '){
						continue;
					}
					if(input[i] == ' '){
			
						char* str = StringToCharArr(s);
						args[ind] = str;
						ind++;
						s = "";
					}				
					else if(input[i] == '>'){
						i += 1;
						last = true;
						for( ; input[i] != '\0'; i++){				
							if((input[i] == ' ' && input[i+1] == ' ') || (input[i] == ' ' && input[i-1] == '>')){
								continue;
							}
							if(input[i] == ' ' && fname != ""){
								break;
							}
							else{
								fname += input[i];
//Storing Output File Name						
							}
						}
					}

					else if(input[i] == '<'){
						i+= 1;
						for( ; input[i] != '\0'; i++){				
							if((input[i] == ' ' && input[i+1] == ' ') || (input[i] == ' ' && input[i-1] == '<')){
								continue;
							}
							if(input[i] == '>'){
								i--;
								break;
							}
							if((input[i] == ' ' && iname != "")){
								
								break;
							}
							else{
								iname += input[i];
//Storing Input File Name						
							}
							last = true;
						}
					}
					else{
						s += input[i];
					}				
				}

				if(!last){
					//cout<<"TEST"<<endl;
					char* str = StringToCharArr(s);	
					args[ind] = str;
					ind++;
					s = "";
				}

				args[ind] = NULL;
				
				pid_t pid2 = fork();	
				if(pid2 > 0) {
					wait(NULL);
				}
				else if (pid2 == 0) {

					char* str1 = StringToCharArr(iname);

					int file1 = open(str1, O_RDONLY);
					dup2(file1,0);

					char* str = StringToCharArr(fname);

					creat(str, S_IRUSR | S_IWUSR);
					int file = open(str, O_WRONLY);

					int old = dup(1);
					dup2(file,1);

					fname = "";
//Exec on all given cmds
					execvp(args[0], args);

					dup2(old, 1);
					close(old);
		
//If wrong Input Display in Tokens.
					s = "";
					for(int i = 0; input[i]!= '\0'; i++){
						if(input[i] == ' '){
							cout<<s<<endl;
							s = "";
						}
						else{
							s += input[i];
						}
					}

					cout<<s<<endl;
					exit(0);
				}

			}
		}
		if(amp)
			exit(0);
		
		}	
	}
}
