#include<stdio.h>

int main() {

  int i = 1000;
  char tmp[10] = {'a','b','c','d','e','f','g','h','i','c'};

  while(i--) {
    printf("tmp: '%s'\n", tmp);
  }
}


