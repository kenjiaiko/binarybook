#include <Windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "$packed.exe <password>\n");
		return 1;
	}
	if(IsDebuggerPresent()){
		// 在调试器上运行
		printf("on debugger\n");
		return -1;
	}else{
		// 未在调试器上运行
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