#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

void bytes_to_hex(const uint8_t *bytes, size_t size, char *out, bool big_endian) {
    out[0] = '\0';
    char temp[10];
    if (!big_endian) {
        for (size_t i = 0; i < size; i++) {
            snprintf(temp, sizeof(temp), "%02X ", bytes[i]);
            strcat(out, temp);
        }
    } else {
        for (int i = (int)size - 1; i >= 0; i--) {
            snprintf(temp, sizeof(temp), "%02X ", bytes[i]);
            strcat(out, temp);
        }
    }
}

void bytes_to_oct(const uint8_t *bytes, size_t size, char *out, bool big_endian) {
    out[0] = '\0';
    char temp[10];
    if (!big_endian) {
        for (size_t i = 0; i < size; i++) {
            snprintf(temp, sizeof(temp), "%03o ", bytes[i]);
            strcat(out, temp);
        }
    } else {
        for (int i = (int)size - 1; i >= 0; i--) {
            snprintf(temp, sizeof(temp), "%03o ", bytes[i]);
            strcat(out, temp);
        }
    }
}

void bytes_to_bin(const uint8_t *bytes, size_t size, char *out, bool big_endian) {
    out[0] = '\0';
    char temp[12];
    int start = (!big_endian) ? 0 : (int)size - 1;
    int end = (!big_endian) ? (int)size : -1;
    int step = (!big_endian) ? 1 : -1;

    for (int i = start; i != end; i += step) {
        char bits[9];
        for (int bit = 7; bit >= 0; bit--) {
            bits[7 - bit] = ((bytes[i] >> bit) & 1) ? '1' : '0';
        }
        bits[8] = '\0';
        snprintf(temp, sizeof(temp), "%s ", bits);
        strcat(out, temp);
    }
}

void render_type(const char *name, const void *ptr, size_t size) {
    char hex_le[128], hex_be[128];
    char oct_le[128], oct_be[128];
    char bin_le[256], bin_be[256];

    const uint8_t *b = (const uint8_t *)ptr;

    bytes_to_hex(b, size, hex_le, false);
    bytes_to_hex(b, size, hex_be, true);

    bytes_to_oct(b, size, oct_le, false);
    bytes_to_oct(b, size, oct_be, true);

    bytes_to_bin(b, size, bin_le, false);
    bytes_to_bin(b, size, bin_be, true);

    printf("\n=== [%s - %zu Byte(s)] ===\n", name, size);
    printf(" [LITTLE-ENDIAN]\n");
    printf("   HEX : %s\n", hex_le);
    printf("   OCT : %s\n", oct_le);
    printf("   BIN : %s\n", bin_le);

    if (size > 1) {
        printf(" [BIG-ENDIAN]\n");
        printf("   HEX : %s\n", hex_be);
        printf("   OCT : %s\n", oct_be);
        printf("   BIN : %s\n", bin_be);
    }
}

int main(void) {
    double input;

    while (true) {
        printf("-------------------------------------------------------");
        printf(" \n  S C R I P T B A S E C O N V E R T E R \n");
        printf("-------------------------------------------------------");
        printf("\n");
        printf("Digite um valor: (ou digite 0 para sair): ");

        if (scanf("%lf", &input) != 1 || input == 0) {
            printf("\nEncerrando...\n");
            break;
        }

        uint64_t u_val = (uint64_t)input;
        int64_t  i_val = (int64_t)input;

        // Inteiros sem sinal
        uint8_t  u8  = (uint8_t)u_val;
        uint16_t u16 = (uint16_t)u_val;
        uint32_t u32 = (uint32_t)u_val;
        uint64_t u64 = u_val;

        render_type("uint8_t", &u8, sizeof(u8));
        render_type("uint16_t", &u16, sizeof(u16));
        render_type("uint32_t", &u32, sizeof(u32));
        render_type("uint64_t", &u64, sizeof(u64));

        // Inteiros com sinal
        int8_t   i8  = (int8_t)i_val;
        int16_t  i16 = (int16_t)i_val;
        int32_t  i32 = (int32_t)i_val;
        int64_t  i64 = i_val;

        render_type("int8_t", &i8, sizeof(i8));
        render_type("int16_t", &i16, sizeof(i16));
        render_type("int32_t", &i32, sizeof(i32));
        render_type("int64_t", &i64, sizeof(i64));

        // Ponto Flutuante
        float  f32 = (float)input;
        double f64 = input;

        render_type("float (32-bit)", &f32, sizeof(f32));
        render_type("double (64-bit)", &f64, sizeof(f64));
    }

    return 0;
}
