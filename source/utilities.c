#include "../header/utilities.h"

void parse_arguments(struct arguments *arguments,int argc, char **argv) {
    int option;
    while((option = getopt(argc, argv,"d:l:f:o")) != -1){
        switch(option){
            case 'd':
                arguments->dimention = atoi(optarg);
                printf("elaaa=%d\n",atoi(optarg));
                break;
            case 'l':
                arguments->loops = atoi(optarg);
                break;
            case 'f':
                strcpy(arguments->filename,optarg);
                break;
            case 'o':
                arguments->output = true;
                break;
            case '?':
                printf("parse_arguments: unknown option '%c'\n", optopt);
                exit(FAILURE);
        }
    }
}

void print_arguments(struct arguments arguments) {
    printf("dimention = %d\n",arguments.dimention);
    printf("loops = %d\n",arguments.loops);
    printf("filename = %s\n",arguments.filename);
    printf(arguments.output ? "output = true\n":"output = false\n");
}