#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 256

void DelNewlineChar(char *str) {
  size_t len;
  int i;
  if (str == NULL) {
    return;
  }
  len = strlen(str);
  for (i = 0; i < len; ++i) {
    if (str[i] == '\n') {
      str[i] = '\0';
      break;
    }
  }
  return;
}

int AnalysisBigIntString(char *str, int *sign, char **num_ptr) {
  size_t len;
  int begin = 0;
  int i;
  if (str == NULL || sign == NULL || num_ptr == NULL) {
    return -1;
  }
  len = strlen(str);
  if (len == 0 || ((len == 1) && (str[0] > '9' || str[0] < '0'))) {
    return -1;
  }
  *sign = 1;
  if (str[0] == '+') {
    begin = 1;
  } else if (str[0] == '-') {
    *sign = -1;
    begin = 1;
  }
  while (begin < len) {
    if (str[begin] != '0') {
      break;
    }
    begin++;
  }
  if (begin == len) {
    begin = len - 1;
  }
  for (i = begin; i < len; ++i) {
    if (str[i] > '9' || str[i] < '0') {
      return -1;
    }
  }
  *num_ptr = str + begin;
  return 0;
}

void BigIntMultiplication(
    char *num_str1, char *num_str2, int sign, char *result) {
  int i, j, m, n;
  int temp_result[2*MAX_LEN] = {0};
  if (num_str1 == NULL || num_str2 == NULL 
      || sign == 0 || result == NULL) {
    return;
  }
  n = strlen(num_str1);
  m = strlen(num_str2);
  for (i = n; i > 0; --i) {
    int temp1 = num_str1[i-1] - '0';
    for (j = m; j > 0; --j) {
      int temp2 = num_str2[j-1] - '0';
      temp_result[i+j-1] += temp1 * temp2;
    }
  }
  for (i = n+m-1; i >= 0; --i) {
    int temp = temp_result[i];
    temp_result[i] = 0;
    for (j = 0; temp > 0; ++j) {
      temp_result[i-j] += temp % 10;
      temp = temp / 10;
    }
  }
  j = 0;
  if (sign < 0) {
    result[j] = '-';
    j = 1;
  }
  for (i = 0; i < n+m-1; ++i) {
    if (temp_result[i] != 0) {
      break;
    }
  }
  while (i < n+m) {
    result[j] = temp_result[i] + '0';
    i++;
    j++;
  }
  result[m+n] = '\0';
  return;
}

int main(int argc, char *argv[]) {
  char num1[MAX_LEN+1];
  char num2[MAX_LEN+1];
  char result[2*MAX_LEN+1];
  char *num1_ptr, *num2_ptr;
  int sign1, sign2;
  int err;
  printf("The max length of digits is %u.\n", MAX_LEN);
  printf("Please input the first integer.\n");
  fgets(num1, MAX_LEN+1, stdin);
  DelNewlineChar(num1);
  err = AnalysisBigIntString(num1, &sign1, &num1_ptr);
  if (err != 0) {
    printf("integer format error!\n");
    exit(1);
  }
  printf("Please input the second integer.\n");
  fgets(num2, MAX_LEN+1, stdin);
  DelNewlineChar(num2);
  err = AnalysisBigIntString(num2, &sign2, &num2_ptr);
  if (err != 0) {
    printf("integer format error!\n");
    exit(1);
  }
  BigIntMultiplication(num1_ptr, num2_ptr, sign1*sign2, result);
  printf("The result is: %s.\n", result);
  return 0;
}
