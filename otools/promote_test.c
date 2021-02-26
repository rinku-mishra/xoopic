#include <math.h>
#include <stdio.h>

float test_float(float arg) {
  return 2.0 * arg;
}

float test_double(double arg) {
  return 3.0 * arg;
}

float test_float_2(float arg1, float arg2) {
  return arg1 + arg2;
}

float test_double_2(double arg1, double arg2) {
  return arg1 + arg2;
}

main() {
  float (*F)();
  float in,in2,out;
  char result[5];  /* the result string.  We'll print it. */
  double ind,ind2;

  result[0]=result[1]=result[2]=result[3]=result[4]=result[5]=result[6]=result[7]='y';
  result[8]=0;

  /* floating argument to floating function*/
  F = test_float;
  in = 2.0;
  out = F(in);
  /* if we get the incorrect result... */
  if(fabs(out - 4.0) > 1) result[0]='n'; 


  /* double argument to float function*/
  ind = 2.0;
  out = F(ind);
  /* if we get the incorrect result... */
  if(fabs(out - 4.0) > 1) result[1]='n'; 
  

  /* float argument to double function */
  F = test_double;
  in = 2.0;
  out = F(in);
  /* if we get the incorrect result... */
  if(fabs(out - 6.0) > 1) result[2]='n'; 


  /* double argument to double function */
  F = test_double;
  ind = 2.0;
  out = F(ind);
  /* if we get the incorrect result... */
  if(fabs(out - 6.0) > 1) result[3]='n'; 


  /* floating argument to floating function*/
  F = test_float_2;
  in = 2.0;
  in2 = 2.0;
  out = F(in,in2);
  /* if we get the incorrect result... */
  if(fabs(out - 4.0) > 1) result[4]='n'; 


  /* double argument to float function*/
  ind = 2.0;
  ind2 = 2.0;
  out = F(ind,ind2);
  /* if we get the incorrect result... */
  if(fabs(out - 4.0) > 1) result[5]='n'; 
  

  /* float argument to double function */
  F = test_double_2;
  in = 2.0;
  in2 = 4.0;
  out = F(in,in2);
  /* if we get the incorrect result... */
  if(fabs(out - 6.0) > 1) result[6]='n'; 


  /* double argument to double function */
  F = test_double_2;
  ind = 2.0;
  ind2 = 4.0;
  out = F(ind,ind2);
  /* if we get the incorrect result... */
  if(fabs(out - 6.0) > 1) result[7]='n'; 
  printf("\n%s\n",result);

}

