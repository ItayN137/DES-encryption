#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const char* hex_to_binary_table[16] = {
    "0000", // 0
    "0001", // 1
    "0010", // 2
    "0011", // 3
    "0100", // 4
    "0101", // 5
    "0110", // 6
    "0111", // 7
    "1000", // 8
    "1001", // 9
    "1010", // A
    "1011", // B
    "1100", // C
    "1101", // D
    "1110", // E
    "1111"  // F
};

const int PC1_table[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};

const int PC2_table[48] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

const int vector[16] = {
    1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};


/// <summary>
/// Displaying hexadecimal based number in binary based number.
/// </summary>
/// <param name="hex_str"></param>
void print_hex_as_binary(const char* hex_str) {
    printf("Binary representation of hexadecimal number %s: ", hex_str);
    while (*hex_str) {
        char hex_char = *hex_str;
        int decimal;
        if (hex_char >= '0' && hex_char <= '9') {
            decimal = hex_char - '0';
        }
        else if (hex_char >= 'A' && hex_char <= 'F') {
            decimal = hex_char - 'A' + 10;
        }
        else if (hex_char >= 'a' && hex_char <= 'f') {
            decimal = hex_char - 'a' + 10;
        }
        else {
            printf("Invalid hexadecimal character '%c'\n", hex_char);
            return;
        }
        for (int i = 3; i >= 0; i--) {
            printf("%d", (decimal >> i) & 1);
        }
        hex_str++;
    }
    printf("\n");
}

/// <summary>
/// Creates the array of the data devided to 64 bits(Each one is a block to encrypt/decrypt).
/// </summary>
/// <param name="data">The full data which the user gets</param>
/// <returns>Array of blocks of the data</returns>
char** create_blocks_from_data(char* data, int chunks) {
    char** dataArr = (char**)malloc(sizeof(char*) * chunks);
    if (dataArr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < chunks; i++) {
        dataArr[i] = (char*)malloc(17 * sizeof(char));
        if (dataArr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            return NULL;
        }
    }

    for (int i = 0; i < chunks; i++) {
        memcpy(dataArr[i], data + (16 * i), 16);
        dataArr[i][16] = '\0';
    }

    return dataArr;
}


unsigned int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else
        return -1;
}

