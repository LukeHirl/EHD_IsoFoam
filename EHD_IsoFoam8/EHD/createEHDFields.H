/* 	1	Mass	kilogram (kg)	pound-mass (lbm)
	2	Length	metre (m)	foot (ft)
	3	Time	second (s)	second (s)
	4	Temperature	Kelvin (K)	degree Rankine (∘ \relax \special {t4ht=R)
	5	Quantity	mole (mol)	mole (mol)
	6	Current	ampere (A)	ampere (A)
	7	Luminous intensity	candela (cd)	candela (cd) */


	const dimensionedScalar sgm1("sgm1", dimensionSet(-1,-3,3,0,0,2,0), readScalar(runTime.controlDict().subDict("phase1").lookup("sgm")));
	const dimensionedScalar sgm2("sgm2", dimensionSet(-1,-3,3,0,0,2,0), readScalar(runTime.controlDict().subDict("phase2").lookup("sgm")));
	
	const dimensionedScalar epsR1_("epsR1", dimless, readScalar(runTime.controlDict().subDict("phase1").lookup("epsR")));
	const dimensionedScalar epsR2_("epsR2", dimless, readScalar(runTime.controlDict().subDict("phase2").lookup("epsR")));
	
	const dimensionedScalar eps0("eps0", dimensionSet(-1,-3,4,0,0,2,0), scalar(8.854188e-12));//

	Info<<"Found: epsR1=" << epsR1_ << ",epsR2=" << epsR2_ << ",sgm1=" << sgm1 << ",sgm2=" << sgm2 << endl;

	dimensionedScalar eps1 = eps0 * epsR1_;//eps0 * 
	dimensionedScalar eps2 = eps0 * epsR2_;
	
	
	Switch Electrodynamic = runTime.controlDict().lookupOrDefault<Switch>("Electrodynamic", true);
	Switch ChargeCondunction = runTime.controlDict().lookupOrDefault<Switch>("ChargeCondunction", true);
	Switch Hydrodynamic = runTime.controlDict().lookupOrDefault<Switch>("Hydrodynamic", true);
	Switch reference = runTime.controlDict().lookupOrDefault<Switch>("ReferenceApproach", true);
	Switch fromScratch = runTime.controlDict().lookupOrDefault<Switch>("NewStart", true);
	Switch singlePhase = true;
	if((sgm1*sgm2).value() != 0)
	{
		singlePhase = false;
		Info << "Two conductive liquids" << endl;
	}
	
	
	
	
	//property fields
	
	volScalarField sgm
	(
		IOobject
		(
			"sgm",
			runTime.timeName(),
			mesh,
			IOobject::READ_IF_PRESENT,
			IOobject::AUTO_WRITE
		),
		alpha1 * sgm1 + (1 - alpha1) * sgm2,
		zeroGrad.boundaryField().types()
	);
	sgm.oldTime();

	surfaceScalarField sgmSurf
	(
		IOobject
		(
			"sgmSurf",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		mesh,
		sgm1
	);

	volScalarField eps
	(
		IOobject
		(
			"eps",
			runTime.timeName(),
			mesh,
			IOobject::READ_IF_PRESENT,
			IOobject::AUTO_WRITE
		),
		alpha1 * eps1 + (1 - alpha1) * eps2,
		zeroGrad.boundaryField().types()
	);
	eps.oldTime();
	
	surfaceScalarField epsSurf
	(
		IOobject
		(
			"epsSurf",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		mesh,
		eps1
	);
	
	
	//variable Fields
	
	Info<< "Reading field Electric Potential Ue\n" << endl;
	volScalarField Ue_EM
	(
		IOobject
		(
			"Ue_EM",
			runTime.timeName(),
			mesh,
			IOobject::MUST_READ,
			IOobject::AUTO_WRITE
		),
		mesh
	);

	Info<< "Reading field electric density rhoE\n" << endl;
	volScalarField rhoE_EM
	(
		IOobject
		(
			"rhoE_EM",
			runTime.timeName(),
			mesh,
			IOobject::MUST_READ,
			IOobject::AUTO_WRITE
		),
		mesh
	);

	volVectorField E_EM
	(
		IOobject
		(
			"E_EM",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		-fvc::grad(Ue_EM)
	);
	
	surfaceScalarField E_EM_Surf
	(
		IOobject
		(
			"E_EM_Surf",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		fvc::snGrad(Ue_EM)
	);
	
	surfaceScalarField phi_EM
	(
		IOobject
		(
			"phi_EM",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		phi
	);
	
	
	//fields related to charge Conduction
	
	surfaceScalarField Conductive				//Field where conductive Boundaries are defined as 1; internal is always set to one, rest is 0, then multiplied with calculated conductivity
	(
		IOobject
		(
			"Conductive",
			runTime.timeName(),
			mesh,
			IOobject::MUST_READ,
			IOobject::AUTO_WRITE
		),
		mesh
	);
	
	surfaceScalarField Current				//Field where conductive Boundaries are defined as 1 internal is also one rest is 0, then multiplied with calculated conductivity
	(
		IOobject
		(
			"Current",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		mesh,
		dimensionedScalar("current", dimensionSet(0,0,0,0,0,1,0), 0.0)
	);

	volScalarField emittedCurrent
	(
		IOobject
		(
			"emittedCurrent",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		mesh,
		dimensionedScalar("emittedCurrent", dimensionSet(0,-3,0,0,0,1,0), 0.0)
	);
	
	
	//force related fields
	
	volTensorField MST
	(
		IOobject
		(
			"MST",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		eps*(E_EM*E_EM - 0.5*magSqr(E_EM)*I)
	);
	
	volVectorField Fe1
	(
		IOobject
		(
			"Fe1",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::AUTO_WRITE
		),
		fvc::div(MST)
	);
	
/* 	volVectorField Fe2
	(
		IOobject
		(
			"Fe2",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		fvc::div(MST)
	);
	
	volVectorField Fe3
	(
		IOobject
		(
			"Fe3",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		fvc::div(MST)
	);
	
	volVectorField Fe4
	(
		IOobject
		(
			"Fe4",
			runTime.timeName(),
			mesh,
			IOobject::NO_READ,
			IOobject::NO_WRITE
		),
		fvc::div(MST)
	); */

	
