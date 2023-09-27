#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

int main() {

    system("/.FAB/audit.sh" 2>/dev/null);

    int fd;
    int wd[16]; // Array para guardar os descritores de watch
    char buffer[EVENT_BUF_LEN];

    // Inicialize o inotify
    fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        return -1;
    }

    // Caminhos para monitoramento
    char *caminhos[] = {
        "/home/n1m1.txt",
        "/home/taiki/n1m1.txt",
        "/home/taiki/Desktop/n1m1.txt",
        "/home/taiki/Downloads/n1m1.txt",
        "/home/taiki/Documents/n1m1.txt",
        "/home/taiki/Pictures/n1m1.txt",
        "/home/taiki/snap/n1m1.txt",
        "/home/taiki/Videos/n1m1.txt",
        "/home/taiki/Music/n1m1.txt",
        "/home/taiki/Public/n1m1.txt",
        "/home/taiki/Templates/n1m1.txt",
        "/boot/n1m1.txt",
        "/etc/n1m1.txt",
        "/root/n1m1.txt",
        "/usr/n1m1.txt",
        "/var/n1m1.txt"
    };

    // Adicione os caminhos para monitoramento
    for (int i = 0; i < 16; i++) {
        wd[i] = inotify_add_watch(fd, caminhos[i], IN_MODIFY);
        if (wd[i] == -1) {
            perror("inotify_add_watch");
            return -1;
        }
    }

    printf("Aguardando alterações nos arquivos...\n");


    // Loop infinito para monitorar eventos
    while (1) {
    int length = read(fd, buffer, EVENT_BUF_LEN);

    if (length < 0) {
        perror("read");
        return -1;
    }

    int i = 0;
    while (i < length) {
        struct inotify_event *event = (struct inotify_event *)&buffer[i];

        if (event->mask & IN_MODIFY) {
            // Determine qual arquivo foi modificado
            for (int j = 0; j < 16; j++) {
                if (event->wd == wd[j]) {
                    printf("Alteração detectada em %s. Executando pid...\n", caminhos[j]);

                    // Execute o programa no caminho
                    system("/.FAB/pid");

                    printf("Execução do pid concluída.\n");
                }
            }
        }

        i += EVENT_SIZE + event->len;
    }
}



    // Remova os watches quando não forem mais necessários
    for (int i = 0; i < 16; i++) {
        inotify_rm_watch(fd, wd[i]);
    }
    close(fd);


    return 0;
}