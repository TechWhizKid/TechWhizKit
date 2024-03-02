#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <ctype.h>

// A function to check if the drive is supported and recognized
int check_drive(char *drive_path)
{
    // Initialize the variable
    int result = 0;

    // Get the drive type using the Windows API
    UINT drive_type = GetDriveTypeA(drive_path);

    // Check the drive type and print the result
    switch (drive_type)
    {
    case DRIVE_FIXED:
        printf("\nThe drive %s is a HDD.\n", drive_path);
        result = 1;
        break;
    case DRIVE_REMOVABLE:
        printf("\nThe drive %s is a removable drive.\n", drive_path);
        result = 1;
        break;
    case DRIVE_CDROM:
        printf("\nThe drive %s is a CD-ROM drive.\n", drive_path);
        result = 0;
        break;
    case DRIVE_REMOTE:
        printf("\nThe drive %s is a network drive.\n", drive_path);
        result = 1;
        break;
    case DRIVE_RAMDISK:
        printf("\nThe drive %s is a RAM disk.\n", drive_path);
        result = 0;
        break;
    case DRIVE_UNKNOWN:
        printf("\nThe drive %s is of unknown type.\n", drive_path);
        result = 0;
        break;
    case DRIVE_NO_ROOT_DIR:
        printf("\nThe drive %s is not a valid drive.\n", drive_path);
        result = 0;
        break;
    default:
        printf("\nThe drive %s is of unrecognized type.\n", drive_path);
        result = 0;
        break;
    }

    // Return the result
    return result;
}

// A function to fill the drive with random data and make it unrecoverable
int wipe_drive(char *drive_path, int pass_count)
{
    // Initialize the variables
    int result = 0;
    DWORD bytes_written = 0;
    ULARGE_INTEGER free_bytes = {0};
    HANDLE file_handle = NULL;
    char file_name[MAX_PATH];
    char *buffer = NULL;

    // Get the free space on the drive
    if (!GetDiskFreeSpaceExA(drive_path, NULL, NULL, &free_bytes))
    {
        fprintf(stderr, "Failed to get the free space on the drive %s.\n", drive_path);
        return -1;
    }

    // Create a file name that fills the entire free space
    sprintf(file_name, "%s\\%llu.tmp", drive_path, free_bytes.QuadPart);

    // Allocate a buffer of 4 KB to write random bytes
    buffer = (char *)malloc(4096);
    if (buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the buffer.\n");
        return -1;
    }

    // Repeat the process for the given pass count
    for (int i = 0; i < pass_count; i++)
    {
        // Create a file that fills the entire free space
        file_handle = CreateFileA(file_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file_handle == INVALID_HANDLE_VALUE)
        {
            fprintf(stderr, "Failed to create the file %s.\n", file_name);
            free(buffer);
            return -1;
        }

        // Write random bytes to the file
        printf("\nWiping the drive %s with random data (pass %d of %d)...\n", drive_path, i + 1, pass_count);
        // Store the initial free space
        ULONGLONG initial_free_bytes = free_bytes.QuadPart;
        while (free_bytes.QuadPart > 0)
        {
            // Generate 4 KB of random bytes
            for (int j = 0; j < 4096; j++)
            {
                buffer[j] = rand() % 256;
            }

            // Write 4 KB of random bytes to the file
            if (!WriteFile(file_handle, buffer, 4096, &bytes_written, NULL))
            {
                fprintf(stderr, "Failed to write to the file %s.\n", file_name);
                CloseHandle(file_handle);
                DeleteFileA(file_name);
                free(buffer);
                return -1;
            }

            // Update the free space
            free_bytes.QuadPart -= bytes_written;

            // Calculate and print the progress percentage
            double progress = 100.0 * (initial_free_bytes - free_bytes.QuadPart) / initial_free_bytes;
            printf("\rProgress: %.2f%%", progress);
            fflush(stdout);
        }
        // Print a newline after the loop
        printf("\n");

        // Close and delete the file
        CloseHandle(file_handle);
        // Flush the file buffers to ensure the data is written to the disk
        FlushFileBuffers(file_handle);
        DeleteFileA(file_name);
        // Refresh the free space before the next pass
        GetDiskFreeSpaceExA(drive_path, NULL, NULL, &free_bytes);
    }

    // Free the buffer
    free(buffer);

    // Print a success message
    printf("\nThe drive %s has been wiped successfully.\n", drive_path);

    // Return 0 if successful
    return 0;
}

