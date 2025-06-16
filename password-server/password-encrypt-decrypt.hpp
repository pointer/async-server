#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define KEY1 "stringkey"
#define KEYL 9

char* encrypt(char*);
char* decrypt(char*);
char hexa_byte_to_dec(char, char);
char hexa_values(char);

int encrypt_decrypt() {


    system("clear || cls");
    char opcion;
    char cadena[255];
    int hexa;
    do {

        printf("Encriptar o desencriptar? (1 | 2 | 3 salir) ");
        scanf("%c", &opcion);

        if (opcion == '1') {

            do {
    
                printf("\nCadena: ");
                scanf("%s", cadena);

            }while(strlen(cadena) == 0);

            char* encryption = encrypt(cadena);
            printf("Cadena encriptada a: ");
            for (int i = 0; i < strlen(encryption); i++) {
                printf("%02x", encryption[i]);
            }
            puts("\n");
            free(encryption);
            getchar();


        }
        else if (opcion == '2') {
            
            do {

                printf("\nHexadecimal: ");
                scanf("%s", cadena);

            }while(strlen(cadena) == 0);

            char* decryption;
            decryption = decrypt(cadena);
            printf("Cadena encriptada a: ");
            for (int i = 0; i < strlen(decryption); i++) {
                printf("%c", decryption[i]);
            }
            puts("\n");
            free(decryption);
            getchar();
        }
        else if (opcion != '3') {
            printf("\nOpcion incorrecta.\n");
            getchar();
        }
    

    } while(opcion != '3');


    return 0;
}

char* encrypt(char* str) {

    char* encryption = malloc(sizeof(char) * strlen(str) + 1);

    int i;
    for (i = 0; i < strlen(str); i++) {

        encryption[i] = (str[i] ^ KEY1[i % KEYL]);
    }

    encryption[i] = '\0';

    return encryption;
}

char* decrypt(char* str) {

    char* decryption = malloc(sizeof(char) * strlen(str) + 1);

    int i, k;
    for (i = 0, k = 0; k < strlen(str); i++, k+=2) {
        char temp = hexa_byte_to_dec(str[k], str[k + 1]);
        decryption[i] = temp ^ KEY1[i % KEYL];
    }

    decryption = realloc(decryption, sizeof(char) * (i + 1) + 1);
    decryption[i] = '\0';

    return decryption;
}

char hexa_byte_to_dec(char half1, char half2) {

    return hexa_values(half1) * 16 + hexa_values(half2);
}

char hexa_values(char c) {

    switch (c) 
    {
    case 'a':
        return 10;
        break;
    case 'b':
        return 11;
        break;
    case 'c':
        return 12;
        break;
    case 'd':
        return 13;
        break;
    case 'e':
        return 14;
        break;
    case 'f':
        return 15;
        break;
    default:
        return c - '0';
        break;
    }
}

