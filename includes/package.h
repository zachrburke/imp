#ifndef PACKAGE_H_
#define PACKAGE_H_

typedef struct {
	json_t *files;
	_Bool empty;
} Package;

Package package_new(json_t *package);
Package package_empty();
Package package_get(const char *name);

#endif