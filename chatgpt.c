#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

void send_to_ai(const char *data) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "Authorization: Bearer ");

        char json_payload[1024];
        snprintf(json_payload, sizeof(json_payload), "{\"model\": \"gpt-3.5-turbo\",\"messages\": [{\"role\": \"user\", \"content\": \"%s\"}]}",
                 data);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
        
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

int main() {
    printf("Hello, World!\n");
    const char *output = "What is the weather like today?"; // Example C program output
    send_to_ai(output);
    return 0;
}

