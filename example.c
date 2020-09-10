#include <stdbool.h>

#include "gtkwave/fstapi.h"


#define FST_TS_S    0
#define FST_TS_MS  -3
#define FST_TS_US  -6
#define FST_TS_NS  -9
#define FST_TS_PS  -12

int main() {
  void* fst = fstWriterCreate("example.fst", /* use_compressed_hier */ 1);
  fstWriterSetPackType(fst, FST_WR_PT_LZ4);
  fstWriterSetTimescale(fst, FST_TS_NS);
#ifdef FST_WRITER_PARALLEL
  fstWriterSetParallelMode(fst, true);
#endif

  fstWriterSetComment(fst, "Created by bluecmd/fst-example");
  fstWriterSetDate(fst, "2020-09-10");

  const char *enumNames[] = { "STATE1", "STATE2", "STATE3" };
  const char *enumValues[] = { "001", "010", "100" };
  fstEnumHandle enumMap = fstWriterCreateEnumTable(
      fst, "ExampleEnum", /* elements */ 3, 0, enumNames, enumValues);

  fstHandle varInt, varEnum, varString;

  // Create a tree like this:
  //
  //  Example
  //   |
  //   |-Function
  //   |
  //   |-Aliases
  //
  fstWriterSetScope(fst, FST_ST_VCD_PROGRAM, "Example", NULL);
  fstWriterSetScope(fst, FST_ST_VCD_FUNCTION, "Function", NULL);

  varInt    = fstWriterCreateVar(fst, FST_VT_VCD_INTEGER, FST_VD_INPUT, 8, "integer", /* alias */ 0);
  varString = fstWriterCreateVar(fst, FST_VT_GEN_STRING, FST_VD_OUTPUT, 0, "string", /* alias */ 0);

  // Activate enum for next variable defined
  fstWriterEmitEnumTableRef(fst, enumMap);
  varEnum   = fstWriterCreateVar(fst, FST_VT_SV_ENUM, FST_VD_INPUT, 3, "enum", /* alias */ 0);

  fstWriterSetUpscope(fst);
  fstWriterSetScope(fst, FST_ST_VCD_PACKAGE, "Aliases", NULL);
  // Alias of the "integer" variable above
  fstWriterCreateVar(fst, FST_VT_VCD_INTEGER, FST_VD_INPUT, 8, "integer", varInt);

  fstWriterEmitTimeChange(fst, 0);
  fstWriterEmitValueChange(fst, varInt, "zzzzxxxx");
  fstWriterEmitTimeChange(fst, 1);
  for (int i = 2; i < 1000;) {
    fstWriterEmitValueChange32(fst, varInt, 8, 100 + i);
    fstWriterEmitVariableLengthValueChange(fst, varString, "str1", 4);
    fstWriterEmitValueChange(fst, varEnum, "001");
    fstWriterEmitTimeChange(fst, i++);
    fstWriterEmitValueChange32(fst, varInt, 8, 100 + i);
    fstWriterEmitVariableLengthValueChange(fst, varString, "str2", 4);
    fstWriterEmitValueChange(fst, varEnum, "010");
    fstWriterEmitTimeChange(fst, i++);
    fstWriterEmitValueChange32(fst, varInt, 8, 100 + i);
    fstWriterEmitVariableLengthValueChange(fst, varString, NULL, 0); // z-equivalent for strings
    fstWriterEmitValueChange(fst, varEnum, "100");
    fstWriterEmitTimeChange(fst, i++);
  }

  fstWriterClose(fst);
}
