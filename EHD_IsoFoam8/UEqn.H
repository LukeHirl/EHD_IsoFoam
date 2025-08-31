	
	volScalarField E_MagSqr = magSqr(E_EM);
	Fe1 = rhoE_EM * E_EM - 0.5 * E_MagSqr * fvc::grad(eps);
	
    MRF.correctBoundaryVelocity(U);
	
    fvVectorMatrix UEqn
    (
        fvm::ddt(rho, U) + fvm::div(rhoPhi, U)
      + MRF.DDt(rho, U)
      + turbulence->divDevRhoReff(rho, U)	  
	  - Fe1
     ==
        fvOptions(rho, U)
    );

    #include "UEqnAddPorosity.H"

    UEqn.relax();

    fvOptions.constrain(UEqn);
	
    if (pimple.momentumPredictor())
    {
        solve
        (
            UEqn
         ==
            fvc::reconstruct
            (
                (
                    mixture.surfaceTensionForce()
                  - ghf*fvc::snGrad(rho)
                  - fvc::snGrad(p_rgh)
                ) * mesh.magSf()
            )
        );

        fvOptions.correct(U);
    }

