#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/**
 * @brief Holds the result of 
 * patterns_match function
 */
typedef struct {
    int found;
    int index;
} match_t;

/**
 * @brief Search for the first match of the patterns in a string
 * 
 * @param patterns patterns (words)
 * @param npatterns number of patterns
 * @param str string
 * @param len length of the string
 * @param from_end 0 first - 1 last
 * @return match_t 
 */
static match_t 
patterns_match(const char* restrict str, int len, int from_end, int npatterns, ...)
{
    match_t res = {
        -1, 0
    };

    // parse va_args
    const char** patterns = (const char**)malloc(sizeof(const char*) * npatterns);
    va_list patterns_arg;
    va_start(patterns_arg, npatterns);
    for(int i = 0; i < npatterns; ++i) {
        const char* pattern = va_arg(patterns_arg, const char* restrict);
        patterns[i] = pattern;
    }

    // initialize length table
    int *plengths = (int*)malloc(sizeof(int) * npatterns);
    for(int pi = 0; pi < npatterns; ++pi) {
        plengths[pi] = strlen(patterns[pi]);
    }
    // search
    for(int i_ = 0; i_ < len; ++i_) {
        int i = (from_end) ? len-1-i_ : i_;
        for(int pi = 0; pi < npatterns; ++pi) {
            const char* pattern = patterns[pi];
            const int n = plengths[pi];
            int pstart = (from_end) ? n-1 : 0;
            if(pattern[pstart] == str[i]) {
                int match = 1;
                for(int j_ = 0; j_ < n; ++j_) {
                    int j = (from_end) ? n-1-j_ : j_;
                    int str_i = (from_end) ? i-j_ : i+j_;
                    if(str_i >= len || str_i < 0) { match = 0; break; }
                    if(pattern[j] != str[str_i]) {
                        match = 0;
                        break;
                    }
                }
                if(match) {
                    free(plengths);
                    res.found = pi;
                    res.index = i+(from_end?-n+1:0);
                    return res;
                }
            }
        }
    }
    free(plengths);
    return res;
}

/**
 * @brief Returns the first number matched in the string. 
 * It searches for digits and written ones (1, one, 2, two, ...)
 *
 * @param str string 
 * @param len length of the string
 * @param from_end 0 first - 1 last
 * @return match_t 
 */
static match_t 
is_number_pattern(char* str, int len, int from_end) {
    match_t r = patterns_match(
        str, len, from_end, 18, 
        "1","2","3","4","5","6","7","8","9",
        "one", "two", "three", "four", "five", "six", "seven", "eight", "nine");
    r.found += 1;
    if(r.found > 9) {
        r.found -= 9;
    }
    return r;
}

void parse_string(char* str)
{
    match_t pattern = is_number_pattern(str, strlen(str), 0);
    if(pattern.found) {
        printf("Found pattern %d \n", pattern.found);
    } else {
        printf("No pattern found");
    }
}

void parse_file(char* filepath) 
{
    FILE* f = fopen(filepath, "r");
    if(f == NULL) {
        perror("fopen\n");
        exit(EXIT_FAILURE);
    }

    char c;
    int sum = 0;
    // not very good but sufficiant
    char line[2048]; int line_length = 0;
    while((c = fgetc(f)) != EOF) {
        line[line_length++] = c;
        line[line_length] = '\0';
        if (c == '\n') {
            line[--line_length] = '\0';
            match_t pattern_first = is_number_pattern(line, line_length, 0);
            int first = pattern_first.found;
            match_t pattern_last  = is_number_pattern(line, line_length, 1);
            int last = pattern_last.found;
            line_length = 0;

            printf("[INFO] %d%d\n", first, last);
            sum += first*10 + last;
            first = last = 0;
        }
    }
    printf("Found: %d\n", sum);
    fclose(f);
}


int main(int argc, char* argv[])
{
    if(argc < 2) {
        fprintf(stderr, "[Error]: Usage ./prog <file>\n");
        return EXIT_SUCCESS;
    }

    parse_file(argv[1]);
    // parse_string(argv[1]);
    
    return EXIT_SUCCESS;
}
