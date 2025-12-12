## XML-based components

### Installation of xerces-c 3.3  is needed for qucs-s building:
[Follow instruction from apache](https://xerces.apache.org/xerces-c/install-3.html)

### Installation of xsd is needed for qucs-s building:
[Follow instruction from codesynthesis](https://codesynthesis.com/projects/xsd/)

### Examples of XML-based components
Examples of XML-based components can be found under
[IHP Open PDK](https://github.com/IHP-GmbH/IHP-Open-PDK/tree/main/ihp-sg13g2/libs.tech/qucs/symbols)

## Basic principle for defining XML-based components:

The class XmlComponent is the internal representations of XML based qucs-s components.
The associated XML Schema can be found under library/components/Component.xsd.
Automatic code generation for schema-based read-in and validation of XML components is
integrated in build system.

### Top level definition:

A typical top-level definition looks like:

    <?xml version="1.0"?>
    <Component
      xsi:noNamespaceSchemaLocation="Component.xsd"
      xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
      library="IHP 131 BiCmos components" names="Diode" schematic_id = "D">
        <Description>
          Diode
        </Description>

Besides the required XML statements there are the following items:

 - *library*: defines the category under which this devices shall be listed in the qucs-s gui,
  the category will we created in the gui if it don't exist.

 - *names*: the name(s) of the device(s) of this component, can be a comma-separated list for
   multi-component definitions, see below.

 - *schematic\_id*: schematic identifier for this device in the qucs-s gui

 - *description*: short description of the component for documentation purpose

### Symbol sharing:

The information for graphic representation of a component can be outsourced to an external file
and be reused by other components too. In that case the Symbol section only refers that file,
e.g.:

    <Symbol id="Standard">
        <File>{QUCS_S_COMPONENTS_LIBRARY}/Resistor.sym</File>
    </Symbol>

{QUCS\_S\_COMPONENTS\_LIBRARY} will be resolve to *path\_of\_installed\_qucs\_s\_components*/components',
e.g. /usr/local/share/qucs-s/components/ under Linux.

Symbol elements can be defined conditional depending of the type (name) of the device, e.g.

    <Line x1="-9" y1="0" x2="-4" y2="-5" color="#800000" width="2" style="1" condition="type=nmos"/>

### Multi-device definition
To save repetition of similiar definitions of variants of a device-family these variants can be defined
as single source using list of names, e.g. defining the resistor-variants rhigh, rsil, rppd, ptap1
and ntap1:

    <?xml version="1.0"?>
    <Component
      xsi:noNamespaceSchemaLocation="Component.xsd"
      xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
      library="IHP 131 BiCmos components" names="rhigh,rsil,rppd,ptap1,ntap1" schematic_id = "R">

The variant names can then be used to define conditions, see below.

### Parameter:
A simple parameter definition looks like:

    <Parameter name="model" unit="" default_value="rhigh" show="true">
      <Description>[rsil,rppd,rhigh,ptap1,ntap1]</Description>
    </Parameter>

It will create a visible parameter *model* with the value *rhigh* and a choice-set of
*rsil, rppd, rhigh, ptap1* and *ntap1*.

A name-conditional parameter definition looks like:

    <Parameter name="W" unit="um" default_value="0.78" show="true" condition="name=ntap1,ptap1">
      <Description>Width</Description>
    </Parameter>

It will create a visible parameter *W* only for the devices *ntap1* and *ptap1*. These names shall be
included in the *names*-attribute of the *Component*-item.

Muparser (https://beltoforion.de/en/muparser) compatible parameter equations are supported for
XML components, e.g.

    <Parameter name="Vrms" unit="V" equation="sqrt(Z/1000.0)*10^(P/20)" show="true">
      <Description>RMS Voltage</Description>
    </Parameter>
    <Parameter name="Z" unit="Ohm" default_value="50" show="true">
      <Description>Port impedance</Description>
    </Parameter>
    <Parameter name="P" unit="dBm" default_value="0" show="true">
      <Description>(Available) AC power (dBm)</Description>
    </Parameter>

### Netlisting:
Netlist generation of XML components is template based. There is dedicated template possible for
every type of Spice, Xyce, CDL and Qucsator.
The following pre-defined netlist placeholder will be supported:
 - {PartCounter} : will be replaced by the qucs\_s internal component counter
 - {nets} : will be replace be the component-assigned nets

Component defined parameters can be referred as {parameter\_name}, e.g.

    <NgspiceNetlist value = 'VP{PartCounter} {nets} dc 0 ac {P} SIN(0 {P} {f}) portnum {NumPorts} z0 {Z}'>

An alternative to dedicated netlists is referencing another netlist template and usage of
type-conditional statements. Referencing another netlist template is done by the template value
{netlist\_name}, e.g.

    <CDLNetlist value = '{NgspiceNetlist}'> </CDLNetlist>

In that case the Ngspice netlist template will be taken as base for CDL netlisting of this component.
Type-conditional statements have the form

1. '{{statement}}::condition' or
2. '{{statement}}::!condition'

with condition = {*CDL*|*XYCE*|*SPICE*}.<br><br>

(1.) will be resolved so that the statement will be used only if the actual netlisting type
matches the condition, e.g. {{p={p}}}::*CDL* will be resolved to p='*value\_of\_parameter\_p*' if a
CDL netlist shall be generated, otherwise to an empty string.<br><br>
(2.) will be resolved so that the statement will be used only if the actual netlisting type
don't matches the condition, e.g. {{p={p}}}::*!CDL* will be resolved to p='*value\_of\_parameter\_p*'
if a netlist of other type then CDL shall be generated, otherwise to an empty string.

#### Conditions for netlist parameters:

For further netlist parameter restrictions the following conditions can be used:

 - Parameter name comparison to *Component* attribute *schematic\_id*:

    ```
        ...
        library="IHP 131 BiCmos components" names="rhigh,rsil,rppd,ptap1,ntap1" schematic_id = "R">
        <NgspiceNetlist value="{{{Letter}}}::schematic_id='nequal'... > </NgspiceNetlist>
        ...
        <Parameter name="Letter" unit="" default_value="X" show="false">
          <Description>[R,X]</Description>
        </Parameter>
        ...
    ```
   The value for the parameter *Letter* will become part of the netlist only if the this value
   is not equal to the value of *schematic_id.*, in this case if it's *X*.

 - Non-empty parameter value restriction:

    ```
        ...
        <NgspiceNetlist value="...{{W={W}}}::W='nonempty'..."></NgspiceNetlist>
        ...
    ```
   The term W=*value\_of\_W* will become part of the netlist only if *value\_of\_W* is not empty.
