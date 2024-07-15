#include <stdio.h>

void log_message(const char *message) {
    FILE *log_file = fopen("logs/server.log", "a");
    if (log_file) {
        fprintf(log_file, "%s\n", message);
        fclose(log_file);
    }
}