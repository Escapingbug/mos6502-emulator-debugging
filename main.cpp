#include "mos6502.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

typedef void (*BusWrite)(uint64_t, uint64_t);
typedef uint8_t (*BusRead)(uint64_t);

char mem[0x17000];

void MemWrite(uint64_t where, uint64_t what) {
  if (where == 0xabcd) {
	write(1, &what, 1);
	return;
  }

  //fprintf(stderr, "mem[0x%llx] = 0x%llx\n", where, what);
  mem[where] = what;
}

uint8_t MemRead(uint64_t where) {
  if (where == 0xabcd) {
	char buf;
	read(0, &buf, 1);
	return buf;
  } else {
	//fprintf(stderr, "read mem[0x%llx]\n", where);
	return mem[where];
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
	printf("usage: %s bin", argv[0]);
	return 1;
  }

  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
	perror("open");
	return 1;
  }

  read(fd, mem, 0xf000);

  mos6502 emu((BusRead)MemRead, (BusWrite)MemWrite);
  emu.Reset();
  emu.Run(0xf000);
  return 0;
}
