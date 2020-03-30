#ifndef SMCOM_COMMON_ITOA_HPP
#define SMCOM_COMMON_ITOA_HPP

#include <string.h>

/*void ltoa(long int n, char *str)
{
  unsigned long i;
  unsigned char j,p;
  i=1000000000L;
  p=0;
  if (n‹0)
  {
    n=-n;
    *str++='-';
  };
  do
  {
    j=(unsigned char) (n/i);
    if (j || p || (i==1))
    {
      *str++=j+'0';
      p=1;
    }
    n%=i;
    i/=10L;
  }
  while (i!=0);
  *str=0;
}*/

void reverse(char s[])
{
  int i, j;
  char c;
  
  for( i = 0, j = strlen(s) - 1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void itoa(int n, char s[])
{
  int i, sign;
  
  if( (sign = n) < 0 ) n = -n;
  i = 0;
  
  do { 
    s[i++] = n % 10 + '0'; 
  } while ( (n /= 10) > 0 );
  
  if( sign < 0 ) s[i++] = '-';
  s[i] = '\0';

  reverse(s);
}

static const char* itoa_hex_digits = "0123456789ABCDEF";

void itoa_hex(uint16_t n, char s[]) {
   for(uint8_t i = 0; i < 4; i++) {
     s[3 - i] = itoa_hex_digits[n & 0xf];
     n >>= 4;
   }
   s[4] = 0;
}

void itoa_hex(uint32_t n, char s[]) {
   for(uint8_t i = 0; i < 8; i++) {
     s[7 - i] = itoa_hex_digits[n & 0xf];
     n >>= 4;
   }
   s[8] = 0;
}

#endif
