    IOdictionary ABLProperties
    (
        IOobject
        (
            "ABLProperties",
            runTime.time().constant(),
            runTime,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    // PROPERTIES CONCERNING THE SOURCE TERMS.

       // Specify the type of source to use for momentum and temperature.  The
       // possible types are "given" and "computed".
       // - The "given" type means that the source values are directly given
       //   and the momentum and temperature fields will react accordingly.
       // - The "computed" type means that the mean velocity and temperature
       //   are given and the source terms that maintain them are computed.
       word momentumSourceType(ABLProperties.lookup("momentumSourceType"));
       word temperatureSourceType(ABLProperties.lookup("temperatureSourceType"));

       // If giving the velocity and computing the sources, specify how the velocity
       // is given.  "component" means you enter the x, y, anc z components.
       // "speedAndDirection" means that you enter the horizontal wind speed, horizontal
       // direction, and vertical component.
       word velocityInputType(ABLProperties.lookup("velocityInputType"));

       // Read in the heights at which the sources are given.
       List<scalar> sourceHeightsMomentumSpecified(ABLProperties.lookup("sourceHeightsMomentum"));
       List<scalar> sourceHeightsTemperatureSpecified(ABLProperties.lookup("sourceHeightsTemperature"));

       // Read in the table of momentum source vs. time and height.
       List<List<scalar> > sourceTableMomentumX(ABLProperties.lookup("sourceTableMomentumX"));
       List<List<scalar> > sourceTableMomentumY(ABLProperties.lookup("sourceTableMomentumY"));
       List<List<scalar> > sourceTableMomentumZ(ABLProperties.lookup("sourceTableMomentumZ"));

       // Read in the table of temperature source vs. time and height.
       List<List<scalar> > sourceTableTemperature(ABLProperties.lookup("sourceTableTemperature"));


       // Check sizes of source tables.
       label nSourceMomentumHeights = sourceHeightsMomentumSpecified.size();
       label nSourceTemperatureHeights = sourceHeightsTemperatureSpecified.size();
       label nSourceMomentumXTimes = sourceTableMomentumX.size();
       label nSourceMomentumYTimes = sourceTableMomentumY.size();
       label nSourceMomentumZTimes = sourceTableMomentumZ.size();
       label nSourceTemperatureTimes = sourceTableTemperature.size();
       forAll(sourceTableMomentumX,i)
       {
           if (sourceTableMomentumX[i].size()-1 != nSourceMomentumHeights)
           {
               FatalErrorIn
               (
                   "Number of sourceTableMomentumX heights does not match sourceHeightsMomentum"
               )   << abort(FatalError);
           }
       }
       forAll(sourceTableMomentumY,i)
       {
           if (sourceTableMomentumY[i].size()-1 != nSourceMomentumHeights)
           {
               FatalErrorIn
               (
                   "Number of sourceTableMomentumY heights does not match sourceHeightsMomentum"
               )   << abort(FatalError);
           }
       }
       forAll(sourceTableMomentumZ,i)
       {
           if (sourceTableMomentumZ[i].size()-1 != nSourceMomentumHeights)
           {
               FatalErrorIn
               (
                   "Number of sourceTableMomentumZ heights does not match sourceHeightsMomentum"
               )   << abort(FatalError);
           }
       }
       forAll(sourceTableTemperature,i)
       {
           if (sourceTableTemperature[i].size()-1 != nSourceTemperatureHeights)
           {
               FatalErrorIn
               (
                   "Number of sourceTableTemperature heights does not match sourceHeightsTemperature"
               )   << abort(FatalError);
           }
       }


       // Break the source tables into interpolation tables.
       List<scalar> sourceMomentumXTimesSpecified(nSourceMomentumXTimes,0.0);
       List<scalar> sourceMomentumYTimesSpecified(nSourceMomentumYTimes,0.0);
       List<scalar> sourceMomentumZTimesSpecified(nSourceMomentumZTimes,0.0);
       List<scalar> sourceTemperatureTimesSpecified(nSourceTemperatureTimes,0.0);

       List<List<scalar> > sourceMomentumXSpecified(nSourceMomentumXTimes,List<scalar>(nSourceMomentumHeights,0.0));
       List<List<scalar> > sourceMomentumYSpecified(nSourceMomentumYTimes,List<scalar>(nSourceMomentumHeights,0.0));
       List<List<scalar> > sourceMomentumZSpecified(nSourceMomentumZTimes,List<scalar>(nSourceMomentumHeights,0.0));
       List<List<scalar> > sourceTemperatureSpecified(nSourceTemperatureTimes,List<scalar>(nSourceTemperatureHeights,0.0));

       for(int i = 0; i < nSourceMomentumXTimes; i++)
       {
           sourceMomentumXTimesSpecified[i] = sourceTableMomentumX[i][0];
           for(int j = 0; j < nSourceMomentumHeights; j++)
           {
               sourceMomentumXSpecified[i][j] = sourceTableMomentumX[i][j+1];
           }
       }

       for(int i = 0; i < nSourceMomentumYTimes; i++)
       {
           sourceMomentumYTimesSpecified[i] = sourceTableMomentumY[i][0];
           for(int j = 0; j < nSourceMomentumHeights; j++)
           {
               sourceMomentumYSpecified[i][j] = sourceTableMomentumY[i][j+1];
           }
       }

       for(int i = 0; i < nSourceMomentumZTimes; i++)
       {
           sourceMomentumZTimesSpecified[i] = sourceTableMomentumZ[i][0];
           for(int j = 0; j < nSourceMomentumHeights; j++)
           {
               sourceMomentumZSpecified[i][j] = sourceTableMomentumZ[i][j+1];
           }
       }

       for(int i = 0; i < nSourceTemperatureTimes; i++)
       {
           sourceTemperatureTimesSpecified[i] = sourceTableTemperature[i][0];
           for(int j = 0; j < nSourceTemperatureHeights; j++)
           {
               sourceTemperatureSpecified[i][j] = sourceTableTemperature[i][j+1];
           }
       }


       // If the desired mean wind or temperature is given at only one height, then revert to
       // the old way of specifying the source term.  Find the two grid levels that bracket
       // the given level and interpolate in between them to compute the source term.  So,
       // here, find those two levels.
       label hLevelsWind1I = 0;
       label hLevelsWind2I = 0;
       scalar hLevelsWind1 = 0;
       scalar hLevelsWind2 = 0;
       if ((momentumSourceType == "computed") && (nSourceMomentumHeights == 1))
       {
           #include "findWindHeight.H"
       }

       label hLevelsTemp1I = 0;
       label hLevelsTemp2I = 0;
       scalar hLevelsTemp1 = 0;
       scalar hLevelsTemp2 = 0;
       if ((temperatureSourceType == "computed") && (nSourceTemperatureHeights == 1))
       {
           #include "findTemperatureHeight.H"
       }


       // Relaxation factor on the source term application.
       scalar alphaMomentum(ABLProperties.lookupOrDefault<scalar>("alphaMomentum",1.0));
       scalar alphaTemperature(ABLProperties.lookupOrDefault<scalar>("alphaTemperature",1.0));




    // PROPERTIES CONCERNING CORIOLIS FORCES

       // Planetary rotation period (hours)
       scalar planetaryRotationPeriod(readScalar(ABLProperties.lookup("planetaryRotationPeriod")));

       // Latitude on the planetary body (degrees)
       scalar latitude(readScalar(ABLProperties.lookup("latitude")));

       // Compute the planetar rotation vector
       vector Omega_;
       Omega_.x() = 0.0;
       Omega_.y() = ((2.0 * Foam::constant::mathematical::pi) / (max(1.0E-5,planetaryRotationPeriod)*3600.0)) * Foam::cos(latitude*Foam::constant::mathematical::pi/180.0);
       Omega_.z() = ((2.0 * Foam::constant::mathematical::pi) / (max(1.0E-5,planetaryRotationPeriod)*3600.0)) * Foam::sin(latitude*Foam::constant::mathematical::pi/180.0);
       uniformDimensionedVectorField Omega
       (
           IOobject
           (
               "Omega",
               runTime.constant(),
               mesh,
               IOobject::NO_READ,
               IOobject::NO_WRITE
           ),
           dimensionedVector("Omega",dimensionSet(0, 0, -1, 0, 0, 0, 0),Omega_)
       );

       Info<<"Omega=" << Omega << endl;




    // PROPERTIES CONCERNING GATHERING STATISTICS

       // Gather/write statistics?
       bool statisticsOn(ABLProperties.lookupOrDefault<bool>("statisticsOn", false));

       // Statistics gathering/writing frequency?
       int statisticsFreq(int(readScalar(ABLProperties.lookup("statisticsFrequency"))));
