
SET(XYCEDIG_resources
A2DBRIDGE.sym    AND4.sym         D2ABRIDGEX4.sym  INV.sym      MUX8TO1.sym  NOR4.sym      PATGENX128.sym  XOR.sym
A2DBRIDGEX2.sym  ANDOR4X2.sym     D2ABRIDGEX8.sym  LOGIC0.sym   Makefile.am  NXOR.sym      PATGENX2.sym
A2DBRIDGEX4.sym  BUF.sym          FADDER.sym       LOGIC1.sym   NAND2.sym    OR2.sym       PATGENX32.sym
A2DBRIDGEX8.sym  D2ABRIDGE.sym    GEN0OR1X4.sym    MUX2TO1.sym  NAND4.sym    OR4.sym       PATGENX4.sym
AND2.sym         D2ABRIDGEX2.sym  GEN0OR1X8.sym    MUX4TO1.sym  NOR2.sym     PATGENX1.sym  PATGENX8.sym
)

INSTALL( FILES ${XYCEDIG_resources} DESTINATION share/qucs/library/XyceDigital )

