
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <zlib.h>
#include <arpa/inet.h>

#define LIT 0
#define ZLIB 1

void write_zlib_packet(FILE * output, char * buf, uint32_t size) {
    fputc(ZLIB, output);
    fwrite(&size, 4, 1, output);
    fwrite(buf, 1, size, output);
}

void write_lit_packet(FILE * output, char * buf, uint32_t size) {
    fputc(LIT, output);
    fwrite(&size, 4, 1, output);
    fwrite(buf, 1, size, output);
}

int main(int argc, char * argv[]) {
    FILE * input, * output;
    if(argc != 4) {
        printf("Usage: ./regiond c|d <input> <output>\n");
        return 1;
    }
    input = fopen(argv[2], "rb");
    output = fopen(argv[3], "wb");
    if (input == NULL) {
        printf("Could not open the input file.\n");
        return 1;
    }
    if (output == NULL) {
        printf("Could not open the output file.\n");
        return 1;
    }
    if(argv[1][0] == 'c') {
        fseek(input, 0, SEEK_END);
        uint32_t size = ftell(input);
        fseek(input, 0, SEEK_SET);
        unsigned char * buffer = malloc(size);
        fread(buffer, 1, size, input);
        fclose(input);

        long last_raw_data = 0;
        for(long i = 0; i < size; i++) {
            if(buffer[i + 4] == 0x02 && buffer[i + 5] == 0x78 && buffer[i + 6] == 0x9C) {
                uint32_t packet_length = ntohl(*(uint32_t *)&buffer[i]);
                if(i + size > packet_length) {
                    uint8_t * decomp_packet = malloc(packet_length * 32);
                    uint64_t sz = packet_length * 32;
                    if(Z_OK == uncompress(decomp_packet, &sz, &buffer[i + 5], packet_length)) {
                        write_lit_packet(output, &buffer[last_raw_data], i - last_raw_data);
                        write_zlib_packet(output, decomp_packet, sz);
                        last_raw_data = i + 5 + packet_length - 1;
                    }
                    free(decomp_packet); i += 5 + packet_length - 1;
                }
            }
        }

        write_lit_packet(output, &buffer[last_raw_data], size - last_raw_data);
        free(buffer);
    } else if(argv[1][0] == 'd') {
        while(!feof(input)) {
            int type = fgetc(input);
            switch(type) {
                case LIT: {
                    uint32_t size;
                    fread(&size, 4, 1, input);
                    for(int i = 0; i < size; i++)
                        fputc(fgetc(input), output);
                    break;
                }

                case ZLIB: {
                    uint32_t size;
                    fread(&size, 4, 1, input);
                    unsigned char * buf = malloc(size);
                    fread(buf, 1, size, input);
                    uint64_t comp_size = compressBound(size);
                    unsigned char * comp_packet = malloc(comp_size);
                    compress(comp_packet, &comp_size, buf, size);
                    uint32_t sz = ntohl(comp_size + 1);
                    fwrite(&sz, 4, 1, output);
                    fputc(0x02, output);
                    fwrite(comp_packet, 1, comp_size, output);
                    free(buf);
                    free(comp_packet);
                    break;
                }
            }
        }

        fclose(input);
    } else {
        printf("Unknown mode: %s\n", argv[1]);
        return 1;
    }

    fclose(output);
}
