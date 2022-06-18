![image](https://user-images.githubusercontent.com/33568643/174427714-5e855101-8380-416b-b326-6bdef5ac5434.png)

<h1 align="center"> Texture Graph </h2>
<p align="center">
    <a href="#">
        <img src="https://img.shields.io/github/repo-size/oxi-dev0/Texture-Graph" alt="Repo Size">
    </a>
    <a href="#">
        <img src="https://img.shields.io/github/stars/oxi-dev0/Texture-Graph" alt="Stars">
    </a>
    <a href="#">
        <img src="https://img.shields.io/github/forks/oxi-dev0/Texture-Graph" alt="Forks">
    </a>
    <a href="#">
        <img src="https://img.shields.io/github/watchers/oxi-dev0/Texture-Graph" alt="Watchers">
    </a>
</p>

<h3 align="center"> A node-based texture generation program. </h3>
<h6 align="center"> Please Note: Currently the app is quite slow compaired to Substance Designer. I am working on a branch that uses gpu frag shaders to generate the textures rather than lua running on the cpu. </h6>
<br>

Documentation is wip.

<h2> Program Documentation </h2>
<h3> Download </h3>

1. When available, download the latest release from the [Releases](https://github.com/oxi-dev0/Texture-Graph/releases) page.  
2. Extract the zip, and run `TextureGraph.exe`.

<h3> Getting Started </h3>

For some quick examples, in the browser view, click on the open bundle button, and then open `examples/basicgraphs.tgbundle`. In the explorer view, all of the graphs inside the bundle will be listed. If you click on a graph, it will be loaded into the graph editor. You can then press the evaluate button in the graph editor to generate the textures. You can select individual nodes by clicking on them, and to download their texture, in the texture viewer you can click the save button.

> More detailed documentation coming soon.


<h2> Source Documentation </h2>
<h3> Dependencies </h3>
<h6> Note: These are all included in the repo, and all are statically linked into the program </h6>

- [Premake](https://github.com/premake/premake-core) - VS Project Generation
- [SFML](https://github.com/SFML/SFML) - Window + 2D/3D Rendering
- [ImGui](https://github.com/ocornut/imgui) - GUI
- [Clip](https://github.com/dacap/clip) - Cross-Platform Clipboard
- [FPacker](https://github.com/oxi-dev0/fpacker) - Cross-Platform Directory Packing
- [Lua](https://github.com/lua/lua) - Dynamic Node Texture Generation
- [NFD](https://github.com/btzy/nativefiledialog-extended) - Cross-Platform File Dialog
- [spdlog](https://github.com/gabime/spdlog) - Advanced Logging

<h3> Texture Graph Node Language </h3>

This language allows nodes to be programmed for the application. The documentation can be found [here](https://github.com/oxi-dev0/Texture-Graph/tree/main/src/Nodes)  
There is a [VSCode Syntax Highlighting plugin](https://github.com/oxi-dev0/TGNL) available for TGNL. A VS2022 extension will be coming too.
