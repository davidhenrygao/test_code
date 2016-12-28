#include <stdio.h>
#include <math.h>

#define PRECISION 0.001

double origin_try_val(double n) {
  return n/2;
}

double test_sqrt(double n) {
  double result = origin_try_val(n);
  double temp_result;
  do {
    temp_result = result;
    result = (n / temp_result + temp_result) / 2;
  } while ((result-temp_result)>PRECISION 
      || (result-temp_result)<-PRECISION);
  return result;
}

int main(int argc, char *argv[]) {
  double n, result;
  printf("Please input a number to get the square root:\n");
  scanf("%lf", &n);
  result = test_sqrt(n);
  printf("The result is %.3f.\n", result);
  return 0;
}
