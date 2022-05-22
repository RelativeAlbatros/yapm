#define DIR_PERM 0755 // default directory permission

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>

static char *version = "1.20";
static char *license_content = "Copyright <YEAR> <COPYRIGHT HOLDER> \n\n"

"Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: \n\n"

"The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\n"

"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.";

static void die(char *message, ...);
static void debug(char *message);
static void usage(void);
static void create_license(char *message);
static void create_directory_tree(char *project, char *extension);
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
    printf("USAGE: yapm <name> <file-extension> [-c compiler]\n"
           "    -v      prints version \n"
           "    -h      shows this helpful message \n"
           "    -c      manually set a compiler \n");
    exit(1);
}

void create_license(char *project) {
    char buffer[16];
    sprintf(buffer, "%s/LICENSE", project);
    FILE *license = fopen(buffer, "w");
    if (license == NULL)
        die("creating license.");
    fputs(license_content, license);
    fclose(license);
}

void create_directory_tree(char *project, char *extension){
    char buffer[256];
    if (mkdir(project, DIR_PERM) != 0)
        die("creating directory.");
    sprintf(buffer, "%s/src", project);
    mkdir(buffer, DIR_PERM);
    sprintf(buffer, "%s/bin", project);
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
    sprintf(buffer, "PREFIX := /usr/local \n"
                    "SRC    := src/main.%s \n"
                    "\n"
                    "%s: ${SRC} \n"
                    "\t%s -o bin/$@ $? \n"
                    "\n"
                    "run: %s \n"
                    "\t./bin/%s \n"
                    "\n"
                    "install: %s \n"
                    "\tmv bin/%s ${PREFIX}/bin/ \n"
                    "\n"
                    "clean: \n"
                    "\trm -f bin/%s \n"
                    "\n"
                    "debug: ${SRC}\n"
                    "\t@time { %s -g $? -o bin/$@; } \n",
                    extension, project, compiler, project, project, project, project, project, compiler);
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
                        "	printf(\"Hello, world!\\n\"); \n"
                        "	return 0; \n"
                        "} \n");
    } else if (strcmp(extension, "cpp") == 0 ) {
        sprintf(buffer, "#include <iostream> \n"
                        " \n"
                        "int main(int argc, char *argv[]) { \n"
                        "	std::cout << \"Hello, World!\" << std::endl; \n"
                        "	return 0; \n"
                        "} \n");
    } else if (strcmp(extension, "python") == 0) {
        sprintf(buffer, " print(\"Hello, World!\") ");
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
    int opt;
    char buffer[256];
    char project[80];
    char extension[8];
    char compiler[16];

    while ((opt = getopt(argc, argv, ":hv")) != -1) {
        switch (opt) {
        case 'h':
            usage();
            break;
        case 'v':
            die("yapm v%s \n", version);
            break;
        }
    }
    if (argc < 3)
        usage();

    strncpy(project, argv[1], 80);
    strncpy(extension, argv[2], 8);
    if (strcmp(extension, "c") == 0)
        strcpy(compiler, "gcc");
    else if (strcmp(extension, "cpp") == 0)
        strcpy(compiler, "g++");
    else if (strcmp(extension, "python") == 0)
        strcpy(compiler, "python");

    while ((opt = getopt(argc, argv, ":c:")) != -1) {
        switch (opt) {
        case 'c':
            strcpy(compiler, optarg);
            break;
        default:
            die("unknown option: %s \n", opt);
        }
    }

    create_directory_tree(project, extension);
    create_makefile(project, extension, compiler);
    create_main(project, extension);
    create_readme(project);
    create_license(project);
    // init_git(project);

    return 0;
}
