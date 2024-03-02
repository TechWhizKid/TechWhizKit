#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sha256.h"
#include "aes.h"

// A helper function to read a file into a buffer
uint8_t *read_file(const char *filename, size_t *length)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *buffer = malloc(*length);
    if (buffer == NULL)
    {
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, *length, file);
    fclose(file);
    return buffer;
}

// A helper function to write a buffer into a file
int write_file(const char *filename, uint8_t *buffer, size_t length)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        return -1;
    }
    fwrite(buffer, 1, length, file);
    fclose(file);
    return 0;
}

// A helper function to pad a buffer to a multiple of 16 bytes using PKCS7 padding
uint8_t *pad_buffer(uint8_t *buffer, size_t *length)
{
    size_t padded_length = (*length + 15) / 16 * 16;
    uint8_t *padded_buffer = malloc(padded_length);
    if (padded_buffer == NULL)
    {
        return NULL;
    }
    memcpy(padded_buffer, buffer, *length);
    uint8_t padding = padded_length - *length;
    for (size_t i = *length; i < padded_length; i++)
    {
        padded_buffer[i] = padding;
    }
    *length = padded_length;
    return padded_buffer;
}

// A helper function to unpad a buffer after decryption using PKCS7 padding
uint8_t *unpad_buffer(uint8_t *buffer, size_t *length)
{
    uint8_t padding = buffer[*length - 1];
    if (padding > 16)
    {
        return NULL;
    }
    for (size_t i = *length - padding; i < *length; i++)
    {
        if (buffer[i] != padding)
        {
            return NULL;
        }
    }
    size_t unpadded_length = *length - padding;
    uint8_t *unpadded_buffer = malloc(unpadded_length);
    if (unpadded_buffer == NULL)
    {
        return NULL;
    }
    memcpy(unpadded_buffer, buffer, unpadded_length);
    *length = unpadded_length;
    return unpadded_buffer;
}

// A function to encrypt a file with a password using AES-256-CBC
int encrypt_file(const char *filename, const char *password)
{
    // Read the file into a buffer
    size_t length;
    uint8_t *buffer = read_file(filename, &length);
    if (buffer == NULL)
    {
        return -1;
    }
    // Pad the buffer to a multiple of 16 bytes
    uint8_t *padded_buffer = pad_buffer(buffer, &length);
    if (padded_buffer == NULL)
    {
        free(buffer);
        return -1;
    }
    free(buffer);
    // Hash the password using SHA-256
    uint8_t key[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (uint8_t *)password, strlen(password));
    sha256_final(&ctx, key);
    // Generate a random initialization vector
    uint8_t iv[16];
    for (int i = 0; i < 16; i++)
    {
        iv[i] = rand() % 256;
    }
    // Initialize the AES context with the key and the IV
    struct AES_ctx aes_ctx;
    AES_init_ctx_iv(&aes_ctx, key, iv);
    // Encrypt the buffer using AES-256-CBC
    AES_CBC_encrypt_buffer(&aes_ctx, padded_buffer, length);
    // Write the IV and the encrypted buffer into the file
    uint8_t *output_buffer = malloc(length + 16);
    if (output_buffer == NULL)
    {
        free(padded_buffer);
        return -1;
    }
    memcpy(output_buffer, iv, 16);
    memcpy(output_buffer + 16, padded_buffer, length);
    free(padded_buffer);
    int result = write_file(filename, output_buffer, length + 16);
    free(output_buffer);
    return result;
}

// A function to decrypt a file with a password using AES-256-CBC
int decrypt_file(const char *filename, const char *password)
{
    // Read the file into a buffer
    size_t length;
    uint8_t *buffer = read_file(filename, &length);
    if (buffer == NULL)
    {
        return -1;
    }
    // Check that the length is at least 16 bytes
    if (length < 16)
    {
        free(buffer);
        return -1;
    }
    // Hash the password using SHA-256
    uint8_t key[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (uint8_t *)password, strlen(password));
    sha256_final(&ctx, key);
    // Extract the initialization vector from the buffer
    uint8_t iv[16];
    memcpy(iv, buffer, 16);
    // Initialize the AES context with the key and the IV
    struct AES_ctx aes_ctx;
    AES_init_ctx_iv(&aes_ctx, key, iv);
    // Decrypt the buffer using AES-256-CBC
    AES_CBC_decrypt_buffer(&aes_ctx, buffer + 16, length - 16);
    // Unpad the buffer
    uint8_t *unpadded_buffer = unpad_buffer(buffer + 16, &length);
    if (unpadded_buffer == NULL)
    {
        free(buffer);
        return -1;
    }
    free(buffer);
    // Subtract 29 from the length to remove padding characters
    length -= 29;
    // Write the decrypted buffer into the file
    int result = write_file(filename, unpadded_buffer, length);
    free(unpadded_buffer);
    return result;
}