// The main function to process the commands
int main(int argc, char *argv[])
{
    // Check if the drive letter is provided as an argument
    if (argc < 2 || argc > 6)
    {
        printf("\nUnRecover 1.0 Beta (x64) : (c) TechWhizKid - All rights reserved.\n");
        printf("Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n");
        printf("\nUsage: %s <drive letter> [--nobanner/-n] [--suppress-warning/-s] [--pass/-p <number>]\n", argv[0]);
        printf("Use --help/-h to see the help menu.\n");
        return 1;
    }

    // Initialize the variables
    int show_banner = 1;
    int suppress_warning = 0;
    int pass_count = 2; // Set the default pass count to 2
    int drive_letter_found = 0;
    char drive_path[4];

    // Loop over the arguments and check for the flags
    for (int i = 1; i < argc; i++)
    {
        // Check if the --nobanner flag is passed as an argument
        if (strcmp(argv[i], "--nobanner") == 0 || strcmp(argv[i], "-n") == 0)
        {
            show_banner = 0;
        }
        // Check if the --suppress-warning/-s flag is passed as an argument
        else if (strcmp(argv[i], "--suppress-warning") == 0 || strcmp(argv[i], "-s") == 0)
        {
            suppress_warning = 1;
        }
        // Check if the --pass/-p flag is passed as an argument
        else if (strcmp(argv[i], "--pass") == 0 || strcmp(argv[i], "-p") == 0)
        {
            // Check if the next argument is a valid number
            if (i + 1 < argc && isdigit(argv[i + 1][0]))
            {
                // Convert the next argument to an integer
                int num = atoi(argv[i + 1]);
                // Check if the number is positive and not too large
                if (num > 0 && num < 100)
                {
                    // Set the pass count to the number
                    pass_count = num;
                    // Skip the next argument
                    i++;
                }
                else
                {
                    printf("\nInvalid pass count: %s\n", argv[i + 1]);
                    printf("The pass count must be a positive integer between 1 and 99.\n");
                    printf("Use --help/-h to see the help menu.\n");
                    return 1;
                }
            }
            else
            {
                printf("\nPass count not specified.\n");
                printf("Use --help/-h to see the help menu.\n");
                return 1;
            }
        }
        // Check if the --help/-h flag is passed as an argument
        else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            // Print the help menu
            printf("\nUnRecover 1.0 Beta (x64) : (c) TechWhizKid - All rights reserved.\n");
            printf("Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n");
            printf("\nUsage: %s <drive letter> [--nobanner/-n] [--suppress-warning/-s] [--pass/-p <number>]\n", argv[0]);
            printf("\nOptions:\n");
            printf("-n, --nobanner\t\tSuppresses the banner.\n");
            printf("-s, --suppress-warning\tSuppresses the warning message for SSDs.\n");
            printf("-p, --pass <number>\tSets the number of passes to wipe the drive.\n");
            printf("-h, --help\t\tShows this help menu.\n");
            return 0;
        }
        // Check if the drive letter is passed as an argument
        else if (strlen(argv[i]) == 1 && isalpha(argv[i][0]))
        {
            drive_letter_found = 1;
            sprintf(drive_path, "%c:\\", argv[i][0]);
        }
        // Check if the drive letter with a colon is passed as an argument
        else if (strlen(argv[i]) == 2 && isalpha(argv[i][0]) && argv[i][1] == ':')
        {
            drive_letter_found = 1;
            sprintf(drive_path, "%s\\", argv[i]);
        }
        // Check if the drive letter with a colon and a backslash is passed as an argument
        else if (strlen(argv[i]) == 3 && isalpha(argv[i][0]) && argv[i][1] == ':' && argv[i][2] == '\\')
        {
            drive_letter_found = 1;
            strcpy(drive_path, argv[i]);
        }
        // Invalid argument
        else
        {
            printf("\nInvalid argument: %s\n", argv[i]);
            printf("Use --help/-h to see the help menu.\n");
            return 1;
        }
    }

    // Check if the drive letter is found
    if (!drive_letter_found)
    {
        printf("\nDrive letter not specified.\n");
        printf("Use --help/-h to see the help menu.\n");
        return 1;
    }

    // Print the banner if the flag is not passed
    if (show_banner)
    {
        printf("\nUnRecover 1.0 Beta (x64) : (c) TechWhizKid - All rights reserved.\n");
        printf("Source - \"https://github.com/TechWhizKid/TechWhizKit\"\n");
    }

    // Check if the drive is supported and recognized
    if (check_drive(drive_path))
    {
        // Print the warning message in red if the flag is not passed
        if (!suppress_warning)
        {
            printf("\x1b[31m\nWARNING: This tool will fill the drive %s with random data, making it unrecoverable. Not suitable for SSDs, as they handle data differently. Using this tool on an SSD may reduce its lifespan and performance. If unsure whether your drive is an SSD, avoid using it. Use at your own risk; back up important files before proceeding. Press 'enter' to continue or Ctrl+C to abort.\x1b[0m", drive_path);
            getchar();
        }

        // Wipe the drive
        if (wipe_drive(drive_path, pass_count) == -1)
        {
            fprintf(stderr, "Failed to wipe the drive %s.\n", drive_path);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "The drive %s is not supported or recognized.\n", drive_path);
        return 1;
    }

    return 0;
}

