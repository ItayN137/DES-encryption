#define _CRT_SECURE_NO_WARNINGS
#define NUM_BITS 64
#define HALF_NUM_BITS 32
#define QUARTER_NUM_BITS 16
#define EXP_HALF_NUM_BITS 48
#define REDUCTION_NUM_BITS 56
#define REDUCTION_HALF_NUM_BITS 28

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//// -----------------------tables part-----------------------

/// <summary>
/// Table representing hexadecimal to binary conversion. Each hexadecimal digit
/// is mapped to its corresponding 4-bit binary representation.
/// </summary>
/// <param name="hex_digit">Hexadecimal digit (0-9, A-F)</param>
/// <returns>Pointer to the 4-bit binary representation of the input hexadecimal digit</returns>
const char* hex_to_binary_table[QUARTER_NUM_BITS] = {
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


/// <summary>
/// Permuted Choice 1 table for key generation in DES. Determines the bit positions
/// to be selected from the original 64-bit key.
/// </summary>
/// <param name="bit_position">Position of the bit in the original key (1-64)</param>
/// <returns>Position of the bit in the permuted key</returns>
const int PC1_table[REDUCTION_NUM_BITS] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};


/// <summary>
/// Permuted Choice 2 table for key generation in DES. Determines the bit positions
/// to be selected from the 56-bit key after the first permutation.
/// </summary>
/// <param name="bit_position">Position of the bit in the 56-bit key after the first permutation (1-56)</param>
/// <returns>Position of the bit in the final 48-bit key for each round</returns>

const int PC2_table[EXP_HALF_NUM_BITS] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};


/// <summary>
/// Vector representing the number of shifts for each round in DES key generation.
/// </summary>
/// <param name="round">Round number (1-16)</param>
/// <returns>Number of shifts for the corresponding round</returns>
const int vector[QUARTER_NUM_BITS] = {
    1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};


/// <summary>
/// Initial Permutation table for data in DES. Determines the initial arrangement
/// of bits in the input data block.
/// </summary>
/// <param name="bit_position">Position of the bit in the original data block (1-64)</param>
/// <returns>Position of the bit in the permuted data block</returns>
const int IP_table[NUM_BITS] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};


/// <summary>
/// Expansion table for data in DES. Determines the arrangement of bits in the
/// expanded data block during the Feistel function.
/// </summary>
/// <param name="bit_position">Position of the bit in the 32-bit half block (1-32)</param>
/// <returns>Position of the bit in the expanded 48-bit block</returns>
static const int E_Table[EXP_HALF_NUM_BITS] = {
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};


