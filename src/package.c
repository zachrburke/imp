#include <jansson.h>
#include "package.h"

Package package_new(json_t *package) {
	Package p;
	p.files = json_object_get(package, "files");
	return p;
}

Package package_empty() {
	Package p;
	p.empty = 1;
	return p;
}

Package package_get(const char *name) {
	json_error_t error;
	json_t *packages = json_load_file("packages.json", 0, &error);

	if (packages) {
		json_t *package = json_object_get(packages, name);

		if (package) {
			return package_new(package);
		}
	}
	else {
		fprintf(stderr, "%s\n", error.text);
	}

	return package_empty();
}