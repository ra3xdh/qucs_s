### Installation of xerces-c 3.3

[Follow instruction from apache](https://xerces.apache.org/xerces-c/install-3.html)

### Installation of xsd

[Follow instruction from codesynthesis](https://codesynthesis.com/projects/xsd/)

### Re-creation of xsd-generated files

~~~
cd qucs/components/xml
xsd cxx-tree --namespace-map =component::xml ../../../library/components/Component.xsd
~~~
