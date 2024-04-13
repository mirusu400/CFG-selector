void swap(char **p, char **q){
  char* t = *p; 
       *p = *q; 
       *q = t;
}
int test(int a1, int a2) {
      return a1 + a2;
}
int main(){
      char a1, b1; 
      char *a = &a1;
      char *b = &b1;
      swap(&a,&b);
}
