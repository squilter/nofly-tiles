#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

uint8_t NOFLY_TYPE_END             = 0;
uint8_t NOFLY_TYPE_CIRCLE          = 1;
uint8_t NOFLY_TYPE_POLYGON         = 2;
uint8_t NOFLY_TYPE_WARN_ONLY       = 4;
uint8_t NOFLY_TYPE_TIME_RESTRICTED = 8;

int main(int argc, char* argv[]) {
    if(argc < 2){
        printf("This program reads a nofly_tile and prints it to the screen in human-readable format.\n");
        printf("Usage: ./parser <filename>\n");
        return 1;
    }
    
    // read tile into memory
    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);
    uint8_t *tile_ptr = malloc(fsize + 1);
    void * tile_start = tile_ptr; //remember the start of the memory to free() later
    fread(tile_ptr, fsize, 1, f);
    fclose(f);
    tile_ptr[fsize] = 0;//add an extra null for good measure

    // loop through nofly's in the tile and print out info.
    while(1){
        if(*tile_ptr == NOFLY_TYPE_END){
            printf("Reached end-of-tile. Exiting.\n");
            break;
        }else if(*tile_ptr & NOFLY_TYPE_CIRCLE){
            uint8_t warn_only = *tile_ptr & NOFLY_TYPE_WARN_ONLY;
            uint8_t time_restricted = *tile_ptr & NOFLY_TYPE_TIME_RESTRICTED;
            uint32_t start_time = 0;
            uint32_t end_time = 0;
            int32_t lat = 0;
            int32_t lon = 0;
            uint16_t radius = 0;
            
            tile_ptr = (uint8_t*) tile_ptr + 1;
            if(time_restricted){
                start_time = *((uint32_t*) tile_ptr);
                end_time = *((uint32_t*) tile_ptr + 1);
                tile_ptr = (uint8_t*) tile_ptr + 8;
            }
            lat = *((int32_t*) tile_ptr);
            lon = *((int32_t*) tile_ptr + 1);
            radius = *((uint16_t*) tile_ptr + 4);
            
            tile_ptr = (uint8_t*) tile_ptr + 10;

            if(time_restricted){
                printf("From %u to %u: ",start_time,end_time);
            }
            printf("%s Circle. Lat: %f, Lon: %f, Radius: %u\n", (warn_only ? "Warn" : "Stay-Out"), lat/10000000.0, lon/10000000.0, radius);
        }else if(*tile_ptr & NOFLY_TYPE_POLYGON){
            uint8_t warn_only = *tile_ptr & NOFLY_TYPE_WARN_ONLY;
            uint8_t time_restricted = *tile_ptr & NOFLY_TYPE_TIME_RESTRICTED;
            uint32_t start_time = 0;
            uint32_t end_time = 0;
            int32_t lat = 0;
            int32_t lon = 0;
            uint8_t coord_pairs = 0;

            tile_ptr = (uint8_t*) tile_ptr + 1;
            if(time_restricted){
                start_time = *((uint32_t*) tile_ptr);
                end_time = *((uint32_t*) tile_ptr + 1);
                tile_ptr = (uint8_t*) tile_ptr + 8;
            }
            coord_pairs = *((uint8_t*) tile_ptr);
            tile_ptr = (uint8_t*) tile_ptr + 1;

            if(time_restricted){
                printf("From %u to %u: ",start_time,end_time);
            }
            printf("%s Polygon, defined by %u coord pairs:\n", (warn_only ? "Warn" : "Stay-Out"), coord_pairs);

            for(int i = 0; i < coord_pairs; i++){
                lat = *((int32_t*) tile_ptr);
                lon = *((int32_t*) tile_ptr + 1);
                printf("\t%u. Lat: %f, Lon: %f\n", i, lat/10000000.0, lon/10000000.0);
                tile_ptr = (uint8_t*) tile_ptr + 8;
            }
            break;
        }

    }
    free(tile_start);
    return 0;
}
