#include <elf.h>
#include <SD.h>

#include "os_elf.h"
#include "psram_alloc.h"
//bool file.seek(pos, SeekSet);
//size_t file.position() const;

uint32_t elf_place_in_ram(File &app, int offset, int filesize, int ramsize)
{
  uint8_t *mem = (uint8_t *)psram_get_blocks((ramsize+PSRAM_BLOCKSIZE-1)/PSRAM_BLOCKSIZE);
  app.seek(offset);
  app.read(mem,filesize);
  return (uint32_t)mem;
}

uint32_t elf_load_sections(File &app, int phoff, int phnum, uint32_t entry)
{
  int i;
  uint32_t *reloc_table;

  uint32_t codeaddr;
  uint32_t codevaddr;
  uint32_t codeend;
  uint32_t code_offset;

  uint32_t dataaddr;
  uint32_t datavaddr;
  uint32_t dataend;
  uint32_t data_offset;

  int num_pointers;
  int entry_offset;
  Elf32_Phdr phdr;
  if(phnum != 2)
  {
    ESP_LOGI("PagerOS","loader assumptions false :(\n");
    return false;
  }
  app.seek(phoff);
  app.read((uint8_t *)&phdr,sizeof(phdr));
  num_pointers = (entry-phdr.p_vaddr)/sizeof(uint32_t);
  phoff += sizeof(phdr); //this is wrong but I like to assume things
  codeaddr = elf_place_in_ram(app,phdr.p_offset,phdr.p_filesz,phdr.p_memsz);
  codevaddr = phdr.p_vaddr;
  codeend = phdr.p_memsz + codevaddr;
  code_offset = codevaddr - codeaddr;
  entry_offset = entry - phdr.p_vaddr;
  entry = codeaddr + entry_offset;

  app.seek(phoff);
  app.read((uint8_t *)&phdr,sizeof(phdr));
  dataaddr = elf_place_in_ram(app,phdr.p_offset,phdr.p_filesz,phdr.p_memsz);
  datavaddr = phdr.p_vaddr;
  dataend = phdr.p_memsz + datavaddr;
  data_offset = datavaddr - dataaddr;
  reloc_table = (uint32_t *)codeaddr;
  for(i=0;i<num_pointers;i++)
  {
    if(reloc_table[i] > codevaddr && reloc_table[i] < codeend)
    {
      ESP_LOGI("PagerOS","reloc %08X",reloc_table[i]);
      reloc_table[i] -= code_offset;
      ESP_LOGI("PagerOS"," to %08X\n",reloc_table[i]);
    }
    else if(reloc_table[i] > datavaddr && reloc_table[i] < dataend)
    {
      ESP_LOGI("PagerOS","reloc %08X",reloc_table[i]);
      reloc_table[i] -= data_offset;
      ESP_LOGI("PagerOS"," to %08X\n",reloc_table[i]);
    }
  }
  ESP_LOGI("PagerOS","new entry %08X\n",entry);
  return entry;
}

uint32_t loadElf(fs::FS &fs, const char *filename)
{
  uint32_t entry;
  Elf32_Ehdr e32_hdr;
  File app = fs.open(filename);

  if(!app)
  {
    ESP_LOGI("PagerOS","App not found\n");
    return 0;
  }
  if(app.read((uint8_t *)&e32_hdr,sizeof(e32_hdr))!=sizeof(e32_hdr))
  {
    ESP_LOGI("PagerOS","App not valid\n");
    goto FAILURE;
  }
  entry = elf_load_sections(app,e32_hdr.e_phoff,e32_hdr.e_phnum,e32_hdr.e_entry);
  app.close();
  return entry;
FAILURE:
  app.close();
  return 0;
}
