#ifndef HTTO_H_
#define HTTO_H_

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
void download_package(const char *url, const char *path);

#endif