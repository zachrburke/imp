#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <curl/curl.h>
#include <jansson.h> 

#include "package.h"
#include "http.h"

_Bool verbose = 0;
char *baseDir = "lib/plugins";

_Bool dir_exists(const char *path) {
	DIR *dirp;

	if ((dirp = opendir(path)) == NULL) {
		return 0;
	}

	return 1;
}

void make_sure_dir_exists(const char *path) {
	size_t length = strlen(path) > FILENAME_MAX ? FILENAME_MAX : strlen(path);
	char pathCopy[strlen(path) + strlen(baseDir) + strlen("./")];
	char pathSoFar[FILENAME_MAX];
	char* pch;
	DIR *dirp;

	// include start directory
	snprintf(pathCopy, sizeof pathCopy, "%s/%s", baseDir, path);

	pch = strtok(pathCopy, "/");

	snprintf(pathSoFar, sizeof pathSoFar, "./%s", pch);

	while(pch != NULL) {
		if (!dir_exists(pathSoFar))
			mkdir(pathSoFar, 0755);

		pch = strtok(NULL, "/"); 
		
		snprintf(pathSoFar, sizeof pathSoFar, "%s%s%s", pathSoFar, "/", pch);
	}
}

void showUsage() {
	printf("usage: ipm [command] [arg]\n");
	printf("commands:\n");
	printf("\tnew - create a new impactjs project\n");
	printf("\tremote [downloadurl] - set url for your impact source files\n");
	printf("\tinstall [packagename] - install specified package\n"); 
}

int remote(const char *remoteUrl) {
	if (!remoteUrl) {
		showUsage();
		fprintf(stderr, "missing url for remote command, please enter the url of your personal impact download\n");
		return 1;
	}

	json_error_t error;
	json_t *settings = json_load_file(".ipm/settings.json", 0, &error);
	json_t *impact_url;
	int codeMsg;

	if (settings) {
		impact_url = json_object_get(settings, "impact_url");
		json_string_set(impact_url, remoteUrl);
		json_object_set(settings, "impact_url", impact_url);
		json_dump_file(settings, ".ipm/settings.json", JSON_INDENT(4));
	}
	else {
		fprintf(stderr, "%s\n", error.text);
	}

	return 0;
}

int new() {
	return 0;
}

int install(char *packageName) {
	if (!packageName) {
		showUsage();
		fprintf(stderr, "missing package name for install command\n");
		return 2;
	}

	DIR *dirp;
	Package package = package_get(packageName);

	if (package.empty) {
		fprintf(stderr, "could not find %s package\n", packageName);
		return 3;
	}

	json_t *file;
	int index;
	json_t *url, *path, *name;
	char fullPath[FILENAME_MAX];

	json_array_foreach(package.files, index, file) {
		url = json_object_get(file, "url");
		path = json_object_get(file, "path");
		name = json_object_get(file, "name");

		if (verbose) {
			printf("File URL: %s\n", json_string_value(url));
			printf("File Path: %s\n", json_string_value(path));
			printf("File Name: %s\n", json_string_value(name));
		}

		strcpy(fullPath, json_string_value(path));
		make_sure_dir_exists(fullPath);

		snprintf(fullPath, sizeof fullPath, "./%s/%s/%s", baseDir, json_string_value(path), json_string_value(name));

		download_package(json_string_value(url), fullPath);
	}

	return 0;
}

int delete(char *packageName) {
	if (!packageName) {
		showUsage();
		fprintf(stderr, "missing package name for remove command\n");
		return 2;
	}

	Package package = package_get(packageName);

	if (package.empty) {
		fprintf(stderr, "could not find %s package\n", packageName);
		return 3;
	}

	json_t *file;
	int index;
	json_t *url, *path, *name;
	char fullPath[FILENAME_MAX];

	json_array_foreach(package.files, index, file) {
		url = json_object_get(file, "url");
		path = json_object_get(file, "path");
		name = json_object_get(file, "name");

		if (verbose) {
			printf("File URL: %s\n", json_string_value(url));
			printf("File Path: %s\n", json_string_value(path));
			printf("File Name: %s\n", json_string_value(name));
		}

		snprintf(fullPath, sizeof fullPath, "./%s/%s", baseDir, json_string_value(path));

		if (!dir_exists(fullPath)) {
			fprintf(stderr, "Package: %s not found locally.  %s not found (nothing to remove)\n", packageName, fullPath);
			continue;
		}

		snprintf(fullPath, sizeof fullPath, "./%s/%s/%s", baseDir, json_string_value(path), json_string_value(name));

		remove(fullPath);
	}	

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		showUsage();
		return 1;
	}

	verbose = 0;

	char *command = argv[1];
	int exitCode = 0;

	if (strcmp(command, "remote") == 0) {
		exitCode = remote(argv[2]);
	}
	else if (strcmp(command, "new") == 0) {
		exitCode = new();
	}
	else if (strcmp(command, "install") == 0) {
		exitCode = install(argv[2]);
	}
	else if (strcmp(command, "remove") == 0) {
		exitCode = delete(argv[2]);
	}

	return exitCode;
}
