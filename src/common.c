#include <stdio.h>
#include <string.h>

#include "common.h"
#include "encrypt.h"

void warn(char *msg) {
    printf("\033[1m\x1b[33m[WARNING]:\x1b[0m %s\n", msg);
}

void err(char *msg, u8 nb_err) {
    printf("\033[1m\x1b[31m[ERROR %u]:\x1b[0m %s\n", nb_err, msg);
}

u8 check_args(i8 *arg){
    if (strlen(arg) > 6) {
        return err("Invalid size of argument\n", 1), 1;
    }
    else {
        for (u32 i = 0; i < strlen(arg); i++) {
            if (arg[i] < 47 ||
                (arg[i] > 57 && arg[i] < 64) ||
                (arg[i] > 70 && arg[i] < 96) ||
                arg[i] > 102)
            {
                return err("Invalid character in the argument", 2), 1;
            }
        }
    }

    return 0;
}

void generate_round_keys(u8 key_reg[10], u8 round_key[11][3]) {
    u8 shifted_reg[10];
    key_reg[3] = key_reg[4] = key_reg[5] = key_reg[6] = key_reg[7] = key_reg[8] = key_reg[9] = 0;

    for (u8 i = 0; i < 11; i++) {
        // Update the round key
        round_key[i][0] = key_reg[5];
        round_key[i][1] = key_reg[6];
        round_key[i][2] = key_reg[7];

        // Shift the key register by 61 to the left
        shifted_reg[0] = key_reg[7] << 5 | key_reg[8] >> 3;
        shifted_reg[1] = key_reg[8] << 5 | key_reg[9] >> 3;
        shifted_reg[2] = key_reg[9] << 5 | key_reg[0] >> 3;
        shifted_reg[3] = key_reg[0] << 5 | key_reg[1] >> 3;
        shifted_reg[4] = key_reg[1] << 5 | key_reg[2] >> 3;
        shifted_reg[5] = key_reg[2] << 5 | key_reg[3] >> 3;
        shifted_reg[6] = key_reg[3] << 5 | key_reg[4] >> 3;
        shifted_reg[7] = key_reg[4] << 5 | key_reg[5] >> 3;
        shifted_reg[8] = key_reg[5] << 5 | key_reg[6] >> 3;
        shifted_reg[9] = key_reg[6] << 5 | key_reg[7] >> 3;

        // First 4 high-order bits of the key register through the SBox
        shifted_reg[0] = (sbox_layer_encrypt(shifted_reg[0]) & 0xF0) | (shifted_reg[0] & 0x0F);

        // XOR bits 19 to 15 with round counter
        shifted_reg[7] ^= (i + 1) >> 1;
        shifted_reg[8] ^= (i + 1) << 7;

        // Copy the temporarily shifted key register to the original
        for (int j = 0; j < 10; j++) {
            key_reg[j] = shifted_reg[j];
        }
    }
}

void print_bin(u8 c) {
    for (i32 i = 7; i >= 0; i--) {
        printf("%d", (c >> i) & 0x01 ? 1 : 0);
    }
    printf("\n");
}

