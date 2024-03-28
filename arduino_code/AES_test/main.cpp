#include <iostream>



/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv) {
	unsigned  int m[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};
		printf("%d\n",m[0]<<24);
		int s=m[0]<<24;
		printf("%d\n",s>>24);
	
	return 0;
}
