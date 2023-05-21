#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char main_c_data[] = {
	"#include <stdio.h>\n"
	"\n"
	"int main(void)\n"
	"{\n"
	"	printf(\"Hello World!\\n\");\n"
	"	return 0;\n"
	"}\n"
};

static char *makefile_generate(const char *bin_name)
{
	size_t buf_size = strlen(
			"CC=gcc\n"
			"CFLAGS=-O2 -Wall -Wextra\n"
			"SRC=$(wildcard src/*.c)\n"
			"OBJ=$(patsubst src/%.c,%.o,$(SRC))\n"
			"BIN=\n"
			"\n"
			"default: $(BIN)\n"
			"\n"
			"$(BIN): $(OBJ)\n"
			"	$(CC) $(CFLAGS) $^ -o $@\n"
			"	\n"
			"%.o: src/%.c\n"
			"	$(CC) $(CFLAGS) -c $< -o $@\n"
			"	\n"
			"clean:\n"
			"	rm -f $(BIN) $(OBJ)\n") + strlen(bin_name);

	char *buf = calloc(buf_size, 1);

	sprintf(buf,
		"CC=gcc\n"
		"CFLAGS=-O2 -Wall -Wextra\n"
		"SRC=$(wildcard src/*.c)\n"
		"OBJ=$(patsubst src/%%.c,%%.o,$(SRC))\n"
		"BIN=%s\n"
		"\n"
		"default: $(BIN)\n"
		"\n"
		"$(BIN): $(OBJ)\n"
		"	$(CC) $(CFLAGS) $^ -o $@\n"
		"	\n"
		"%%.o: src/%%.c\n"
		"	$(CC) $(CFLAGS) -c $< -o $@\n"
		"	\n"
		"clean:\n"
		"	rm -f $(BIN) $(OBJ)\n", bin_name);

	return buf;

}

static void mkdirr(const char *dir, mode_t mode)
{
	char tmp[512];
	snprintf(tmp, sizeof(tmp),"%s",dir);
	size_t len = strlen(tmp);
	if(tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	
	char *p = NULL;
	for(p = tmp + 1; *p; p++) {
		if(*p != '/')
			continue;
		*p = 0;
		mkdir(tmp, mode);
		*p = '/';
	}

	mkdir(tmp, mode);
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		fprintf(stderr, "You must include a "
				"path to generate project\n");
		exit(EXIT_FAILURE);
	}

	if(argc < 3) {
		fprintf(stderr, "You must include a name for the project\n");
		exit(EXIT_FAILURE);
	}

	const char *input_dir_rel = argv[1];
	mkdirr(input_dir_rel, 0777);
	const char *project_name = argv[2];
	char *input_dir_abs = realpath(input_dir_rel, NULL);
	printf("Generating project '%s' at '%s'\n",
			project_name, input_dir_abs);
	if(!input_dir_abs) {
		fprintf(stderr, "Failed to parse "
				"'%s' as absolute path.\n", input_dir_rel);
		exit(EXIT_FAILURE);
	}

	char buf[512];
	strcpy(buf, input_dir_abs);
	const char *makefile_path = strcat(buf, "/Makefile");
	FILE *makefile = fopen(makefile_path, "w");
	char *makefile_data = makefile_generate(project_name);
	fwrite(makefile_data, strlen(makefile_data) - 1, 1, makefile);
	fclose(makefile);
	printf("Created Makefile at '%s'\n", makefile_path);

	bzero(buf, 512);
	strcpy(buf, input_dir_abs);
	const char *include_path = strcat(buf, "/include");
	mkdir(include_path, 0777);
	printf("Created include/ at '%s'\n", include_path);

	bzero(buf, 512);
	strcpy(buf, input_dir_abs);
	const char *src_path = strcat(buf, "/src");
	mkdir(src_path, 0777);
	printf("Created src/ at '%s'\n", src_path);

	bzero(buf, 512);
	strcpy(buf, input_dir_abs);
	const char *main_c_path = strcat(buf, "/src/main.c");
	FILE *main_c = fopen(main_c_path, "w");
	fwrite(main_c_data, sizeof(main_c_data) - 1, 1, main_c);
	fclose(main_c);
	printf("Created main.c at '%s'\n", main_c_path);

	free(makefile_data);
	free(input_dir_abs);
	return 0;
}
