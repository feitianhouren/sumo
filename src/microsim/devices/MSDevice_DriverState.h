/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_DriverState.h
/// @author  Leonhard Luecken
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    15.06.2018
/// @version $Id$
///
/// The Driver State Device mainly provides a configuration and interaction interface for the vehicle's driver state.
/// @see microsim/MSDriverState.h
///
/****************************************************************************/
#ifndef MSDevice_DriverState_h
#define MSDevice_DriverState_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSVehicleDevice.h"
#include <utils/common/SUMOTime.h>
#include <utils/common/WrappingCommand.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;
class MSVehicle;
class MSSimpleDriverState;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_DriverState
 *
 * @brief The ToC Device controls transition of control between automated and manual driving.
 * @todo: Provide logging facilities
 * @todo: allow manual and automated type to refer to vTypeDistributions
 *
 * @see MSDevice
 */
class MSDevice_DriverState : public MSVehicleDevice {
public:
    /** @brief Inserts MSDevice_DriverState-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a ToC-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into);

    /// update internal state
    void update();

    /// return internal state
    inline std::shared_ptr<MSSimpleDriverState> getDriverState() const {
        return myDriverState;
    }

private:
    /// @name Helpers for parameter parsing
    /// @{
    static double getMinAwareness(const SUMOVehicle& v, const OptionsCont& oc);
    static double getInitialAwareness(const SUMOVehicle& v, const OptionsCont& oc);
    static double getErrorTimeScaleCoefficient(const SUMOVehicle& v, const OptionsCont& oc);
    static double getErrorNoiseIntensityCoefficient(const SUMOVehicle& v, const OptionsCont& oc);
    static double getSpeedDifferenceErrorCoefficient(const SUMOVehicle& v, const OptionsCont& oc);
    static double getSpeedDifferenceChangePerceptionThreshold(const SUMOVehicle& v, const OptionsCont& oc);
    static double getHeadwayChangePerceptionThreshold(const SUMOVehicle& v, const OptionsCont& oc);
    static double getHeadwayErrorCoefficient(const SUMOVehicle& v, const OptionsCont& oc);
    /// @}


public:
    /// @brief Destructor.
    ~MSDevice_DriverState() {};

    /// @brief return the name for this type of device
    const std::string deviceName() const {
        return "driverstate";
    }

    /// @brief try to retrieve the given parameter from this device. Throw exception for unsupported key
    std::string getParameter(const std::string& key) const;

    /// @brief try to set the given parameter for this device. Throw exception for unsupported key
    void setParameter(const std::string& key, const std::string& value);


private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_DriverState(SUMOVehicle& holder, const std::string& id,
                         double minAwareness,
                         double initialAwareness,
                         double errorTimeScaleCoefficient,
                         double errorNoiseIntensityCoefficient,
                         double speedDifferenceErrorCoefficient,
                         double speedDifferenceChangePerceptionThreshold,
                         double headwayChangePerceptionThreshold,
                         double headwayErrorCoefficient);

    /// @brief Initializeses the driver state parameters
    void initDriverState();

private:
    /// @brief The holder vehicle casted to MSVehicle*
    MSVehicle* myHolderMS;

    double myMinAwareness;
    double myInitialAwareness;
    double myErrorTimeScaleCoefficient;
    double myErrorNoiseIntensityCoefficient;
    double mySpeedDifferenceErrorCoefficient;
    double mySpeedDifferenceChangePerceptionThreshold;
    double myHeadwayChangePerceptionThreshold;
    double myHeadwayErrorCoefficient;

    /// @brief The driver state of the holder.
    std::shared_ptr<MSSimpleDriverState> myDriverState;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_DriverState(const MSDevice_DriverState&);

    /// @brief Invalidated assignment operator.
    MSDevice_DriverState& operator=(const MSDevice_DriverState&);

};


#endif

/****************************************************************************/

