/* 	1	Mass	kilogram (kg)	pound-mass (lbm)
	2	Length	metre (m)	foot (ft)
	3	Time	second (s)	second (s)
	4	Temperature	Kelvin (K)	degree Rankine (∘ \relax \special {t4ht=R)
	5	Quantity	mole (mol)	mole (mol)
	6	Current	ampere (A)	ampere (A)
	7	Luminous intensity	candela (cd)	candela (cd) */
	
	const dimensionedScalar surfCellTol = alpha1.mesh().solverDict(alpha1.name()).getOrDefault<scalar>("surfCellTol", 1e-8);
	//const dimensionedScalar surfCellTol = runTime.controlDict().lookupOrDefault<scalar>("surfCellTol", 1e-6);
	
	volScalarField V
    (
        IOobject
        (
            mesh.V().name(),
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar(mesh.V().dimensions(), Zero),
        calculatedFvPatchField<scalar>::typeName
    );
	V.ref() = mesh.V();

	volVectorField C
    (
        IOobject
        (
            mesh.C().name(),
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedVector(mesh.C().dimensions(), Zero),
        calculatedFvPatchField<scalar>::typeName
    );
	C.ref() = mesh.C();
	
	volScalarField zeroGrad // empty field just serves for providing zeroGradient condition for alphaCond, sgm and eps
	(
		IOobject
		(
			"zeroGrad",
			runTime.timeName(),
			mesh,
			IOobject::MUST_READ,
			IOobject::AUTO_WRITE
		),
		mesh
	);
	
	volScalarField alphaCond
	(
		IOobject
		(
			"alphaCond",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		alpha1,
		zeroGrad.boundaryField().types()
	);
	
	volScalarField alphaCond2
	(
		IOobject
		(
			"alphaCond2",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		alpha1,
		zeroGrad.boundaryField().types()
	);
	
	surfaceScalarField alphaSurf1
	(
		IOobject
		(
			"alphaSurf1",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		fvc::interpolate(alphaCond)
	);
	
	surfaceScalarField alphaSurf2
	(
		IOobject
		(
			"alphaSurf2",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		1-alphaSurf1
	);
	
	surfaceScalarField alphaInterSurf
	(
		IOobject
		(
			"alphaInterSurf",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		alphaSurf1
	);
		
	surfaceScalarField phiTemp
	(
		IOobject
		(
			"phiTemp",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		phi
	);
	
/* 	volScalarField FeFilter
	(
		IOobject
		(
			"FeFilter",
			runTime.timeName(),
			mesh,
			IOobject::MUST_READ,
			IOobject::AUTO_WRITE
		),
		mesh,
		dimensionedScalar("interface", dimensionSet(0,0,0,0,0,0,0), 0.0),
		zeroGrad.boundaryField().types()
	); */
	
	volScalarField interface
	(
		IOobject
		(
			"interface",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		mesh,
		dimensionedScalar("interface", dimensionSet(0,0,0,0,0,0,0), 0.0),
		zeroGrad.boundaryField().types()
	);
	
	volVectorField interfaceNormal
	(
		IOobject
		(
			"interfaceNormal",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		mesh,
		dimensionedVector("interfaceNormal", dimensionSet(0,0,0,0,0,0,0), vector::zero)
	);
