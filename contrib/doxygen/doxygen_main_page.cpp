/* NOTE:  This file has a ".c" extension to make it visible to Doxygen.
 */

/*! \mainpage Qucs GUI documentation

\section intro Introduction
This documentation is generated from the qucs sourcecode using Doxygen.
Diagrams in the manual are produced by the <kbd>dot</kbd> utility--normally distributed as part of the <kbd>graphviz</kbd> package.

\section hacking Hacking on this documentation
Eventually there will be a style guide for documenting,
with an example of a one of the source files documented "by the book".
Until then, feel free to start documenting or playing with doxygen configuration.

\subsection config Doxygen Configuration
To edit the doxygen configuration, you can use (from the qucs-doc top directory):<br>
- <kbd> cd doxygen; doxywizard doxygen.cfg &</kbd>

The default doxygen configuration only produces documentation for modules and data structures
that have been explicitly tagged in the source code.
More detailed documentation may be obtained by modifing the doxygen configuration file (<kbd>.../doxygen/doxygen.cfg</kbd>).

Options of particular interest include:<br>
- <kbd>INPUT</kbd> which identifies the code to be scanned.
	The default configuration assumes that the <kbd>qucs</kbd>, <kbd>qucs-core</kbd>
	and <kbd>qucs-doc</kbd> top level directories are siblings.
- <kbd>EXTRACT_ALL</kbd> which will document all software components whether tagged or not.
- <kbd>SOURCE_BROWSER</kbd> which will embed implementation code in the documentation.

Enabling the latter two options will significantly increase the size of this manual, as well as the time it takes to generate it.

\subsection reference Doxygen reference documentation
The Doxygen web site (<kbd>http://www.doxygen.org/</kbd>) has a complete user manual.
For the impatient, the most interesting sections are:
- How to write documentation blocks for files, functions, variables, etc.: <kbd>http://www.doxygen.org/docblocks.html</kbd>
Be sure to include a file documentation block (\@file or \\file) at the top of your file.
Without it your documentation will not appear in the html.
- List of the special commands you can use within your documentation blocks: <kbd>http://www.doxygen.org/commands.html</kbd>

\section contact Contacts

\subsection web Web Site
News about Qucs as well as the latest version can always be found at the sourceforge project page
http://qucs.sourceforge.net/

\subsection email Email
If you have any suggestions concerning this documentation, do not hesitate to send suggestions to
qucs-devel (see <kbd>http://qucs.sourceforge.net/contact.shtml</kbd> for details)

\author
Guilherme Brondani Torri  (mailto:guitorri@gmail.com)
 */
