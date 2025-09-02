#include <stdio.h>
#include <stdlib.h> // For strtol

int main() {
    const char *str1 = "12345";
    const char *str2 = "0xA_invalid";
    const char *str3 = "   -789xycz";
    const char *str4 = "0";

    char *endptr;
    long result;

    result = strtol(str1, NULL, 10); // Decimal conversion
    printf("'%s' in decimal: %ld\n", str1, result);

    result = strtol(str2, &endptr, 0); // Auto-detected base (hexadecimal)
    printf("'%s' in auto-detected base: %ld (stopped at '%s')\n", str2, result, endptr);

    result = strtol(str3, &endptr, 10); // Decimal conversion with error checking
    printf("'%s' in decimal: %ld (stopped at '%s')\n", str3, result, endptr);


    result = strtol(str4, &endptr, 10);

    printf("%ld", result);
    printf("\nhgbew");
    return 0;
}