// The main function to process the commands
int main(int argc, char *argv[])
{
    // Check if correct number of arguments are provided
    if (argc < 2 || argc > 6)
    {
        printf("\nSecurByte 1.0 Beta (x64) : (c) TechWhizKid - All rights reserved.\n");
        printf("Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n");
        printf("\nUsage: %s [--encrypt-file/-ef | --decrypt-file/-df <filepath>] [--passwd/-p <password>] [--nobanner/-n]\n", argv[0]);
        printf("Use --help/-h to see the help menu.\n");
        return 1;
    }

    // Initialize the variables
    int show_banner = 1;
    char *mode = NULL;
    char *filename = NULL;
    char *password = NULL;

    // Loop over the arguments and check for the flags
    for (int i = 1; i < argc; i++)
    {
        // Check if the --nobanner flag is passed as an argument
        if (strcmp(argv[i], "--nobanner") == 0 || strcmp(argv[i], "-n") == 0)
        {
            show_banner = 0;
        }
        else if (strcmp(argv[i], "--encrypt-file") == 0 || strcmp(argv[i], "-ef") == 0)
        {
            mode = "ef";
            // Check if the next argument is a valid file path
            if (i + 1 < argc && access(argv[i + 1], F_OK) == 0)
            {
                // Assign the file name to the filename variable
                filename = argv[i + 1];
                // Increment the loop counter to skip the next argument
                i++;
            }
            else
            {
                // Print an error message and exit the program
                printf("Invalid file path: %s\n", argv[i + 1]);
                return 1;
            }
        }
        else if (strcmp(argv[i], "--decrypt-file") == 0 || strcmp(argv[i], "-df") == 0)
        {
            mode = "df";
            // Check if the next argument is a valid file path
            if (i + 1 < argc && access(argv[i + 1], F_OK) == 0)
            {
                // Assign the file name to the filename variable
                filename = argv[i + 1];
                // Increment the loop counter to skip the next argument
                i++;
            }
            else
            {
                // Print an error message and exit the program
                printf("Invalid file path: %s\n", argv[i + 1]);
                return 1;
            }
        }
        else if (strcmp(argv[i], "--passwd") == 0 || strcmp(argv[i], "-p") == 0)
        {
            password = argv[i + 1];
            // Check if a valid password is provided
            if (password == NULL)
            {
                // Print an error message and exit the program
                printf("Invalid password: %s\n", password);
                return 1;
            }
            else if (strlen(password) < 6)
            {
                printf("\nPassword is too short, Please use minimum 6 characters.\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            printf("\nSecurByte 1.0 Beta (x64) : (c) TechWhizKid - All rights reserved.\n");
            printf("Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n");
            printf("\nUsage: %s [--encrypt-file/-ef | --decrypt-file/-df <filepath>] [--passwd/-p <password>] [--nobanner/-n]\n", argv[0]);
            printf("\nOptions:\n");
            printf("  -ef, --encrypt-file <filepath>   Encrypts the file at the given path using AES-256-CBC.\n");
            printf("  -df, --decrypt-file <filepath>   Decrypts the file at the given path using AES-256-CBC.\n");
            printf("  -p,  --passwd <password>         Specifies the password to use for encryption or decryption.\n");
            printf("  -n,  --nobanner                  Suppresses the banner display at the start of the program.\n");
            printf("  -h,  --help                      Displays this help menu.\n");
            return 0;
        }
    }

    // Print the banner if the flag is not passed
    if (show_banner)
    {
        printf("\nSecurByte 1.0 Beta (x64) : (c) TechWhizKid - All rights reserved.\n");
        printf("Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n");
    }

    // Perform the encryption or decryption
    int result;
    if (strcmp(mode, "ef") == 0)
    {
        // Encrypt the file
        result = encrypt_file(filename, password);
        if (result == 0)
        {
            printf("\nFile encrypted successfully\n");
        }
        else
        {
            printf("\nFile encryption failed\n");
        }
    }
    else if (strcmp(mode, "df") == 0)
    {
        // Decrypt the file
        result = decrypt_file(filename, password);
        if (result == 0)
        {
            printf("\nFile decrypted successfully\n");
        }
        else
        {
            printf("\nFile decryption failed\n");
        }
    }
    else
    {
        printf("\nInvalid or missing file path.\n");
        printf("Use --help/-h to see the help menu.\n");
        return 1;
    }
    return result;
}

