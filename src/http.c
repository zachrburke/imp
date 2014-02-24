#include <curl/curl.h>

#include "http.h"

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

void download_package(const char *url, const char *path) {
	CURL *curl;
	CURLcode res;
	char *buffer;
	FILE *fp;

	printf("downloading package %s to %s\n", url, path);

	curl = curl_easy_init();
	fp = fopen(path, "w");
	if(curl && fp) {
  		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
			printf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

		curl_easy_cleanup(curl);
		fclose(fp);
	}
}