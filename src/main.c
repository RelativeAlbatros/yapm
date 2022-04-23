#define DIR_PERM 0711 // default directory permission

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

static void die(char *);
static void debug(char *);
static void usage(void);
static void create_directory_tree(char *, char*);
static void create_makefile(char *, char*);
static void create_main(char *, char *);
static void create_readme(char *);

void die(char *message){
    fprintf(stderr, "[!!] %s\n", message);
    exit(1);
}

void debug(char *message){
    fprintf(stderr, "[??] %s\n", message);
}

void usage(void){
    printf("USAGE: yapm <name> <type>\n");
    exit(1);
}

void create_directory_tree(char *project, char *type){
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

void create_makefile(char *project, char *type){
    char buffer[256];
    sprintf(buffer, "%s/Makefile", project);
    FILE *makefile = fopen(buffer, "w");
    if (makefile == NULL)
        die("creating makefile.");
    sprintf(buffer, "PREFIX := /usr/local\n"
		    "\n"
		    "%s: src/main.%s\n"
		    "	${CC} -o bin/$@ $?\n"
		    "\n"
		    "install: %s\n"
		    "	mv bin/%s ${PREFIX}/bin/ \n"
		    "\n"
		    "clean:\n"
		    "	rm -f bin/%s\n", project, type, project, project, project);
    fprintf(makefile, "%s", buffer);
    fclose(makefile);
}

void create_main(char *project, char *type){
    char buffer[256];
    sprintf(buffer, "%s/src/main.%s", project, type);
    FILE *main = fopen(buffer, "w");
    if (main == NULL)
        die("creating main file.");
    if (strcmp(type, "c") == 0){
        sprintf(buffer, "#include <stdio.h> \n"
                        " \n"
                        "int main(int argc, char **argv) { \n"
                        "	printf(\"Hello, world! \n\");\n"
                        "	return 0; \n"
                        "} \n");
    }
    fprintf(main, "%s", buffer);
    fclose(main);
}

void
create_readme(char *project){
    char buffer[256];
    sprintf(buffer, "%s/README", project);
    FILE *readme = fopen(buffer, "w");
    fprintf(readme, "===[ %s README ]===\n");
    sprintf(buffer, "${EDITOR} %s/README", project);
    system(buffer);
    fclose(readme);
}

int main(int argc, char **argv) {
    if (argc < 3)
        usage();
    char project[80];
    char type[8];
    char buffer[256];
    strncpy(project, argv[1], 80);
    strncpy(type, argv[2], 8);

    create_directory_tree(project, type);
    create_makefile(project, type);
    create_main(project, type);
    create_readme(project);

    return 0;
}
