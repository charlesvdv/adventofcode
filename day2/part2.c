#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct StringList {
    char* value;
    size_t len;
    struct StringList* next;
};

void string_list_push(struct StringList* head, char* value, size_t len) {
    head->next = malloc(sizeof(struct StringList));

    head->next->value = value;
    head->next->len = len;
    head->next->next = NULL;
}

void string_list_free(struct StringList* start) {
    struct StringList* current = start;

    while (current != NULL) {
        free(current->value);

        struct StringList* previous = current;
        current = current->next;
        free(previous);
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

    struct StringList* value_list_start = NULL;
    struct StringList* value_list_head = NULL;
    char* line = NULL;
    size_t len = 0;
    size_t line_len;
    while ((line_len = getline(&line, &len, input_file)) != -1) {
        char* val_cpy = malloc(line_len*sizeof(char));
        strncpy(val_cpy, line, line_len);
        // Replace the '\n' by a '\0' to have a correct C string.
        val_cpy[line_len-1] = '\0';

        if (!value_list_start) {
            // Init the list
            value_list_start = malloc(sizeof(struct StringList));
            value_list_start->value = val_cpy;
            value_list_start->len = line_len;
            value_list_start->next = NULL;

            value_list_head = value_list_start;
        } else {
            string_list_push(value_list_head, val_cpy, line_len);
            value_list_head = value_list_head->next;
        }
    }

    free(line);

    struct StringList* list_outer = value_list_start;
    struct StringList* list_inner = value_list_start;

    char* val_outer;
    char* val_inner;
    int iteration_count = 0;
    while (list_outer != NULL) {
        bool found = false;
        while (list_inner != NULL) {
            unsigned int error_count = 0;
            iteration_count++;
            for (size_t i = 0; i < list_outer->len; i++) {
                if (list_outer->value[i] != list_inner->value[i]) {
                    error_count++;
                }
                if (error_count > 1) {
                    break;
                }
            }
            if (error_count == 1) {
                val_outer = list_outer->value;
                val_inner = list_inner->value;

                found = true;
                printf("found!\n");
                break;
            }
            list_inner = list_inner->next;
        }
        if (found) {
            break;
        }

        list_inner = value_list_start;
        list_outer = list_outer->next;
    }
    printf("Iteration count: %d\n", iteration_count);

    for (int i = 0; i < value_list_start->len; i++) {
        if (val_inner[i] == val_outer[i]) {
            printf("%c", val_inner[i]);
        }
    }
    printf("\n");

    string_list_free(value_list_start);

    fclose(input_file);
    return 0;
}
