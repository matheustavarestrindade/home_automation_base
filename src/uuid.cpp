#include "uuid.h"

#include <string.h>

#include "esp_system.h"

void uuid_generate(char* out) {
    uint8_t uu[16];

    esp_fill_random(uu, sizeof(uu));

    /* uuid version */
    uu[6] = 0x40 | (uu[6] & 0xF);

    /* uuid variant */
    uu[8] = (0x80 | uu[8]) & ~0x40;

    snprintf(out, UUID_STR_LEN,
             "%02x%02x%02x%02x-%02x%02x-%02x%02x-"
             "%02x%02x-%02x%02x%02x%02x%02x%02x",
             uu[0], uu[1], uu[2], uu[3], uu[4], uu[5], uu[6], uu[7], uu[8], uu[9], uu[10], uu[11],
             uu[12], uu[13], uu[14], uu[15]);
}