char* hex_to_binary(const char* hex_str) {
    int len = strlen(hex_str);
    char* binary_str = (char*)malloc((len * 4 + 1) * sizeof(char));
    if (binary_str == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    int index = 0;
    while (*hex_str) {
        char hex_char = *hex_str;
        int decimal = hex_char_to_int(hex_char);
        if (decimal == -1) {
            free(binary_str);
            return NULL;
        }
        for (int i = 3; i >= 0; i--) {
            binary_str[index++] = ((decimal >> i) & 1) + '0';
        }
        hex_str++;
    }
    binary_str[index] = '\0';
    return binary_str;
}


char* binary_to_hex(const char* bin_key) {
    size_t bin_len = strlen(bin_key);
    size_t hex_len = bin_len / 4;
    char* hex_key = (char*)malloc(hex_len + 1);
    if (hex_key == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    for (size_t i = 0; i < hex_len; i++) {
        int decimal = 0;
        for (int j = 0; j < 4; j++) {
            decimal = (decimal << 1) | (bin_key[i * 4 + j] - '0');
        }
        if (decimal < 10) {
            hex_key[i] = '0' + decimal;
        }
        else {
            hex_key[i] = 'A' + (decimal - 10);
        }
    }
    hex_key[hex_len] = '\0';
    return hex_key;
}

char* doPC1(const char* key) {
    int index;
    char* pc1_key = (char*)malloc(57 * sizeof(char)); // 56 bits => 7 bytes + 1 for null terminator
    if (pc1_key == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < 56; i++) {
        pc1_key[i] = key[PC1_table[i] - 1];
    }
    pc1_key[56] = '\0';
    return pc1_key;
}


char* do_PC2(char* key) {
    char* pc2_key = (char*)malloc(49 * sizeof(char));
    if (pc2_key == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 48; i++) {
        pc2_key[i] = key[PC2_table[i] - 1];
    }
    pc2_key[48] = '\0';
    return pc2_key;
}

char** apply_PC2_to_keys(char** keys_arr) {
    char** pc2_keys_arr = (char**)malloc(16 * sizeof(char*));
    if (pc2_keys_arr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 16; i++) {
        pc2_keys_arr[i] = do_PC2(keys_arr[i]);
    }

    return pc2_keys_arr;
}

void free_keys_array(char** keys_arr, int num_keys) {
    for (int i = 0; i < num_keys; i++) {
        free(keys_arr[i]);
    }
    free(keys_arr);
}

char* rotate_left(const char* key, int rotations) {
    int length = strlen(key);
    char* rotated_key = (char*)malloc((length + 1) * sizeof(char));
    if (rotated_key == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    rotations = rotations % length;
    strncpy(rotated_key, key + rotations, length - rotations);
    strncpy(rotated_key + length - rotations, key, rotations);
    rotated_key[length] = '\0';

    return rotated_key;
}


char** generate_half_keys(char* first_key) {
    char** half_keys = (char**)malloc(16 * sizeof(char*));
    if (half_keys == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    for (int i = 0; i < 16; i++) {
        half_keys[i] = (char*)malloc(28 * sizeof(char));
        if (half_keys[i] == NULL) {
            printf("Memory allocation failed!\n");
            return NULL;
        }
    }
    half_keys[0] = rotate_left(first_key, vector[0]);
    for (int i = 1; i < 16; i++) {
        half_keys[i] = rotate_left(half_keys[i - 1], vector[i]);
    }
    
    return half_keys;
}


char** generate_keys_arr(char** left_half_keys, char** right_half_keys) {
    char** keys = (char**)malloc(16 * sizeof(char*));
    if (keys == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    for (int i = 0; i < 16; i++) {
        keys[i] = (char*)malloc(57 * sizeof(char));
        if (keys[i] == NULL) {
            printf("Memory allocation failed!\n");
            return NULL;
        }
    }
    for (int i = 0; i < 16; i++) {
        strcpy(keys[i], left_half_keys[i]);
        strcat(keys[i], right_half_keys[i]);

    }
    return keys;
}


const char* hex_to_binary_char(char hex) {
    if (hex >= '0' && hex <= '9')
        return hex_to_binary_table[hex - '0'];
    else if (hex >= 'A' && hex <= 'F')
        return hex_to_binary_table[hex - 'A' + 10];
    else {
        fprintf(stderr, "Invalid hexadecimal character: %c\n", hex);
        exit(EXIT_FAILURE);
    }
    return NULL;
}




void printRows(char** array, int numRows) {
    for (int i = 0; i < numRows; i++) {
        printf("%s\n", array[i]);
    }
}

int main() {
    //// -----------------------variables initialization part-----------------------
    int size_of_data = 0, chunks;
    char* data = (char*)"0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";
    char* key = (char*)"133457799BBCDFF1";

    char* c0_key = (char*)malloc(29);
    char* d0_key = (char*)malloc(29);
    if (c0_key == NULL || d0_key == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    char** c_keys_arr = (char**)malloc(16);
    char** d_keys_arr = (char**)malloc(16);
    if (c_keys_arr == NULL || d_keys_arr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < 16; i++) {
        c_keys_arr[i] = (char*)malloc(28);
        d_keys_arr[i] = (char*)malloc(28);
        if (c_keys_arr[i] == NULL || d_keys_arr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            return 1;
        }
    }

    char** keys_arr = (char**)malloc(16);
    if (keys_arr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < 16; i++) {
        keys_arr[i] = (char*)malloc(28);
        if (keys_arr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            return 1;
        }
    }


    //print_hex_as_binary(key);


    //// -----------------------key creation part-----------------------
    char* binary_key = hex_to_binary(key);
    printf("key before PC1: %s\n", binary_key);
    char* pc1_key = doPC1(binary_key);
    printf("key after PC1 in binary: %s\n", pc1_key);
    
    memcpy(c0_key, pc1_key, 28);
    c0_key[28] = '\0';
    memcpy(d0_key, pc1_key + 28, 28);
    d0_key[28] = '\0';

    printf("c0 key: %s\n", c0_key);
    printf("d0 key: %s\n", d0_key);

    c_keys_arr = generate_half_keys(c0_key);
    d_keys_arr = generate_half_keys(d0_key);
    /*
    for (int i = 0; i < 16; i++) {
        printf("c%d: %s \n", i+1, c_keys_arr[i]);
        printf("d%d: %s \n", i+1, d_keys_arr[i]);
    }
    */
    keys_arr = generate_keys_arr(c_keys_arr, d_keys_arr);
    char** pc2_keys_arr = apply_PC2_to_keys(keys_arr);
    for (int i = 0; i < 16; i++) {
        printf("k%d: %s \n", i + 1, pc2_keys_arr[i]);
    }


    //// -----------------------data creation part-----------------------

    char** dataArr, **binaryDataArr;
    char* binaryData = hex_to_binary(data);
    chunks = ceil((double)strlen(data) / 16);
    printf("%s \n", binaryData);
    printf("%d \n", strlen(binaryData));
    dataArr = create_blocks_from_data(data, chunks);
    printRows(dataArr, chunks);









    //free all and avoid memory leaks
    free(binary_key);
    free(pc1_key);
    free(c0_key);
    free(d0_key);
    for (int i = 0; i < 16; i++) {
        free(c_keys_arr[i]);
        free(d_keys_arr[i]);
    }
    free(c_keys_arr);
    free(d_keys_arr);
    free_keys_array(keys_arr, 16);
    free_keys_array(pc2_keys_arr, 16);



    return 0;
}


