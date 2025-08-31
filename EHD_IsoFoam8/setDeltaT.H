/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Global
    setDeltaT

Description
    Reset the timestep to maintain a constant maximum courant Number.
    Reduction of time-step is immediate, but increase is damped to avoid
    unstable oscillations.

\*---------------------------------------------------------------------------*/

if (adjustTimeStep)
{
    scalar maxDeltaTFact =
        min(maxCo/(CoNum + SMALL), maxAlphaCo/(alphaCoNum + SMALL));

    scalar deltaTFact = min(min(maxDeltaTFact, 1.0 + 0.1*maxDeltaTFact), 1.2);
	scalar deltaTElectricMax = 10;
	scalar deltaTElectricMax1 = 10;
	scalar deltaTElectricMax2 = 10;
	
	if(sgm1.value() > 0)
	{	
		deltaTElectricMax1 = 0.9*(eps1.value()/sgm1.value());
	}
	if(sgm2.value() > 0)
	{	
		deltaTElectricMax2 = 0.9*(eps2.value()/sgm2.value());
	}

    scalar deltaTElectric = min(deltaTElectricMax1,deltaTElectricMax2);
	deltaTElectric = min(deltaTElectric,deltaTElectricMax);	
	
	if(!Electrodynamic)
	{
		deltaTElectric = 10;
	}
	
	Info<< "deltaTElectric: " << deltaTElectric << endl;
    runTime.setDeltaT
    (
        min
        (
            deltaTFact*runTime.deltaTValue(),
            min
			(
				maxDeltaT,
				deltaTElectric
			)
		)
    );

    Info<< "deltaT = " <<  runTime.deltaTValue() << endl;
}

// ************************************************************************* //
