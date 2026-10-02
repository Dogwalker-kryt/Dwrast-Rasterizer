#include "obj_parser.h"

void get_objects_file_content(FILE *file) {
    char line[512];
    
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '#') {
            continue;
        }

        if (strcmp(line[0], "v") == 0) {
            
        }
    }
}