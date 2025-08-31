		
			//update electric properties
			eps== alpha1 * eps1 + (1 - alpha1) * eps2;
			eps.correctBoundaryConditions();			
			sgm == alpha1 * sgm1 + (1 - alpha1) * sgm2;
			sgm.correctBoundaryConditions();
			
			alphaSurf1 = alphaSurf1 + alphaInterSurf * fvc::interpolate(alpha1);
			alphaSurf2 = alphaSurf2 + (alphaInterSurf - alphaInterSurf * fvc::interpolate(alpha1));
			
			sgmSurf = (alphaSurf1 * sgm1 + alphaSurf2 * sgm2) * Conductive;
			epsSurf = (alphaSurf1 * eps1 + alphaSurf2 * eps2);
						
			if(singlePhase)
			{
				phi_EM = phi * (alphaSurf1 + alphaInterSurf);
			}
			
			if(!Hydrodynamic)
			{
				phi_EM = phi_EM * 0;
			}
			
			
			if(reference)
			{
				if(ChargeCondunction && !fromScratch)
				{
					surfaceScalarField rhoEFlux = -fvc::interpolate(sgm)*mesh.magSf()*fvc::snGrad(Ue_EM); // -fvc::interpolate(sgm)*mesh.magSf()*fvc::snGrad(Ue_EM); //
					Current = rhoEFlux;
					fvScalarMatrix rhoE_EMEqn
					(
						fvm::ddt(rhoE_EM)
					  + fvm::div(phi_EM, rhoE_EM)
					  + fvc::div(rhoEFlux)
					);
					rhoE_EMEqn.solve();
					rhoE_EM.correctBoundaryConditions();
				}
				
				fvScalarMatrix Ue_EMEqn
				(
					fvm::laplacian(eps,Ue_EM) == -rhoE_EM
				);
				Ue_EMEqn.solve();
				Ue_EM.correctBoundaryConditions();
			}
			else
			{
				if(ChargeCondunction && !fromScratch)
				{
					surfaceScalarField rhoEFlux = -sgmSurf*mesh.magSf()*fvc::snGrad(Ue_EM); // -fvc::interpolate(sgm)*mesh.magSf()*fvc::snGrad(Ue_EM); //
					Current = rhoEFlux;
					fvScalarMatrix rhoE_EMEqn
					(
						fvm::ddt(rhoE_EM)
					  + fvm::div(phi_EM, rhoE_EM)
					  + fvc::div(rhoEFlux)
					);
					rhoE_EMEqn.solve();
					rhoE_EM.correctBoundaryConditions();
				}
				
				fvScalarMatrix Ue_EMEqn
				(
					fvm::laplacian(epsSurf,Ue_EM) == -rhoE_EM
				);
				Ue_EMEqn.solve();
				Ue_EM.correctBoundaryConditions();
			}
			
			
			E_EM = -fvc::grad(Ue_EM);
			E_EM.correctBoundaryConditions();						
			
			if(fromScratch)
			{
				fromScratch = false;
			}
			
			

