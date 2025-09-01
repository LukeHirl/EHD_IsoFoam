[README.md](https://github.com/user-attachments/files/22072072/README.md)
# EHD_IsoFoam User Guide

EHD_IsoFoam is an electrohydrodynamic solver aimed at simulating electrosprays. The solver itself is found in EHD_IsoFoam8, while example cases can be found in the respective folder. 
It is based on openfoam2212, while it might be able to compile on neweer versions, this is not yet testet
---

## Installation Guide for EHD_IsoFoam in Openfoam

EHD_IsoFoam can only be compiled in a Linux environment. Please use either a dedicated Linux installation or WSL2 [Windows Subsystem for Linux (WSL2)](https://learn.microsoft.com/en-us/windows/wsl/install) 
Once Openfoam is installed in your system (plesae follow the instructions providded here: [OpenFOAM website](https://develop.openfoam.com/Development/openfoam/-/wikis/precompiled/)), and you have downloaded the solver, enter the solver folder structure and start the openfoam environment.

```bash
cd /path/to/EHD_IsoFoam
openfoam2212
```

In the folder `Make/` you will find a file called files containing the lines

EHD_IsoFoam8.C

EXE = $(FOAM_USER_APPBIN)/EHD_IsoFoam8

The folder name conatining the folder, the main `.C` file name and and the names of the solver in this file must be the same. For `EXE = $(FOAM_USER_APPBIN)/EHD_IsoFoam8` you can change `$(FOAM_USER_APPBIN)` to `$(PWD)/output`, which writes the compiled solver to the `output` directory in the solver folder.
Using the first option you run the solver with `EHD_IsoFoam8`, otherwise use `/path/to/EHD_IsoFoam/output/EHD_IsoFoam8`.

Compilation is done with
```bash
wclean
wmake
```

If any compilations error occur please open an issue or try to fix it yourself, most of the time errors are fairly well explained.

## running a case

Download one or all of the Example cases and navigate to one of them.

```bash
cd /path/to/ExampleCase
```

In it you find three folders and a .foam file.

The `0` folder contains initial field distributions and boundary conditions for every solution variable.
The `system` folder contains general settings for the simulation, as well as setup scripts.
The `constant` folder contains constants for the simulation like gravity,and fluid properties, as well as the generated mesh.

### 0/ folder

A solution variable is always stored in a field file. Lets look at the the volume fraction file for a fluid `alpha.water` as an example

```
dimensions      [0 0 0 0 0 0 0];            // set the physical dimensions of the field

internalField   uniform 0;                  // internal field distribution; right now the second fluid occupies the entire domain.

boundaryField
{
    upWall
    {
        type            zeroGradient;
    }
    downWall
    {
        type            zeroGradient;
    }
    leftandright
    {
        type            zeroGradient;
    }
    Front
    {
        type            empty;
    }
    Back
    {
        type            empty;
    }
}
```
Boundary conditions need to be changed within this file by replacing the entry behind type. Please compare with other files for options.
initial field distributions can be set with on of three commands

```bash
setFields              //used to set a constant value in simple geometries, as defined in the setFieldsDict
setExprFields          // used to set initial distributions according to a funtion, as defined in setExprFieldsDict
setAlphaFields         // mainly used for volume fraction fields in order to also be able to set a cell only partially ocuppied by a geometry to a partial of the specified value(enhanced setFields)
```

These are found in the `system folder`

### system/ folder

In the system folde ryou will also find the controlDict, for most setting please refer to [OpenFOAM website](https://www.openfoam.com/documentation/user-guide/6-solving/6.1-time-and-data-inputoutput-control)
The last lines are additional settings for EHD_IsoFoam8

```
Electrodynamic				true;              //switch to toggle the solving of the implemented electrodynamic equaitons

Hydrodynamic				  false;             //switch to toggle the solving of the Navier-Stokes equation

ChargeCondunction			true;              //switch to toggle the solving of the charge conservation equation

ReferenceApproach			true;              //switch to toggle the the use of the reference approach or the FVD approach

NewStart					    true;              //if newStart is true the charge conservation equation will not be solved in the first time step, as the solve assumes that no electric field distribution has yet been calculated

interfaceCellTol 			0.01;

// provide the elctrical properties for both fluids(phase1 always refers to the alpha field equal toone in the alpha file
phase1
{
    sgm             1; 
    epsR            2;
}

phase2
{
    sgm             2;
    epsR            1;
}
```

`blockMeshDict` defines the mesh
`snappyHexMeshDict` allows for local refinement of the mesh at the start of the simulation
`decomposeParDict`  allows for the decompostition of the simulated domain into multiple parts, enabling parallel computation

## 📂 Workflow Overview

In order to run the provided example simulations please follow the commands provided in the run.txt file in each case.

Visualization can be done with the help of Paraview. For that please install Paraview [Paraview](https://www.paraview.org/download/) and use the `.foam` file to open the case in Paraview

For further information please refer to the OpenFOAM user guide [OpenFoamGuide](https://www.openfoam.com/documentation/user-guide)

