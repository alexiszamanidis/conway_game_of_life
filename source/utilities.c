#include "../header/utilities.h"

void parse_arguments(struct arguments *arguments,int argc, char **argv) {
    int option;
    while((option = getopt(argc, argv,"d:g:i:t:o")) != -1){
        switch(option){
            case 'd':
                arguments->dimension = atoi(optarg);
                break;
            case 'g':
                arguments->generations = atoi(optarg);
                break;
            case 'i':
                strcpy(arguments->inputfile,optarg);
                break;
            case 't':
                arguments->threads = atoi(optarg);
                break;
            case '?':
                printf("parse_arguments: unknown option '%c'\n", optopt);
                exit(FAILURE);
        }
    }
}

void print_arguments(struct arguments arguments) {
    printf("dimension = %d\n",arguments.dimension);
    printf("generations = %d\n",arguments.generations);
    printf("inputfile = %s\n",arguments.inputfile);
    printf("threads = %d\n",arguments.threads);
}

int is_perfect_square(int number) {
    double root = sqrt((double)number);

    if( (root - floor(root)) == 0)
        return root;
    else
        return -1;
}