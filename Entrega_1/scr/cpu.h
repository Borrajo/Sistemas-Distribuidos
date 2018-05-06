#include <cpuid.h>
#include <string.h>
#include <stdio.h>
char CPUBrandString[0x40];
char* cpu_id()
{
  unsigned int u;
  unsigned int CPUInfo[4] = {0,0,0,0};

  __cpuid(0x80000000, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
  unsigned int nExIds = CPUInfo[0];

  memset(CPUBrandString, 0, sizeof(CPUBrandString));

  for (u = 0x80000000; u <= nExIds; ++u)
  {
      __cpuid(u, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);

      if (u == 0x80000002)
          memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
      else if (u == 0x80000003)
          memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
      else if (u == 0x80000004)
          memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
  }
  return CPUBrandString;
}
