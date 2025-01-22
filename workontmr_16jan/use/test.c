#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
    for (char **env = envp; *env != NULL; env++) {
        printf("%s\n", *env);
    }
    return 0;
}
