#include <Windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "$packed.exe <password>\n");
		return 1;
	}
	if(IsDebuggerPresent()){
		// デバッガ上で動作している
		printf("on debugger\n");
		return -1;
	}else{
		// デバッガ上で動作していない
		if(strcmp(argv[1], "unpacking") == 0){
			printf("correct!\n");
		}else{
			printf("auth error\n");
			return -1;
		}
	}
	getchar();
	return 0;
}