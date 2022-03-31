//////////////////////////////////////////////////////////////////
// File Name		: proxy_cache.c				//
// Date			: 2022/03/30				//
// Os			: Ubuntu 16.04 LTS 64bits		//
// Author		: Lee Yeon Geol				//
// Student ID		: 2018202076				//
// ------------------------------------------------------------	//
// Title : System Programming Assignment #1-1 (proxy server)	//
// Description : Encrpyt the input by the user. It is a task to //
//		 Create directories and files with encrypted url//
//////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>

//////////////////////////////////////////////////////////////////
// sha1_hash							//
// ============================================================ //
// Input: char* -> input_url,					//
//	  char* -> hashed_url,					//
//	  Descrpition: input_url is the url which is input from //
//		       user and hashed_url is the string pointe //
//		       r in which the encrypted url contains.	//
// Output: char* hashed_url					//
//	   Descrpition: encrypted url				//
// Purpose: To encrypt url using SHA1				//
//////////////////////////////////////////////////////////////////

char *sha1_hash(char *input_url, char *hashed_url) {
	unsigned char hashed_160bits[20];
	char hashed_hex[41];
	int i;
	
	SHA1(input_url, strlen(input_url), hashed_160bits); // Encrypt url to 160 bits
		
	for(i=0; i < sizeof(hashed_160bits); i++) {
		sprintf(hashed_hex + i*2, "%02x", hashed_160bits[i]); // Insert encrypted url into the hashed_hex variable in hexadecimal
	}

	strcpy(hashed_url, hashed_hex); // Copy the encrypted url, which has been changed to a hexadecimal number, to the return value
	return hashed_url;
}

//////////////////////////////////////////////////////////////////
// getHomeDir							//
// ============================================================ //
// Input: char* -> home						//
//	  Descrpition: Empty variables that are expected to con	//
//		       tain Home Directory paths		//
// Output: char* -> home					//
//	   Descrpition: variable containing Home Directory path //				//
// Purpose: To find the Home directory path without using absol //
//          ute paths.						//
//////////////////////////////////////////////////////////////////
char *getHomeDir(char *home) {
	struct passwd *usr_info = getpwuid(getuid()); // Get the current uid and initialize the usr_info field of the structure passwd
	strcpy(home, usr_info->pw_dir); // access the structure field and copy the pwd

	return home;
}


void makeDirAndFile(char* home_dir, char *file_name) {
	char file_name_ow[256]; // overwrite to prevent null terminator from being included in file names

	// Make Directory
	umask(0); // Command to give directory all permissions
	int isDirCreated = mkdir(home_dir, 0777);
	if (isDirCreated == -1) { 
		printf("mkdir Error Occur!\n");
		return;
	}

	// Make File
	strcpy(file_name_ow,file_name);
	strcat(home_dir, "/");
	strcat(home_dir, file_name_ow);

	int isFileCreated = creat(home_dir, 0777);
	if (isFileCreated == -1) {
		printf("creat Error Occur!\n");
		return;
	}
	return;
}


//////////////////////////////////////////////////////////////////
// main								//
// ============================================================ //
// Input: -							//
// Output: int  -   0 success					//
//		    1 error					//
// Purpose: To run the program to encrypt url, to create direct //
// ories and files with encrypted url, and to end the program a //
// fter all work is done. 					//
//////////////////////////////////////////////////////////////////

int main()
{	
	char *input_url;
	char *hashed_url;
	char home_dir[1024];

	char dir_name[256]={"0"};
	char file_name[256]={"0"}; // actual file name

	int hit_cnt = 0, miss_cnt = 0;

	DIR *dir = NULL; // directory pointer declaration
	struct dirent *file = NULL; 


	time_t currentTime = time(NULL); // get Current Time
	struct tm *local_time = localtime(&currentTime);
	if (local_time == NULL) {
		printf("plocal Error Occur!\n");
		return 0;
	}
	
	while(1) {
		input_url = malloc(sizeof(char)* 1024); // dynamic memory allocation
	        hashed_url = malloc(sizeof(char) * 1024);
		
		printf("input url> ");
		scanf("%s", input_url);
		
                if (strcmp(input_url, "bye") == 0) {
                        free(hashed_url); // deallocation
			free(input_url);
			return 0;
                }

		sha1_hash(input_url, hashed_url); // url encryption
		getHomeDir(home_dir); // Get Home Directory Path

		for (int i = 0; i < 3; i++) {
			dir_name[i] = *(hashed_url + i);

		}
		strcat(home_dir, "/cache/");
		strcat(home_dir, dir_name);
		
		// for (int i = 3; (int)*(hashed_url + i) != 0; i++) { // Extracting elements from encrypted url until NULL characters are released
		// 	extracted_url[i-3] = *(hashed_url + i);
		// }
		// strcpy(file_name,extracted_url); // Copy for not including the termination character ('0') in the file name

		for (int i =3; (int)*(hashed_url + i) != 0; i++) {
			file_name[i-3] = *(hashed_url + i);
		}

		dir = opendir(home_dir);
		if (dir == NULL) {
			printf("MISS no dir exist\n");
			// need to change this part to log.
			printf("[MISS] %s - [%d/%d/%d, %d:%d:%d]\n", input_url, 1900+local_time->tm_year, local_time->tm_mon+1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
			makeDirAndFile(home_dir, file_name);
		}
		else {
			for (file = readdir(dir); strlen(file->d_name) < 3; file = readdir(dir)) {
				continue;
			}

			if (strcmp(file -> d_name, file_name) == 0) {
				printf("HIT dir and file exist");
				// need to change this part to log.
				printf("[Hit] %s/ %s - [%d/%d/%d, %d:%d:%d]\n", dir_name, file->d_name, 1900+local_time->tm_year, local_time->tm_mon+1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
				printf("[Hit]%s\n", input_url);
				
			}
			else {
				printf("MISS no file matched");
				// need to change this part to log.
				printf("[MISS] %s - [%d/%d/%d, %d:%d:%d]\n", input_url, 1900+local_time->tm_year, local_time->tm_mon+1, local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
				makeDirAndFile(home_dir, file_name);
			}
		}
		closedir(dir);
	
		free(hashed_url); // memory deallocation
		free(input_url);
	}


	return 0;
}

