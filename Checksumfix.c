#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_PATH 1024

typedef struct {
    char name[256];
    bool is_dir;
} FileEntry;

// Cálculo da soma aditiva de 32 bits (Padrão GTA San Andreas)
uint32_t calc_gta_sa_checksum(const uint8_t *buf, size_t len) {
    uint32_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += buf[i];
    }
    return sum;
}

void process_gta_save(const char *filepath) {
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        printf("\n[ERRO] Nao foi possivel abrir o arquivo: %s\n", filepath);
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size < 4) {
        printf("\n[ERRO] Arquivo muito pequeno ou invalido para ser um save de GTA SA.\n");
        fclose(f);
        return;
    }

    uint8_t *buffer = malloc(size);
    if (!buffer) {
        printf("\n[ERRO] Falha de alocacao de memoria.\n");
        fclose(f);
        return;
    }

    fread(buffer, 1, size, f);
    fclose(f);

    // O checksum do GTA SA e a soma de todos os bytes exceto os ultimos 4 bytes
    size_t data_len = size - 4;
    uint32_t calculated_sum = calc_gta_sa_checksum(buffer, data_len);
    
    // Checksum lido nos ultimos 4 bytes (Little-Endian)
    uint32_t current_chk = buffer[data_len] | 
                          (buffer[data_len + 1] << 8) | 
                          (buffer[data_len + 2] << 16) | 
                          (buffer[data_len + 3] << 24);

    printf("\n===================================================\n");
    printf("        ANALISE DE CHECKSUM - GTA SAN ANDREAS      \n");
    printf("===================================================\n");
    printf("Arquivo : %s\n", filepath);
    printf("Tamanho : %ld bytes (%.2f KB)\n", size, (float)size / 1024.0f);
    printf("---------------------------------------------------\n");
    printf("Soma Calculada  (4 Bytes) : 0x%08X\n", calculated_sum);
    printf("Bytes (Little-Endian)     : %02X %02X %02X %02X\n",
           (uint8_t)(calculated_sum & 0xFF),
           (uint8_t)((calculated_sum >> 8) & 0xFF),
           (uint8_t)((calculated_sum >> 16) & 0xFF),
           (uint8_t)((calculated_sum >> 24) & 0xFF));
    printf("Checksum Gravado no Save  : 0x%08X\n", current_chk);
    printf("---------------------------------------------------\n");

    if (current_chk == calculated_sum) {
        printf("STATUS : [VALIDO / OK]\n");
    } else {
        printf("STATUS : [INVALIDO / CORROMPIDO]\n");
        
        char op;
        printf("\nDeseja recalcular e aplicar a medicao correta no save? (s/n): ");
        scanf(" %c", &op);

        if (op == 's' || op == 'S') {
            FILE *fw = fopen(filepath, "rb+");
            if (fw) {
                fseek(fw, data_len, SEEK_SET);
                uint8_t chk_bytes[4] = {
                    (uint8_t)(calculated_sum & 0xFF),
                    (uint8_t)((calculated_sum >> 8) & 0xFF),
                    (uint8_t)((calculated_sum >> 16) & 0xFF),
                    (uint8_t)((calculated_sum >> 24) & 0xFF)
                };
                fwrite(chk_bytes, 1, 4, fw);
                fclose(fw);
                printf("\n[SUCESSO] Checksum do GTA SA corrigido com sucesso!\n");
            } else {
                printf("\n[ERRO] Nao foi possivel abrir o arquivo para escrita.\n");
            }
        }
    }

    printf("===================================================\n");
    free(buffer);
}

void file_manager(void) {
    char current_path[MAX_PATH];
    
    if (access("/sdcard", F_OK) == 0) {
        strcpy(current_path, "/sdcard");
    } else {
        getcwd(current_path, sizeof(current_path));
    }

    while (true) {
        DIR *dir = opendir(current_path);
        if (!dir) {
            printf("\n[ERRO] Nao foi possivel abrir o diretorio: %s\n", current_path);
            strcpy(current_path, "/");
            continue;
        }

        struct dirent *entry;
        FileEntry entries[256];
        int count = 0;

        if (strcmp(current_path, "/") != 0) {
            strcpy(entries[count].name, "..");
            entries[count].is_dir = true;
            count++;
        }

        while ((entry = readdir(dir)) != NULL && count < 256) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            char full_item_path[MAX_PATH];
            snprintf(full_item_path, sizeof(full_item_path), "%s/%s", current_path, entry->d_name);

            struct stat st;
            if (stat(full_item_path, &st) == 0) {
                strncpy(entries[count].name, entry->d_name, 255);
                entries[count].is_dir = S_ISDIR(st.st_mode);
                count++;
            }
        }
        closedir(dir);

        printf("——————————————————————————————————————————————————————\n");
        printf("  C H E C K S U M F I X G T A S A N M O B I L E  \n");
        printf("_____________________________________________________\n");
        printf("PASTA ATUAL: %s\n", current_path);
        printf("---------------------------------------------------\n");

        for (int i = 0; i < count; i++) {
            if (entries[i].is_dir) {
                printf(" [%d] [DIR]  %s/\n", i + 1, entries[i].name);
            } else {
                printf(" [%d] [ARQ]  %s\n", i + 1, entries[i].name);
            }
        }
        printf(" [0] Sair do Programa\n");
        printf("---------------------------------------------------\n");
        printf("Escolha o numero do item: ");

        int choice;
        if (scanf("%d", &choice) != 1 || choice == 0) {
            printf("\nEncerrando o programa...\n");
            break;
        }

        if (choice < 1 || choice > count) {
            printf("\nOpcao invalida! Tente novamente.\n");
            continue;
        }

        FileEntry selected = entries[choice - 1];

        if (selected.is_dir) {
            if (strcmp(selected.name, "..") == 0) {
                char *last_slash = strrchr(current_path, '/');
                if (last_slash && last_slash != current_path) {
                    *last_slash = '\0';
                } else {
                    strcpy(current_path, "/");
                }
            } else {
                if (strcmp(current_path, "/") == 0) {
                    snprintf(current_path, sizeof(current_path), "/%s", selected.name);
                } else {
                    strcat(current_path, "/");
                    strcat(current_path, selected.name);
                }
            }
        } else {
            char target_file[MAX_PATH];
            snprintf(target_file, sizeof(target_file), "%s/%s", current_path, selected.name);
            process_gta_save(target_file);

            printf("\nPressione ENTER para voltar ao gerenciador...");
            getchar(); getchar();
        }
    }
}

int main(void) {
    file_manager();
    return 0;
}
