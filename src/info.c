#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <blorb.h>

void extract(RES_ENTRY res, FILE *blb);

int main(int argc, char *argv[]) {
  FILE *blb;
  UI32 num_res = 0;
  RES_ENTRY res_entries[MAX_RES];

  if(argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(1);
  }

  if(!(blb = fopen(argv[1],"rb"))) {
    perror(argv[1]);
    exit(2);
  }

  fseek(blb, 0, SEEK_SET);

  IFF_HEADER form;
  fread(&form, sizeof(IFF_HEADER), 1, blb);
#ifdef _INTEL
  endian_transform(&form.length);
#endif
  printf("FORM IFF magic number: %s\n", form.iff_magic);
  printf("FORM length: %d\n", form.length);
  printf("FORM type: %s\n\n", form.type);

  RES_INDEX res_idx;
  fread(&res_idx, sizeof(RES_INDEX), 1, blb);

#ifdef _INTEL
  endian_transform(&res_idx.length);
  endian_transform(&res_idx.num_res);
#endif

  printf("Resource Index chunk id: %s\n", res_idx.id);
  printf("Resource Index length: %d\n", res_idx.length);
  printf("Resource Index number of resources: %d\n\n", res_idx.num_res);

  fread(res_entries, sizeof(RES_ENTRY), res_idx.num_res, blb);

  UI32 i;
  for (i = 0; i < res_idx.num_res; i++) {
    if (!strncmp(res_entries[i].usage, "Snd ", 4)
        || !strncmp(res_entries[i].usage, "Pict", 4)
        || !strncmp(res_entries[i].usage, "Exec", 4)) {
      extract(res_entries[i], blb);
    }
  }

  fclose(blb);
  return 0;
}

void extract(RES_ENTRY res_entry, FILE *blb) {
#ifdef _INTEL
  endian_transform(&res_entry.number);
  endian_transform(&res_entry.start);
#endif

  fseek(blb, res_entry.start, SEEK_SET);
  RES_CHUNK res_chunk;
  fread(&res_chunk, sizeof(RES_CHUNK), 1, blb);

#ifdef _INTEL
  endian_transform(&res_chunk.length);
#endif

  printf("Extract resource: usage: \'%s\', number: %d, start: %d\n", res_entry.usage, res_entry.number, res_entry.start);
  printf("  -> type: %s, length:%d\n", res_chunk.type, res_chunk.length);
}

