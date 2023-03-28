<img width="1440" alt="V1 0-Progress" src="https://user-images.githubusercontent.com/33568643/221988685-cf63a443-9fec-4519-be77-234a89295e3e.png">

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
<h6 align="center"> Please Note: The app is, currently, quite slow in comparison to other texture generation programs, such as Substance Designer. </h6>
<br>

## About Texture Graph
Often I find the need to create textures for my game projects, and hand-drawing textures takes a lot of time. Therefore I created this program which allows for quick texture generation using procedural nodes that can be connected in a graph, similar to a program like Substance Designer. It is not finished currently, but the core of the app is working and I have found it very useful for creating my games.

## Cloning the repository
To clone the repository, please only use GitHub Desktop,  
or with the Git CLI run `git clone --recursive https://github.com/oxi-dev0/Texture-Graph.git`.  
> The `--recursive` option is very important, as the repository uses the submodules system for it's [Dependencies](https://github.com/oxi-dev0/Texture-Graph/wiki/Dependencies).

<br>

## Usage and Contribution documentation can be found [here](https://github.com/oxi-dev0/Texture-Graph/wiki).

<br>

## Task List for v1.0
| Issue / Improvement | Complete |
| --- | --- |
| Some nodes broke after Core Functions were introduced |x|
| Deleting nodes will sometimes cause relation issues, with evaluating and serialization | |
| Mouse switching views while inputs are being processed will sometimes cause both views to handle inputs | |
| Graph editor view background is buggy | |
| Evaluation is inefficient, and slow | |
| GUI Overhaul | |
| Needs 3D Viewer and maybe material system | |
| TGNL is badly formatted and unintuitive |x|
| New TGNL condition parser needs validation checking | |
