#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void run_command(const char *command) {
    if (system(command) != 0) {
        fprintf(stderr, "Erro ao executar o comando: %s\n", command);
    }
}

int get_last_ppid_from_audit_file(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Arquivo '%s' não encontrado.\n", file_path);
        return -1;
    }

    char line[256];
    int last_ppid = -1;
    while (fgets(line, sizeof(line), file) != NULL) {
        char *ppid_str = strstr(line, "ppid=");
        if (ppid_str != NULL) {
            int ppid;
            if (sscanf(ppid_str, "ppid=%d", &ppid) == 1) {
                last_ppid = ppid;
            }
        }
    }

    fclose(file);
    return last_ppid;
}

int main() {
    const char *audit_file_path = "/.FAB/logs";
    const char *command = "ausearch -k fab_changes | tail -n 8 > /.FAB/logs";

    run_command(command);

    int last_ppid = get_last_ppid_from_audit_file(audit_file_path);

    if (last_ppid != -1) {
        printf("Último PPID no arquivo: %d\n", last_ppid);
        char kill_command[64];
        snprintf(kill_command, sizeof(kill_command), "pkill -P %d", last_ppid);
        run_command(kill_command);
    } else {
        printf("Nenhum PPID encontrado no arquivo.\n");
    }

    return 0;
}