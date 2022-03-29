#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>

char *sha1_hash(char *input_url, char *hashed_url) {
	unsigned char hashed_160bits[20];
	char hashed_hex[41];
	int i;
	
	SHA1(input_url, strlen(input_url), hashed_160bits);
		
	for(i=0; i < sizeof(hashed_160bits); i++) {
		sprintf(hashed_hex + i*2, "%02x", hashed_160bits[i]);
	}
        

	strcpy(hashed_url, hashed_hex);
	
	return hashed_url;
}

char *getHomeDir(char *home) {
	struct passwd *usr_info = getpwuid(getuid());
	strcpy(home, usr_info->pw_dir);

	return home;
}

void main(int argc, char *argv[])
{
	
	char *input_url;
	char *hashed_url;
	char home_dir[1024];
	while(1) {
		input_url = malloc(1024);
	        hashed_url = malloc(1024);

		printf("input url> ");
		scanf("%s", input_url);
		
                if (strcmp(input_url, "Bye") == 0) {
                        break;
                }


		hashed_url = sha1_hash(input_url, hashed_url);
        	
		printf("%s\n", hashed_url);
		
		getHomeDir(home_dir);
		mkdir(home_dir, 777);
        	free(input_url);
	        free(hashed_url);
	}
	return;
}
