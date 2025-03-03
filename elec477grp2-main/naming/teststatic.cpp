
#include <iostream>

void f1(){
   static int s = 0;
   s++;
}

void f2(){
   static int s = 0;
   printf("s = %d\n", s);
}

int main() {
   f1();
   f2();
}
