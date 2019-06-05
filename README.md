# obj-2-ramses

Import OBJ files, export binary RAMSES files, preview the 3D scene on a ramses renderer.

This is an experimental demo illustrating how .obj files could be converted to ramses scenes and stored in binary ramses files. The project is developed within the Google Summer of Code 2019 project to develop tools for the RAMSES distributed rendering engine.

The .obj format is quite old, doesn't support all modern features of OpenGL/DX of recent generations, and is very limited in what it can store. However, it is a very simple text-based format, easy to understand and debug, and the code to parse it is can be kept minimal. Use this repository as an illustration how 3D rendering data can be converted to ramses.
