#define DIR_PERM 0711 // default directory permission

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

static char *version = "1.0.0";

static void die(char *message, ...);
static void debug(char *message);
static void usage(void);
static void create_directory_tree(char *, char*);
static void create_makefile(char *project, char *extension, char *compiler);
static void create_main(char *project, char *extension);
static void create_readme(char *project);
static void init_git(char *project);

void die(char *message, ...){
    va_list ap;
    va_start(ap, message);
    vfprintf(stderr, message, ap);
    va_end(ap);
    exit(1);
}

void debug(char *message){
    fprintf(stderr, "[??] %s\n", message);
}

void usage(void){
    printf("USAGE: yapm <name> -e <extension> -c <compiler>\n");
    exit(1);
}

void create_directory_tree(char *project, char *extension){
    char buffer[256];
    if (mkdir(project, DIR_PERM) != 0)
        die("creating directory.");
    sprintf(buffer, "%s/src", project);
    mkdir(buffer, DIR_PERM);
    sprintf(buffer, "%s/bin", project);
    mkdir(buffer, DIR_PERM);
    sprintf(buffer, "%s/lib", project);
    mkdir(buffer, DIR_PERM);
    sprintf(buffer, "%s/include", project);
    mkdir(buffer, DIR_PERM);
    memset(buffer, 0, sizeof(buffer));
}

void create_makefile(char *project, char *extension, char *compiler){
    char buffer[256];
    sprintf(buffer, "%s/Makefile", project);
    FILE *makefile = fopen(buffer, "w");
    if (makefile == NULL)
        die("creating makefile.");
    sprintf(buffer, "PREFIX := /usr/local\n"
		    "\n"
		    "%s: src/main.%s \n"
		    "	%s -o bin/$@ $?\n"
		    "\n"
		    "install: %s \n"
		    "	mv bin/%s ${PREFIX}/bin/ \n"
		    "\n"
		    "clean: \n"
		    "	rm -f bin/%s \n", project, extension, compiler, project, project, project);
    fprintf(makefile, "%s", buffer);
    fclose(makefile);
}

void create_main(char *project, char *extension){
    char buffer[256];
    sprintf(buffer, "%s/src/main.%s", project, extension);
    FILE *main = fopen(buffer, "w");
    if (main == NULL)
        die("creating main file.");
    if (strcmp(extension, "c") == 0){
        sprintf(buffer, "#include <stdio.h> \n"
                        " \n"
                        "int main(int argc, char **argv) { \n"
                        "	printf(\"Hello, world!\n\"); \n"
                        "	return 0; \n"
                        "} \n");
    } else if (strcmp(extension, "cpp") == 0 ) {
        sprintf(buffer, "#include <iostream> \n"
                        " \n"
                        "int main(int argc, char *argv[]) { \n"
                        "	std::cout << \"Hello, World!\" << std::endl; \n"
                        "	return 0; \n"
                        "} \n");
    }
    fprintf(main, "%s", buffer);
    fclose(main);
}

void create_readme(char *project){
    char buffer[256];
    sprintf(buffer, "%s/README", project);
    FILE *readme = fopen(buffer, "w");
    fclose(readme);
}

void init_git(char *project){
    chdir(project);
    if (fork() == 0) {
        // TODO: fix git init
        if (execlp("git", "init", NULL) == -1)
            perror("initializing git");
    }
    chdir("..");
}

int main(int argc, char **argv) {
    if (argc < 2)
        usage();
    int opt;
    char buffer[256];
    char project[80];
    char extension[8];
    char compiler[16];
    strncpy(project, argv[1], 80);
    strncpy(extension, "c", 8);
    strcpy(compiler, "gcc");
    while ((opt = getopt(argc, argv, ":hvt:c:")) != -1) {
        switch (opt) {
        case 'h':
            usage();
            break;
        case 'v':
            die("yapm v%s", version);
            break;
        case 't':
            strncpy(extension, optarg, 8);
            break;
        case 'c':
            strncpy(compiler, optarg, 16);
            break;
        default:
            die("unknown option: %s", opt);
        }
    }

    create_directory_tree(project, extension);
    create_makefile(project, extension, compiler);
    create_main(project, extension);
    create_readme(project);
    // init_git(project);

    return 0;
}
