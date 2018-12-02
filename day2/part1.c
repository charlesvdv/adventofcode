#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define ALPHABET_SIZE 26

struct CountingResult {
    unsigned int two_appearances;
    unsigned int three_appearances;
};

void process_line(char* line, size_t line_len, struct CountingResult* result) {
    char letter_counter[ALPHABET_SIZE] = {0};

    for (int i = 0; i < line_len; i++) {
        size_t index = line[i] - 'a';
        letter_counter[index]++;
    }

    bool has_two = false;
    bool has_three = false;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (letter_counter[i] == 2) {
            has_two = true;
        } else if (letter_counter[i] == 3) {
            has_three = true;
        }
    }

    if (has_two) {
        result->two_appearances++;
    }
    if (has_three) {
        result->three_appearances++;
    }
}

int main(int argc, char const* argv[]) {
    if (argc != 2) {
        return 1;
    }
    const char* filename = argv[1];

    FILE* input_file = fopen(filename, "r");
    if (!input_file) {
        return 1;
    }

    struct CountingResult result = { .two_appearances = 0, .three_appearances = 0};
    char* line = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, input_file) != -1) {
        process_line(line, line_len, &result);
    }

    free(line);

    unsigned int checksum = (result.two_appearances * result.three_appearances);
    printf("%d\n", checksum);

    fclose(input_file);
    return 0;
}
