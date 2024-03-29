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

const int IP_table[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

static const int E_Table[48] = {
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};


static const int S_Box[8][4][16] = {
    // S-box 1
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    // S-box 2
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    // S-box 3
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },
    // S-box 4
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },
    // S-box 5
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },
    // S-box 6
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },
    // S-box 7
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },
    // S-box 8
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
};

const int P_Table[32] = {
    16,  7, 20, 21,
    29, 12, 28, 17,
     1, 15, 23, 26,
     5, 18, 31, 10,
     2,  8, 24, 14,
    32, 27,  3,  9,
    19, 13, 30,  6,
    22, 11,  4, 25
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
        dataArr[i] = (char*)malloc(65 * sizeof(char));
        if (dataArr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            return NULL;
        }
    }

    for (int i = 0; i < chunks; i++) {
        memcpy(dataArr[i], data + (64 * i), 64);
        dataArr[i][64] = '\0';
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

char* apply_IP_to_data_block(char* data) {
    char* IP_data = (char*)malloc(65 * sizeof(char)); // 64 bits + null terminator
    if (IP_data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 64; i++) {
        IP_data[i] = data[IP_table[i] - 1]; // Apply IP permutation
    }
    IP_data[64] = '\0'; // Null-terminate the string

    return IP_data;
}

char** apply_IP_to_data_array(char** dataArr, int chunks) {
    char** IP_data_arr = (char**)malloc(chunks * sizeof(char*));
    if (IP_data_arr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < chunks; i++) {
        IP_data_arr[i] = apply_IP_to_data_block(dataArr[i]);
    }

    return IP_data_arr;
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


char* binary_xor(const char* num1, const char* num2, size_t length) {
    char* result = (char*)malloc(length + 1);
    if (result == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    for (size_t i = 0; i < length; ++i) {
        result[i] = ((num1[i] - '0') ^ (num2[i] - '0')) + '0';
    }

    result[length] = '\0';

    return result;
}

char* apply_permutation_p(char* data) {
    char* p_data = (char*)malloc(33 * sizeof(char));
    if (p_data == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    for (int i = 0; i < 32; i++) {
        p_data[i] = data[P_Table[i] - 1];
        //printf("number to apply in round %d: %d --> %d\n", i, P_Table[i] - 1, data[P_Table[i] - 1] - '0');
    }
    p_data[32] = '\0';
    return p_data;
}


char* apply_s_boxes(char* data) {
    char* s_data = (char*)calloc(33, sizeof(char));
    if (s_data == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    int row, column;
    for (int i = 0; i < 8; i++) {
        row = (data[i*6] -'0') * 2 + (data[(i * 6) + 5] - '0');
        column = (data[(i * 6) + 1] - '0') * 8 + (data[(i * 6) + 2] - '0') * 4 + (data[(i * 6) + 3] - '0') * 2 + (data[(i * 6) + 4] - '0');
        strcat(s_data,hex_to_binary_table[S_Box[i][row][column]]);
    }
    s_data[32] = '\0';
    //printf("\ndata after s box: %s", s_data);
    return s_data;
}

char* expension(char* data) {
    char* expanded_data = (char*)malloc(49 * sizeof(char));
    if (expanded_data == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    for (int i = 0; i < 48; i++) {
        expanded_data[i] = data[E_Table[i] - 1];
    }
    expanded_data[48] = '\0';
    return expanded_data;
}


char* encryption_rounds(char* data, char** keys) {
    char* right_data = (char*)malloc(33*sizeof(char));
    char* left_data = (char*)malloc(33*sizeof(char));
    char* next_left_data = (char*)malloc(33*sizeof(char));

    if (right_data == NULL || left_data == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    memcpy(left_data, data, 32);
    left_data[32] = '\0';
    memcpy(right_data, data + 32, 32);
    right_data[32] = '\0';

    strcpy(next_left_data, right_data);
    printf("next left data: %s \n", next_left_data);

    right_data = (char*)realloc(right_data, 49 * sizeof(char));
    right_data = expension(right_data);
    printf("%s \n", right_data);
    right_data = binary_xor(right_data, keys[0], 48);
    printf("%s \n", right_data);
    char* s_right_data = apply_s_boxes(right_data);
    printf("%s \n", s_right_data);
    char* p_right_data = apply_permutation_p(s_right_data);
    printf("permutation p:%s \n", p_right_data);
    char* next_data = binary_xor(p_right_data, left_data, 32);
    printf("R1: %s \n", next_data);
    strncpy(left_data, next_left_data, 32);
    printf("%s \n", left_data);
    return right_data;
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

    char** dataArr, **ipData;
    char* binaryData = hex_to_binary(data);
    chunks = ceil((double)strlen(binaryData) / 64);
    printf("%s \n", binaryData);
    printf("%d \n", strlen(binaryData));
    dataArr = create_blocks_from_data(binaryData, chunks);
    printf("%s", "before IP: \n");
    printRows(dataArr, chunks);


    //// -----------------------encryption part--------------------------

    ipData = apply_IP_to_data_array(dataArr, chunks);
    printf("%s", "after IP: \n");
    printRows(ipData, chunks);

    char* data1 = (char*)malloc(49 * sizeof(char));
    data1 = encryption_rounds(ipData[0], pc2_keys_arr);










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


