# Subcircuited version of the common-emitter amplifier

This is the same circuit of the previous demo, but the amplifier
itself has been turned into a subcircuit, and the driving top-level
circuit references it through the Subcircuit component in the "file
components" section of the components library.

Common_Emitter_Driver is the top-level schematic that should be run
through Xyce.  CommonEmmiter_subcircuited.sch is the amplifier
subcircuit.

There are comments in the schematic indicating how to interact with the
subcircuit.

Here we've used the default symbol for the four-port subcircuit, but
if one wanted to make a different symbol, that could be done by
navigating down to the subcircuit component from the driver schematic,
then choosing "Edit Circuit Symbol" from the context menu.

### Exported schematic image
![Created with Qucs "Export as image" option"](export.png)