/// <summary>
/// S-Boxes used in DES for the Feistel function. Substitutes 6-bit input blocks
/// with 4-bit output blocks according to predefined tables.
/// </summary>
/// <param name="s_box_index">Index of the S-Box (1-8)</param>
/// <param name="row">Row index within the S-Box (0-3)</param>
/// <param name="column">Column index within the S-Box (0-15)</param>
/// <returns>Value of the corresponding 4-bit output block</returns>
static const int S_Box[8][4][QUARTER_NUM_BITS] = {
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


/// <summary>
/// Permutation table for data in DES. Determines the final arrangement
/// of bits in the data block after the Feistel function.
/// </summary>
/// <param name="bit_position">Position of the bit in the 32-bit half block (1-32)</param>
/// <returns>Position of the bit in the permuted 32-bit block</returns>
const int P_Table[HALF_NUM_BITS] = {
    16,  7, 20, 21,
    29, 12, 28, 17,
     1, 15, 23, 26,
     5, 18, 31, 10,
     2,  8, 24, 14,
    32, 27,  3,  9,
    19, 13, 30,  6,
    22, 11,  4, 25
};

//// -----------------------functions part-----------------------


/// <summary>
/// Pads a string with '0' bits to ensure its length is a multiple of a specified number of bits.
/// </summary>
/// <param name="input">The input string to be padded.</param>
/// <param name="padded_length">The length of the padded string (output parameter).</param>
/// <returns>A dynamically allocated string containing the padded input string.</returns>
/// <remarks>The returned string must be freed using the free() function when it is no longer needed
///          to avoid memory leaks.</remarks>
/// <remarks>The caller is responsible for managing the memory allocated for the returned string.</remarks>
/// <remarks>The length of the input string should be less than or equal to INT_MAX / 2 to avoid
///          potential integer overflow issues during calculations.</remarks>
char* pad_string(const char* input) {
    int input_length = strlen(input);
    int padding_length = (QUARTER_NUM_BITS - (input_length % QUARTER_NUM_BITS)) % QUARTER_NUM_BITS; // Calculate the required padding
    int padded_length = input_length + padding_length; // Calculate the padded length
    char* padded_string = (char*)malloc((padded_length + 1) * sizeof(char)); // Allocate memory for the padded string
    if (padded_string == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(padded_string, input); // Copy the original string
    for (int i = input_length; i < padded_length; i++) {
        padded_string[i] = '0'; // Add padding bits
    }
    padded_string[padded_length] = '\0'; // Null-terminate the string
    return padded_string;
}


/// <summary>
/// Prints the binary representation of a hexadecimal number.
/// </summary>
/// <param name="hex_str">Hexadecimal string to be converted</param>
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
/// Creates an array of blocks from the given data, dividing it into chunks of specified size.
/// </summary>
/// <param name="data">Input data to be divided into blocks</param>
/// <param name="chunks">Number of chunks to divide the data into</param>
/// <returns>Array of pointers to blocks of data</returns>
/// <remarks>Memory allocation is performed internally and must be freed by the caller.</remarks>
char** create_blocks_from_data(char* data, int chunks) {
    char** dataArr = (char**)malloc(sizeof(char*) * chunks);
    if (dataArr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < chunks; i++) {
        dataArr[i] = (char*)malloc((NUM_BITS + 1) * sizeof(char));
        if (dataArr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            return NULL;
        }
    }

    for (int i = 0; i < chunks; i++) {
        memcpy(dataArr[i], data + (NUM_BITS * i), NUM_BITS);
        dataArr[i][NUM_BITS] = '\0';
    }

    return dataArr;
}


/// <summary>
/// Concatenates an array of strings into a single string.
/// </summary>
/// <param name="dataArr">Array of strings to be concatenated</param>
/// <param name="chunks">Number of strings in the array</param>
/// <returns>Newly allocated string containing the concatenated data</returns>
/// <remarks>The caller is responsible for freeing the memory allocated for the returned string.</remarks>
char* concatenate_blocks(char** dataArr, int chunks) {
    int total_length = 0;
    for (int i = 0; i < chunks; i++) {
        total_length += strlen(dataArr[i]);
    }

    char* concatenated = (char*)malloc((total_length + 1) * sizeof(char));
    if (concatenated == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    int pos = 0;
    for (int i = 0; i < chunks; i++) {
        strcpy(concatenated + pos, dataArr[i]);
        pos += strlen(dataArr[i]);
    }
    concatenated[total_length] = '\0'; // Null-terminate the string

    return concatenated;
}


/// <summary>
/// Converts a string of ASCII characters to its hexadecimal representation.
/// </summary>
/// <param name="input">Input string containing ASCII characters</param>
/// <returns>Newly allocated string containing the hexadecimal representation</returns>
/// <remarks>The caller is responsible for freeing the memory allocated for the returned string.</remarks>
char* get_ascii_hex(const char* input) {
    int len = strlen(input);
    char* output = (char*)malloc(len * 2 + 1);
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < len; i++) {
        sprintf(output + i * 2, "%02X", (unsigned char)input[i]);
    }
    output[len * 2] = '\0';

    return output;
}


/// <summary>
/// Converts a string of hexadecimal characters to its ASCII representation.
/// </summary>
/// <param name="input">Input string containing hexadecimal characters</param>
/// <returns>Newly allocated string containing the ASCII representation</returns>
/// <remarks>The caller is responsible for freeing the memory allocated for the returned string.</remarks>
char* get_hex_ascii(const char* input) {
    int len = strlen(input);
    if (len % 2 != 0) {
        return NULL;
    }
    char* output = (char*)malloc(len / 2 + 1);
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < len; i += 2) {
        char hex_pair[3] = { input[i], input[i + 1], '\0' };
        int ascii_value;
        sscanf(hex_pair, "%02X", &ascii_value);
        output[j++] = (char)ascii_value;
    }
    output[j] = '\0';

    return output;
}


/// <summary>
/// Converts a hexadecimal character to its corresponding integer value.
/// </summary>
/// <param name="c">Hexadecimal character (0-9, A-F, a-f)</param>
/// <returns>Integer value of the hexadecimal character</returns>
/// <remarks>Returns -1 for invalid input characters.</remarks>
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


/// <summary>
/// Converts a hexadecimal string to its binary representation.
/// </summary>
/// <param name="hex_str">Hexadecimal string to be converted</param>
/// <returns>Binary representation of the input hexadecimal string</returns>
/// <remarks>Memory is allocated internally for the binary string and must be freed by the caller.</remarks>
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


/// <summary>
/// Converts a binary string to its hexadecimal representation.
/// </summary>
/// <param name="bin_key">Binary string to be converted</param>
/// <returns>Hexadecimal representation of the input binary string</returns>
/// <remarks>Memory is allocated internally for the hexadecimal string and must be freed by the caller.</remarks>
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


/// <summary>
/// Performs Permuted Choice 1 (PC1) on the given key, selecting specific bits
/// from the original 64-bit key to form a 56-bit key.
/// </summary>
/// <param name="key">Original 64-bit key</param>
/// <returns>56-bit key after PC1 permutation</returns>
/// <remarks>Memory is allocated internally for the resulting key and must be freed by the caller.</remarks>
char* doPC1(const char* key) {
    int index;
    char* pc1_key = (char*)malloc((REDUCTION_NUM_BITS + 1) * sizeof(char)); // 56 bits => 7 bytes + 1 for null terminator
    if (pc1_key == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < REDUCTION_NUM_BITS; i++) {
        pc1_key[i] = key[PC1_table[i] - 1];
    }
    pc1_key[REDUCTION_NUM_BITS] = '\0';
    return pc1_key;
}


/// <summary>
/// Performs Permuted Choice 2 (PC2) on the given key, selecting specific bits
/// from the 56-bit key after the PC1 permutation to form a 48-bit key for each round.
/// </summary>
/// <param name="key">56-bit key after PC1 permutation</param>
/// <returns>48-bit key after PC2 permutation</returns>
/// <remarks>Memory is allocated internally for the resulting key and must be freed by the caller.</remarks>
char* do_PC2(char* key) {
    char* pc2_key = (char*)malloc((EXP_HALF_NUM_BITS + 1) * sizeof(char));
    if (pc2_key == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < EXP_HALF_NUM_BITS; i++) {
        pc2_key[i] = key[PC2_table[i] - 1];
    }
    pc2_key[EXP_HALF_NUM_BITS] = '\0';
    return pc2_key;
}


/// <summary>
/// Applies Permuted Choice 2 (PC2) permutation to each key in the given array
/// of keys, resulting in an array of 48-bit keys for each round.
/// </summary>
/// <param name="keys_arr">Array of 56-bit keys after PC1 permutation</param>
/// <returns>Array of 48-bit keys after PC2 permutation for each round</returns>
/// <remarks>Memory is allocated internally for the resulting array and keys, and must be freed by the caller.</remarks>
char** apply_PC2_to_keys(char** keys_arr) {
    char** pc2_keys_arr = (char**)malloc(QUARTER_NUM_BITS * sizeof(char*));
    if (pc2_keys_arr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 16; i++) {
        pc2_keys_arr[i] = do_PC2(keys_arr[i]);
    }

    return pc2_keys_arr;
}


/// <summary>
/// Applies the Initial Permutation (IP) to the given data block, rearranging
/// the bits according to the IP table.
/// </summary>
/// <param name="data">Data block to be permuted</param>
/// <returns>Data block after IP permutation</returns>
/// <remarks>Memory is allocated internally for the resulting data block and must be freed by the caller.</remarks>
char* apply_IP_to_data_block(char* data) {
    char* IP_data = (char*)malloc((NUM_BITS + 1) * sizeof(char)); // NUM_BITS bits + null terminator
    if (IP_data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_BITS; i++) {
        IP_data[i] = data[IP_table[i] - 1];
    }
    IP_data[NUM_BITS] = '\0';

    return IP_data;
}


/// <summary>
/// Applies the Reverse Initial Permutation (IP^(-1)) to the given data block,
/// rearranging the bits according to the reverse IP table.
/// </summary>
/// <param name="data">Data block to be permuted</param>
/// <returns>Data block after reverse IP permutation</returns>
/// <remarks>Memory is allocated internally for the resulting data block and must be freed by the caller.</remarks>
char* apply_reverse_IP_to_data_block(char* data) {
    char* IP_data = (char*)malloc((NUM_BITS + 1) * sizeof(char));
    if (IP_data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_BITS; i++) {
        IP_data[IP_table[i] - 1] = data[i];
    }
    IP_data[NUM_BITS] = '\0';

    return IP_data;
}


/// <summary>
/// Applies the Initial Permutation (IP) to each data block in the given array,
/// rearranging the bits according to the IP table.
/// </summary>
/// <param name="dataArr">Array of data blocks to be permuted</param>
/// <param name="chunks">Number of data blocks in the array</param>
/// <returns>Array of data blocks after IP permutation</returns>
/// <remarks>Memory is allocated internally for the resulting array and data blocks, and must be freed by the caller.</remarks>
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


/// <summary>
/// Applies the Reverse Initial Permutation (IP^(-1)) to each data block in the given array,
/// rearranging the bits according to the reverse IP table.
/// </summary>
/// <param name="dataArr">Array of data blocks to be permuted</param>
/// <param name="chunks">Number of data blocks in the array</param>
/// <returns>Array of data blocks after reverse IP permutation</returns>
/// <remarks>Memory is allocated internally for the resulting array and data blocks, and must be freed by the caller.</remarks>
char** apply_reverse_IP_to_data_array(char** dataArr, int chunks) {
    char** IP_data_arr = (char**)malloc(chunks * sizeof(char*));
    if (IP_data_arr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < chunks; i++) {
        IP_data_arr[i] = apply_reverse_IP_to_data_block(dataArr[i]);
    }

    return IP_data_arr;
}


/// <summary>
/// Frees the memory allocated for the array of keys.
/// </summary>
/// <param name="keys_arr">Array of keys to be freed</param>
/// <param name="num_keys">Number of keys in the array</param>
void free_keys_array(char** keys_arr, int num_keys) {
    for (int i = 0; i < num_keys; i++) {
        free(keys_arr[i]);
    }
    free(keys_arr);
}


/// <summary>
/// Rotates the bits of the given key to the left by the specified number of rotations.
/// </summary>
/// <param name="key">Original key to be rotated</param>
/// <param name="rotations">Number of left rotations</param>
/// <returns>Key after rotation</returns>
/// <remarks>Memory is allocated internally for the rotated key and must be freed by the caller.</remarks>
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


/// <summary>
/// Generates half keys from the given first key using a rotation vector.
/// </summary>
/// <param name="first_key">First key used for generation</param>
/// <returns>Array of half keys</returns>
/// <remarks>Memory is allocated internally for the array and keys, and must be freed by the caller.</remarks>
char** generate_half_keys(char* first_key) {
    char** half_keys = (char**)malloc(QUARTER_NUM_BITS * sizeof(char*));
    if (half_keys == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    for (int i = 0; i < QUARTER_NUM_BITS; i++) {
        half_keys[i] = (char*)malloc(REDUCTION_HALF_NUM_BITS * sizeof(char));
        if (half_keys[i] == NULL) {
            printf("Memory allocation failed!\n");
            return NULL;
        }
    }
    half_keys[0] = rotate_left(first_key, vector[0]);
    for (int i = 1; i < QUARTER_NUM_BITS; i++) {
        half_keys[i] = rotate_left(half_keys[i - 1], vector[i]);
    }
    
    return half_keys;
}


/// <summary>
/// Generates full keys by concatenating left and right half keys.
/// </summary>
/// <param name="left_half_keys">Array of left half keys</param>
/// <param name="right_half_keys">Array of right half keys</param>
/// <returns>Array of full keys</returns>
/// <remarks>Memory is allocated internally for the array and keys, and must be freed by the caller.</remarks>
char** generate_keys_arr(char** left_half_keys, char** right_half_keys) {
    char** keys = (char**)malloc(QUARTER_NUM_BITS * sizeof(char*));
    if (keys == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    for (int i = 0; i < QUARTER_NUM_BITS; i++) {
        keys[i] = (char*)malloc(57 * sizeof(char));
        if (keys[i] == NULL) {
            printf("Memory allocation failed!\n");
            return NULL;
        }
    }
    for (int i = 0; i < QUARTER_NUM_BITS; i++) {
        strcpy(keys[i], left_half_keys[i]);
        strcat(keys[i], right_half_keys[i]);

    }
    return keys;
}


/// <summary>
/// Converts a hexadecimal character to its binary representation.
/// </summary>
/// <param name="hex">Hexadecimal character to be converted</param>
/// <returns>Binary representation of the hexadecimal character</returns>
/// <remarks>This function does not allocate memory; it returns a pointer to a constant array.</remarks>
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


/// <summary>
/// Performs bitwise XOR operation between two binary strings.
/// </summary>
/// <param name="num1">First binary string</param>
/// <param name="num2">Second binary string</param>
/// <param name="length">Length of the binary strings</param>
/// <returns>Result of the XOR operation</returns>
/// <remarks>Memory is allocated internally for the result and must be freed by the caller.</remarks>
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


/// <summary>
/// Applies the permutation P to the given data block.
/// </summary>
/// <param name="data">Data block to be permuted</param>
/// <returns>Data block after permutation P</returns>
/// <remarks>Memory is allocated internally for the resulting data block and must be freed by the caller.</remarks>
char* apply_permutation_p(char* data) {
    char* p_data = (char*)malloc(33 * sizeof(char));
    if (p_data == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    for (int i = 0; i < HALF_NUM_BITS; i++) {
        p_data[i] = data[P_Table[i] - 1];
        //printf("number to apply in round %d: %d --> %d\n", i, P_Table[i] - 1, data[P_Table[i] - 1] - '0');
    }
    p_data[HALF_NUM_BITS] = '\0';
    return p_data;
}


/// <summary>
/// Applies the S-box substitution to the given data block.
/// </summary>
/// <param name="data">Data block to be substituted</param>
/// <returns>Data block after S-box substitution</returns>
/// <remarks>Memory is allocated internally for the resulting data block and must be freed by the caller.</remarks>
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
    s_data[HALF_NUM_BITS] = '\0';
    //printf("\ndata after s box: %s", s_data);
    return s_data;
}


/// <summary>
/// Expands the given data block using the expansion table.
/// </summary>
/// <param name="data">Data block to be expanded</param>
/// <returns>Expanded data block</returns>
/// <remarks>Memory is allocated internally for the resulting data block and must be freed by the caller.</remarks>
char* expension(char* data) {
    char* expanded_data = (char*)malloc((EXP_HALF_NUM_BITS + 1) * sizeof(char));
    if (expanded_data == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    for (int i = 0; i < EXP_HALF_NUM_BITS; i++) {
        expanded_data[i] = data[E_Table[i] - 1];
    }
    expanded_data[EXP_HALF_NUM_BITS] = '\0';
    return expanded_data;
}


/// <summary>
/// Performs encryption rounds using the provided data and keys.
/// </summary>
/// <param name="data">Data block to be encrypted</param>
/// <param name="keys">Array of keys for encryption rounds</param>
/// <returns>Encrypted data block</returns>
/// <remarks>Memory is allocated internally for intermediate data blocks and must be freed by the caller.</remarks>
char* encryption_rounds(char* data, char** keys) {
    char* result_data = (char*)malloc((NUM_BITS + 1) * sizeof(char));
    char* right_data = (char*)malloc(33*sizeof(char));
    char* left_data = (char*)malloc(33*sizeof(char));
    char* next_left_data = (char*)malloc(33*sizeof(char));

    if (right_data == NULL || left_data == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    memcpy(left_data, data, HALF_NUM_BITS);
    left_data[HALF_NUM_BITS] = '\0';
    memcpy(right_data, data + HALF_NUM_BITS, HALF_NUM_BITS);
    right_data[HALF_NUM_BITS] = '\0';


    for (int i = 0; i < QUARTER_NUM_BITS; i++)
    {
        strcpy(next_left_data, right_data);
        //printf("next left data: %s \n", next_left_data);

        right_data = expension(right_data);
        //printf("%s \n", right_data);
        right_data = binary_xor(right_data, keys[i], EXP_HALF_NUM_BITS);
        //printf("%s \n", right_data);
        right_data = apply_s_boxes(right_data);
        //printf("%s \n", right_data);
        right_data = apply_permutation_p(right_data);
        //printf("permutation p:%s \n", right_data);
        right_data = binary_xor(right_data, left_data, HALF_NUM_BITS);
        //printf("R %d: %s \n", i + 1, right_data);
        strncpy(left_data, next_left_data, HALF_NUM_BITS);
        //printf("L %d %s \n", i+1, left_data);
    }

    strncpy(result_data, right_data, HALF_NUM_BITS);
    strncpy(result_data + HALF_NUM_BITS, left_data, HALF_NUM_BITS);
    result_data[NUM_BITS] = '\0';


    return result_data;
}
/// <summary>
/// Performs decryption rounds using the provided data and keys.
/// </summary>
/// <param name="data">Data block to be decrypted</param>
/// <param name="keys">Array of keys for decryption rounds</param>
/// <returns>Decrypted data block</returns>
/// <remarks>Memory is allocated internally for intermediate data blocks and must be freed by the caller.</remarks>
char* decryption_rounds(char* data, char** keys) {
    char* result_data = (char*)malloc((NUM_BITS + 1) * sizeof(char));
    char* right_data = (char*)malloc(33 * sizeof(char));
    char* left_data = (char*)malloc(33 * sizeof(char));
    char* next_left_data = (char*)malloc(33 * sizeof(char));

    if (right_data == NULL || left_data == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    memcpy(left_data, data, HALF_NUM_BITS);
    left_data[HALF_NUM_BITS] = '\0';
    memcpy(right_data, data + HALF_NUM_BITS, HALF_NUM_BITS);
    right_data[HALF_NUM_BITS] = '\0';


    for (int i = 15; i >= 0; i--)
    {
        strcpy(next_left_data, right_data);
        //printf("next left data: %s \n", next_left_data);


        right_data = expension(right_data);
        //printf("%s \n", right_data);
        right_data = binary_xor(right_data, keys[i], EXP_HALF_NUM_BITS);
        //printf("%s \n", right_data);
        right_data = apply_s_boxes(right_data);
        //printf("%s \n", right_data);
        right_data = apply_permutation_p(right_data);
        //printf("permutation p:%s \n", right_data);
        right_data = binary_xor(right_data, left_data, HALF_NUM_BITS);
        //printf("R %d: %s \n", i + 1, right_data);
        strncpy(left_data, next_left_data, HALF_NUM_BITS);
        //printf("L %d %s \n", i + 1, left_data);
    }

    strncpy(result_data, right_data, HALF_NUM_BITS);
    strncpy(result_data + HALF_NUM_BITS, left_data, HALF_NUM_BITS);
    result_data[NUM_BITS] = '\0';


    return result_data;
}

/// <summary>
/// Prints the contents of the array of strings row by row.
/// </summary>
/// <param name="array">Array of strings to be printed</param>
/// <param name="numRows">Number of rows in the array</param>
void printRows(char** array, int numRows) {
    for (int i = 0; i < numRows; i++) {
        printf("%s\n", array[i]);
    }
}

int main() {
    //// -----------------------variables initialization part-----------------------
    int size_of_data = 0, chunks;
    char* data = (char*)"hello world!";
    data = get_ascii_hex(data);
    printf("%s", data);
    data = pad_string(data);
    char* key = (char*)"NVJdqu12";
    key = get_ascii_hex(key);

    char* c0_key = (char*)malloc(REDUCTION_HALF_NUM_BITS + 1);
    char* d0_key = (char*)malloc(REDUCTION_HALF_NUM_BITS + 1);
    if (c0_key == NULL || d0_key == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    char** c_keys_arr = (char**)malloc(QUARTER_NUM_BITS);
    char** d_keys_arr = (char**)malloc(QUARTER_NUM_BITS);
    if (c_keys_arr == NULL || d_keys_arr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < QUARTER_NUM_BITS; i++) {
        c_keys_arr[i] = (char*)malloc(REDUCTION_HALF_NUM_BITS);
        d_keys_arr[i] = (char*)malloc(REDUCTION_HALF_NUM_BITS);
        if (c_keys_arr[i] == NULL || d_keys_arr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            return 1;
        }
    }

    char** keys_arr = (char**)malloc(QUARTER_NUM_BITS);
    if (keys_arr == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < QUARTER_NUM_BITS; i++) {
        keys_arr[i] = (char*)malloc(REDUCTION_HALF_NUM_BITS);
        if (keys_arr[i] == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            return 1;
        }
    }
    

    //// -----------------------key creation part-----------------------
    char* binary_key = hex_to_binary(key);
    //printf("key before PC1: %s\n", binary_key);
    char* pc1_key = doPC1(binary_key);
    //printf("key after PC1 in binary: %s\n", pc1_key);
    
    memcpy(c0_key, pc1_key, REDUCTION_HALF_NUM_BITS);
    c0_key[REDUCTION_HALF_NUM_BITS] = '\0';
    memcpy(d0_key, pc1_key + REDUCTION_HALF_NUM_BITS, REDUCTION_HALF_NUM_BITS);
    d0_key[REDUCTION_HALF_NUM_BITS] = '\0';

    //printf("c0 key: %s\n", c0_key);
    //printf("d0 key: %s\n", d0_key);

    c_keys_arr = generate_half_keys(c0_key);
    d_keys_arr = generate_half_keys(d0_key);
    /*
    for (int i = 0; i < QUARTER_NUM_BITS; i++) {
        printf("c%d: %s \n", i+1, c_keys_arr[i]);
        printf("d%d: %s \n", i+1, d_keys_arr[i]);
    }
    */
    keys_arr = generate_keys_arr(c_keys_arr, d_keys_arr);
    char** pc2_keys_arr = apply_PC2_to_keys(keys_arr);


    //// -----------------------data creation part-----------------------

    char** dataArr, **ipData;
    char* binaryData = hex_to_binary(data);
    chunks = ceil((double)strlen(binaryData) / NUM_BITS);
    //printf("%s \n", binaryData);
    //printf("%d \n", strlen(binaryData));
    dataArr = create_blocks_from_data(binaryData, chunks);
    //printf("%s", "before IP: \n");
    //printRows(dataArr, chunks);


    //// -----------------------encryption part--------------------------

    dataArr = apply_IP_to_data_array(dataArr, chunks);
    //printf("%s", "after IP: \n");
    //printRows(dataArr, chunks);


    for (int i = 0; i < chunks; i++) {
        dataArr[i] = encryption_rounds(dataArr[i], pc2_keys_arr);
    }

    dataArr = apply_reverse_IP_to_data_array(dataArr, chunks);
    for (int i = 0; i < chunks; i++) {
        dataArr[i] = binary_to_hex(dataArr[i]);
    }

    printf("%s", "\nThe encrypted data: \n");
    char* encrypted_data = concatenate_blocks(dataArr, chunks);
    printf("%s\n", encrypted_data);


    for (int i = 0; i < chunks; i++) {
        dataArr[i] = hex_to_binary(dataArr[i]);
    }

    dataArr = apply_IP_to_data_array(dataArr, chunks);

    for (int i = 0; i < chunks; i++) {
        dataArr[i] = decryption_rounds(dataArr[i], pc2_keys_arr);
    }

    dataArr = apply_reverse_IP_to_data_array(dataArr, chunks);


    char* concatenated = concatenate_blocks(dataArr, chunks);
    concatenated = binary_to_hex(concatenated);
    concatenated = get_hex_ascii(concatenated);
    printf("\nThe decrypted data: \n %s", concatenated);


    //free all and avoid memory leaks
    free(binary_key);
    free(pc1_key);
    free(c0_key);
    free(d0_key);
    for (int i = 0; i < QUARTER_NUM_BITS; i++) {
        free(c_keys_arr[i]);
        free(d_keys_arr[i]);
    }
    free(c_keys_arr);
    free(d_keys_arr);
    free_keys_array(keys_arr, QUARTER_NUM_BITS);
    free_keys_array(pc2_keys_arr, QUARTER_NUM_BITS);
    return 0;
}


