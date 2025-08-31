	
	
	//interface center and normal from isoAdvector
	const volVectorField& interfaceInFluid = advector.surf().centre();
	const volVectorField& interfaceNormalInFluid = advector.surf().normal();
	
	for (label cellI = 0; cellI < mesh.nCells(); ++cellI)
	{
		
		interface[cellI] = (mag(interfaceInFluid[cellI]) != 0 ); // determining interface cells
		interfaceNormal[cellI] = interfaceNormalInFluid[cellI];
		scalar magni = mag(interfaceNormalInFluid[cellI]);
		if (magni != 0)
		{	
			interfaceNormal[cellI] = interfaceNormalInFluid[cellI] / magni; //get normalized interface vector pointing away from surface
		}
	}
	
	
	for (label celli = 0; celli < mesh.nCells(); ++celli)
	{				
		alphaCond[celli] = scalar(1) * ((alpha1[celli] > 0.5) || (interface[celli] == 1));	//every cell that is occupied by alpha1 = 1
		alphaCond2[celli] = alphaCond[celli] - interface[celli];							//every cell that is occupied by alpha1 = 1, except the interface
	}
	alphaCond.correctBoundaryConditions();
	alphaCond2.correctBoundaryConditions();
	
	
	//surf-conductivity preparations			
	alphaSurf1 = fvc::interpolate(alphaCond2);		// surfaceField, which will be 1 on faces in the first phase, else 0
	alphaInterSurf = fvc::interpolate(interface); 	// surfcaeField, which will be 1 on faces between interface cells, else 0
	
	
	for (label facei = 0; facei < mesh.nInternalFaces(); ++facei)
	{
		alphaSurf1[facei] = (alphaSurf1[facei] > scalar(0));
		alphaInterSurf[facei] = (alphaInterSurf[facei] == scalar(1));
	}
	const fvPatchList& patches = mesh.boundary();
	forAll(patches, patchi)
	{
		const fvPatch& currPatch = patches[patchi];
		if (currPatch.type() == "processor")	
		{
			forAll(currPatch, facei)
			{
				label cellI_own = mesh.boundary()[patchi].faceCells()[facei];
				alphaSurf1.boundaryFieldRef()[patchi][facei] = alphaSurf1.boundaryFieldRef()[patchi][facei] * ( (alphaCond2.boundaryFieldRef()[patchi].patchNeighbourField()()[facei] + alphaCond2.boundaryFieldRef()[patchi].patchInternalField()()[facei]) > 0);
				alphaInterSurf.boundaryFieldRef()[patchi][facei] = alphaInterSurf.boundaryFieldRef()[patchi][facei] * ( (interface.boundaryFieldRef()[patchi].patchNeighbourField()()[facei] + interface.boundaryFieldRef()[patchi].patchInternalField()()[facei]) == 2);
			}
		}
		else
		{
			forAll(currPatch, facei)
			{
				label cellI_own = mesh.boundary()[patchi].faceCells()[facei];
				alphaSurf1.boundaryFieldRef()[patchi][facei] = ( alphaCond2.boundaryFieldRef()[patchi].patchInternalField()()[facei] > 0);
				alphaInterSurf.boundaryFieldRef()[patchi][facei] = (interface.boundaryFieldRef()[patchi].patchInternalField()()[facei] > 0);
			}
		}
	}	
	
	alphaSurf2 = 1 - alphaSurf1 - alphaInterSurf;	// surfaceField, which will be 1 on faces in the second phase, else 0
