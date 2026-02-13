#include <stdio.h>

int main(void)
{
    int a;
    printf("enter a number: ");
    fflush(stdin); 
    while (scanf("%d", &a) != 2)
    {
        // input was not a number, ask again:
        printf("enter a number: ");
    }
    printf("You entered %d.\n", a);
}