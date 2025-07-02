# Unreal SDK

## Introduction

Welcome to the Weart Unreal SDK documentation.

<b>Support Unreal: 5.1, 5.2, 5.3, 5.4</b>

\warning <b>The Unreal 4.27 is no longer supported.</b> 

The SDK allows to connect to the weart app and perform various actions with the TouchDIVER Pro devices:

* Start and Stop device execution
* Calibrate the finger tracking device
* Receive tracking data from the devices
* Retrieve raw data from the device
* Send haptic effects to the devices (actuations)
* Read status information from the device
* Virtual hand system for object interaction
  * Physics-based interaction, ensuring hands do not penetrate objects with colliders
  * Grasping system managed through pressure points, leveraging physics-based constraints
  * Interaction with constrained objects, utilizing the physics engine for realistic behavior
  * Gesture system for creating new interactions
  * Runtime offset adjustment, allowing adaptation to different trackers
  * UI interaction support
  * Surface exploration to maximize tactile feedback

The minimum setup to use the weart SDK consists of:

* A TouchDIVER device
* An Unreal project using the SDK plugin 
* WEART App running

### TouchDIVER Pro

![](./TDPro.png)

### TouchDIVER

![](./TDmain.png)

### Plugin Configuration

Go to Project Settings, then under the Plugins category you will find the WeArt settings. Here you can edit if you want to start the weart app and the calibration at the beginning of the experience.

![](./pluginSettings.png)

Here you can choose device generation for TouchDIVER Pro or TouchDIVER

![](./DeviceGeneration.png)

### Importing WEART SDK

* Run the WEART Middleware (for TouchDIVER), run the WEART APP (for TouchDIVER Pro)
* Connect at least 1 TouchDIVER
* Run Unreal experience

<b>Warning for Unreal 5.4</b> You need to paste path to AppLocal of your engine in prerequisites

![](./Prereq.png)

Create a new project by selecting Virtual Reality.

![](./projectCreation.bmp)

Copy the whole plugin folder either in $UE_LOCATION$/Engine/Plugins for global use
or in $PROJECT_FOLDER$/Plugins to use in a specific project. Enable it inside the
UE editor in Edit/Plugins.

![](./pluginsFolder.png)

Enable the plugin if it was placed globally

![](./weArtPlugin.png)


### Setup Example

For an example scene ready to use open "WEARTSampleLevel". To find It go to DemoResources folder.

![](./levelFile.png)

Make sure you have WeArtGameMode blueprint set in WorldSettings tab, after that level is ready to test SDK functionality. If you would like to start from using your own controller classes, you can change them
in GameMode blueprint, or in controller class itself.

![](./gameModeInDemo.png)

To use controller with test hand system make sure you have you WeArtGameMode set in world settings.

![](./gameModeWorldSettings.png)

This GameMode contains controller and pawn with ready to test hands.

![](./gameModeContent.png)

#### Haptic Surfaces

On the left side from player start, there are 3 plates with different touchable surface settings, you can test haptic effects with them.

![](./alumPlate.png)

#### Interaction

To test temperature you can use bunsens on the table, red one produces hot particles, blue one - cold. To enable them, press the button, It can be clicked by hand or grabbed object.

![](./bunsens.png)

On table placed several objects, on which you can test grab system.

![](./grabableObjects.png)

#### Physics Materials

You can use physics materials to achieve different behavior for different surfaces, like friction or bounciness. By default, demo
level uses materials for different planes, you can find them in ```DemoResources/PhysicsMaterials```.

![](./physicsMats.png)

To apply them, choose static mesh component of an object and apply your physics material on It.

![](./physMatExample.png)

#### Making an object graspable

Grabbing system works on actors, that have WeArtTouchable component attached and proper collision preset. Take care to activate the *Graspable* property.

![](./collisonExample.png)

![](./touchableExampleLevel.png)


### Setup In New Map

We recommend to create a new project starting with the VR Template. In order to have the project ready for VR. If you have an already existing map, you can follow the same steps.

Take the Player Start actor close to working area. Then go to world settings and set WeArtGamemode at GameMode Override.

![](./playerStart.png)

If you play the map in VR Preview, you should be able to see your hands.

Add to the map "CalibrationUXBP", "StatusTrackingBP" and "StatusTrackerDisplay" or just the "WEART_BP"

![](./threeBPSetup.png)

If the VR Priview is not active, refer to "Set-up project and import plugin" at the top of the page.     

## PC Windows Platform

### Architecture

![Unreal SDK Architecture](./architecture.png)

### Add C++ file

@note In order to use the WeArt Plugin, you must convert the project to a C++ project, on Unreal 5 you can find it in Tools 🡪 New C++ class. After the file was created, restart the project.

### Visual Studio requirements

If the conversion fails, we recommend following the next steps.

Download and install Visual Studio 2022 Community.

Follow this guide for installing the correct tools 
\htmlonly
 <a href="https://docs.unrealengine.com/4.26/en-US/ProductionPipelines/DevelopmentSetup/VisualStudioSetup/" target="_blank">Setting Up Visual Studio for Unreal Engine</a>
\endhtmlonly

If you still face difficulties continue doing the following.

Open the search bar and look for "Visual Studio Installer", it is an application.

Next find the 2022 version and press "Modify"

![](./visualStudioModify.png)

Check if the next tools are enabled.
Enable "Desktop development with C++" and make sure that it is enabled as by pressing it you can both enable and disable it ( you may want to use the Windows 10 sdk if you don't have Windows 11)
The "Individual components" can be found in the tab called "Individual Components" near "Workloads".

![](./visualStudioSettings1.png)

Enable "Game development with C++" and check the installed tools.

![](./visualStudioSettings2.png)

Enable ".NET desktop development" and check the tools installed.

@note C++ compilers according to visual studio version

| Visual Studio Version                | _MSC_VER |
|--------------------------------------|----------|
| Visual Studio 6.0                    | 1200     |
| Visual Studio .NET 2002 (7.0)        | 1300     |
| Visual Studio .NET 2003 (7.1)        | 1310     |
| Visual Studio 2005 (8.0)             | 1400     |
| Visual Studio 2008 (9.0)             | 1500     |
| Visual Studio 2010 (10.0)            | 1600     |
| Visual Studio 2012 (11.0)            | 1700     |
| Visual Studio 2013 (12.0)            | 1800     |
| Visual Studio 2015 (14.0)            | 1900     |
| Visual Studio 2017 RTW (15.0)        | 1910     |
| Visual Studio 2017 version 15.3      | 1911     |
| Visual Studio 2017 version 15.5      | 1912     |
| Visual Studio 2017 version 15.6      | 1913     |
| Visual Studio 2017 version 15.7      | 1914     |
| Visual Studio 2017 version 15.8      | 1915     |
| Visual Studio 2017 version 15.9      | 1916     |
| Visual Studio 2019 RTW 16.0          | 1920     |
| Visual Studio 2019 version 16.1      | 1921     |
| Visual Studio 2019 version 16.2      | 1922     |
| Visual Studio 2019 version 16.3      | 1923     |
| Visual Studio 2019 version 16.4      | 1924     |
| Visual Studio 2019 version 16.5      | 1925     |
| Visual Studio 2019 version 16.6      | 1926     |
| Visual Studio 2019 version 16.7      | 1927     |
| Visual Studio 2019 version 16.8, 16.9 a | 1928  |
| Visual Studio 2019 version 16.10, 16.11 b | 1929 |
| Visual Studio 2022 RTW 17.0          | 1930     |
| Visual Studio 2022 version 17.1      | 1931     |
| Visual Studio 2022 version 17.2      | 1932     |
| Visual Studio 2022 version 17.3      | 1933     |
| Visual Studio 2022 version 17.4      | 1934     |
| Visual Studio 2022 version 17.5      | 1935     |
| Visual Studio 2022 version 17.6      | 1936     |
| Visual Studio 2022 version 17.7      | 1937     |
| Visual Studio 2022 version 17.8      | 1938     |
| Visual Studio 2022 version 17.9      | 1939     |
| Visual Studio 2022 version 17.10     | 1940     |
| Visual Studio 2022 version 17.11     | 1941     |
| Visual Studio 2022 version 17.12     | 1942     |
| Visual Studio 2022 version 17.13     | 1943     |
| Visual Studio 2022 version 17.14     | 1944     |

@note If you're facing issues with vulnerable NUget packages - the root of the issue is in ".NET Framework Version"

![](./vulnrabable.png)

Unreal uses .NET to automate build processes with UBT - Unreal Build Tool. To fix that - try downgrading your target .NET framework during build.

1. Open solution with your project.
2. Add the terminal view, if you do not have one.
3. Write down ```dotnet --list-sdks```.
4. You will see list of installed .NET sdks.

For example, If you have .NET 9.0 (to see current .NET version - type ```dotnet --version```) 
downgrade target framework to .NET 8.0. After you installed older .NET version you need to 
specify used version in ```global.json``` file, in root folder of the project.

![](./globaljson.png)

In ```global.json``` write desired, downgraded .NET version:

``` 
{
  "sdk": {
    "version": "8.0.404"
  }
}
```

Now you can add the c++ class to the project and open it.

![](./visualStudioSettings3.png)

# Features Guide

## Level Setup

In the World Settings change the Default GameMode to WeArtGameMode.

![](./weArtPlayerBP.png)

If the VR Preview is inactive make sure to install the plugin that corresponds to your headset. 
Make sure only OpenXR plugin is enabled from all public VR API.

![](./inactiveVR.png)

\htmlonly
</br>
\endhtmlonly

@note Use OpenXR plugin for a better precision of the position and rotation for the virtual hands inside the experience.

![](./oculusPlugin.png)

@note The hand system is affected heavily by low FPS (Frames Per Second). For the best experience we recommend that the experience runs in minimum 60 FPS. This can be achieved by optimizing the experience (lower resolution textures, models with lower polygons, not using ray tracing if the device cannot run it properly)


## How to create Haptic/Touchable objects

By adding actor components to existing actors. Either insert any actor in the map,
and equip it with a Haptic/Touchable component and a collision, or create a
blueprint actor class which you can then reuse anytime you want. In the details
panel, you should have access to all the modifiable variables.


## Components

### WeArtController

It is actually created automatically as soon as you enable the plugin. But
if you want access to data members etc, just create a Blueprint Class
(Add->Blueprints->Blueprint Class) which inherits from WeArtController.

### WeArtTouchableObject

Component responsible for the description of the haptic effect to be applied in the event of a collision with the HapticObject actor.

Properties:

* Temperature – Temperature value implemented on the target thimble or thimbles (from 0.0 to 1.0) [0.5 is environment temp – 0.0 really cold – 1.0 really hot] 
* Force – Force value applied on the target thimble (s) (from 0 to 1) [0.0 no force – 1.0 max force]
* Texture – Type of texture rendered on the thimble or target thimbles (from 0 to N)
* Volume Texture: Configure the intensity of texture rendering (from 0 to 100) 
* Graspable: Enable the ability to grasp the object with virtual hands
* Surface Exploration - it will use a which will use a different strategy to explore the object's texture improving the haptics
* Disable Dynamic Force - uses stiffness value as direct value in force feedback, without taking distance magnitude.

![](./touchable.png)

The WeArtTouchableObject component has a field called ForcedVelocity. if it is enabled, when the hand enters the touchable object, the texture feeling will run at maximum speed. It will not take in consideration the movement of the hand.

![](./forcedVelocity.png)

Touchable object must have one of these collision types: WorldStatic, WorldDynamic or PhysicsBody. This list can be modified in HapticHand_BP.

![](./collisionChannels.png)

### WeArtHapticObject

Component responsible for the haptic actuation of the individual digital devices belonging to the TouchDiver device.

Properties:

* Hand Side Flag – Which device it belongs to (RIGHT | LEFT)
* Actuation Point Flag – Target of the thimble or the thimbles on which to apply implementation
  * TouchDIVER Pro (THUMB | MIDDLE | INDEX | ANNULAR | PINKY | PALM) [Multi selection]
  * TouchDIVER (THUMB | MIDDLE | INDEX) [Multi selection]
* Control: - (Set by TouchableObject during interaction or manually by developer)
  * Temperature – Temperature value implemented on the target thimble or thimbles (from 0.0 to 1.0) [0.5 is environment temp – 0.0 really cold – 1.0 really hot] 
  * Force – Force value applied on the target thimble (s) (from 0 to 1) [0.0 no force – 1.0 max force]
  * Texture – Type of texture rendered on the thimble or target thimbles (from 0 to N)


### WeArtThimbleTrackingObject

Component responsible for tracking of thimble's movements  for quantifying its closed state and animating virtual hands.
Properties:
* Hand Side Flag – Which device it belongs to (RIGHT | LEFT)
* Actuation Point Flag – Target of the thimble or the thimbles on which to apply implementation 
  * TouchDIVER Pro (THUMB | MIDDLE | INDEX | ANNULAR | PINKY) (Palm's value is fixed, it will always return 1)
  * TouchDIVER (THUMB | MIDDLE | INDEX)

### WeArtHandController

Component responsible for grasping, managing physics updates calls and getting touched object from all actuation points (THUMB | INDEX | MIDDLE | ANNULAR | PINKY | PALM)

### WeArtPhysicHandler

Component responsible for managing physics interactions, physics grasping, updating body setup for physical hand and grasped object and handling interactions between phantom hand and physical one.

### WeArtHapticController

Component responsible for handling effect applying, based ob touchable object, surface and haptic strategy, such as surface exploration.
Properties:
* Index Point - Finger Socket (Socket where capsule will be traced for surface exploration), Capsule Height, Capsule Radius
* Middle Point - Finger Socket (Socket where capsule will be traced for surface exploration), Capsule Height, Capsule Radius
* Annular Point - Finger Socket (Socket where capsule will be traced for surface exploration), Capsule Height, Capsule Radius
* Pinky Point - Finger Socket (Socket where capsule will be traced for surface exploration), Capsule Height, Capsule Radius
* Thumb Point - Finger Socket (Socket where capsule will be traced for surface exploration), Capsule Height, Capsule Radius
* Palm Point - Finger Socket (Socket where capsule will be traced for surface exploration), Capsule Height, Capsule Radius

### WeArtPhysicsInteractionComponent

Component responsible for allowing interacting anchored object with WeArtTouchableObject and enabled Graspable flag
Properties:
* Interaction Enabled - allows, or bans interacting with given anchored object.

### WeArtDeviceTracking

Component responsible for the tracking of the wrist on which the TouchDiver device is placed
Properties:

* Update method – Mode in which the position of the object is updated on the basis of the source tracking
* Tracking Source – Transform reference of motion controller source (ex. VR Controller or Vive Trackers, etc. etc.)
* Position Offset – Transform position offset respect from the tracking source
* Rotation Offset – Transform rotation offset respect from the tracking source
* Offset presets – You can choose a preset offset parameters like OpenXR

\warning <b>During changes in the hand offset while experience is running, features like haptics and grasping will be turned off. Use this only for setting up correct offset for your hand and the device.</b>

![](./deviceTracking.png)


## Hands System

These are the two blueprints for the hands:

![](./handsPrefabs.png)

### Hand Grasp Events

In order to enable the events to be called, drag this blueprint into the level.

![](./graspEventsBP.png)

The blueprint contains the following events inside it. Each event returns the hand side that grabbed the object and the object that was grabbed as a WeArtTouchableObject.

![](./graspEvents.png)


### Hand Offset

\warning <b>During changes in the hand offset while experience is running, features like haptics and grasping will be turned off. Use this only for setting up correct offset for your hand and the device.</b>

The two hands blueprints contain the component called WeArtDeviceTrackingObject.

These are the presets available:

![](./presetsList.png)


### Physical Grasping System

Grabbing and physical interaction is all part of UWeArtPhysicHandler class, which is responsible for managing the physics of a skeletal mesh representing a hand in a virtual reality environment. Physical interaction between touchable objects 
and hands handled using Physical Assets. Physic assets contains collision data for fingers, and being used during runtime. During grasping, UWeArtPhysicHandler rebuilds collision data from physics asset to create right offsets for each finger
part, because of that we can handle precise hit events and support solid grabbing.

![](./physicAsset.png)

Grasping works as with physics bodies, and with bodies with Block collision profile. 

### Phantom Hand

For debugging purposes you can use Phantom Hand, which is in-game representation of current hand in world, It can be useful to check force with which fingers push into collision object. To turn It on, use ShowDebugHand parameter in HapticHand_BP.

![](./ShowDebugHand.png)

## Gesture System

Gestures allow to bind functionality, events e.t.c to your hand, by combining different closure values of fingers. You can create your custom gestures, or try our example ones, like teleportation or spawning test UI panel. To find more information about teleport or ui panel, move to next section. In ```WeArtGestures``` component stores all values for gestures used in runtime, here you can configure, test, remove or add gestures. 

![](./Gestures.png)

@note To enable gesture system, set to ```true``` Gestures flag in plugin settings.

![](./pluginGesturesFlag.png)

@note By default two gestures are disabled: Teleportation and Information Panel.

![](./GesturesFlag.png)

To bind functionality to gesture, use ```OnGestureCheck``` function in ```HapticHand_BP```, modify Switch node and bind you gestures by "gesture name -> logic"

![](./GestureBinding.png)

If you want to use it in your system - use this event in ```HapticHand_BP/WeArtGestureComponent```. 

![](./GestureEvent.png)

@note To get access to this event in other blueprints:

* Get reference to HapticHand_BP (one that casts gesture).
* Get component of type WeArtGesture.

![](./GestureCompFinding.png)

And here is example how you can use function callback:

![](./Callback.png)

To see all gestures in your hand, just look into ```WeArtGesture``` component gesture list:

![](./WeArtGestureComponent.png)

@note Gestures now support Touch Diver Pro actuation points

![](./tdProGestures.png)

### Teleportation

Teleport consists of two parts: trajectory showing and teleport activation. To show the trajectory, close your index and thumb while keeping your middle fingers open, forming a 'pinch' gesture. You will see a laser curve: green if teleportation is reachable and red if teleportation is not allowed at the destination point. To activate teleport, keep "pinch" gesture and close your middle fingers. You can teleport to any surface set to 'Character can step on'.

![](./CharacterStep.png)

![](./GreenLaser.png)

![](./RedLASER.png)

### UI Panel

You can test ui interaction with test panel, to cast it - hold fist closed for 3 seconds. To hide it - repeat first step.

![](./UIPanelCast.png)

![](./UIPanelButton.png)

This panel can be replaced or customized, you can find it in ```Blueprints/Component/UI Panel``` and widget class in ```UI/Front Panel```.


## Anchored Objects

SDK now supports physics interactions with anchored objects, using physic constraints. You can achieve behavior like opening doors, interacting with levers, linear motor movements and everything what Unreal's PhysicsConstraint component supports.

![](./lever.png)

![](./valve.png)

![](./piccolo.png)


To setup Anchored Object, you need 4 main components: WeArtTouchableObject with *Graspable* flag enabled, WeArtPhysicsInteraction with *Interaction Enabled* flag enabled, StaticMeshComponent with physics simulation, PhysicsConstraint component constrained to StaticMeshComponent.

![](./DoorExample.png)

The entire physics behavior relies on constraints. To customize the behavior of your physics intractable object, you can modify constraints such as linear and angular motors, swing movements, etc. Additionally, properties like mass and damping also affect the object's behavior, so pay attention to these as well.

## Surface Exploration

If you want to provide continuous feedback on surface exploration from touching different surfaces, you can turn on the touchable object ```Surface Exploration``` checkbox, which will use a different strategy to explore the object's texture, using additional collisions and ray casts to obtain data about the surface of the touched object. 

![](./touchableSurfaceExploration.png)

Make sure, that you have minimum two contact points on the surface, otherwise default strategy will work.

## Weart App Information

You can easily setup weart app components - just drag and drop to level ```WEART_BP```. This actor contains all components to communicate with the weart app. If you do not need all the components, but just several ones - you can use them independently.

![](./WEART_BP.png)

The "StatusTrackerDisplay" blueprint (shown below) is a 2D canvas containing all the status information received by the weart app.

![](./middlewarestatusdisplay.png)

In particular, the object will display:
* The current application version
* The current status of the application (as shown in the weart application UI)
* Whether actuations are enabled or not
* The last status code received from the application and, if not ok, a description of the error
* The status of the connected TouchDIVERs
  * Mac Address
  * Assigned HandSide
  * Battery level (and will show a bolt symbol if the device is charging)
  * Calibration status during the session
  * Status of each thimble (shown on the hand icon by the three colored dots)

The icons indicate:

![](./Actuations_On.png)
<b>Actuations Enabled</b>

![](./Autoconnection_On.png)
<b>Autoconnection Enabled</b>

![](./RawData_On.png)
<b>Raw Data Log Enabled</b>

![](./AnalogSensorsData_On.png)
<b>Sensor On Mask Enabled</b>

In order to have the weart app status display working in the scene, add the following components:

![](./statusdysplayitems.png)

![](./statusdysplayitemsTrackingBP.png)

### Status Codes

The current status codes (along with their description) are:

| Status Code |   | Description |
|---|---|---|
| 0 | OK | Ok |
| 100 | START_GENERIC_ERROR | Can't start generic error: Stopping |
| 101 | CONNECT_THIMBLE | Unable to start, connect at least one thimble and retry |
| 102 | WRONG_THIMBLES | Unable to start, connect the right thimbles matched to the bracelet and retry |
| 103 | BATTERY_TOO_LOW | Battery is too low, cannot start |
| 104 | FIRMWARE_COMPATIBILITY | Can't start while the devices are connected to the power supply |
| 105 | SET_IMU_SAMPLE_RATE_ERROR | Error while setting IMU Sample Rate! Device Disconnected! |
| 106 | RUNNING_SENSOR_ON_MASK | Inconsistency on Analog Sensors raw data! Please try again or Restart your device/s! |
| 107 | RUNNING_DEVICE_CHARGING | Can't start while the devices are connected to the power supply |
| 200 | CONSECUTIVE_TRACKING_ERRORS | Too many consecutive running sensor errors, stopping session |
| 201 | DONGLE_DISCONNECT_RUNNING | BLE Dongle disconnected while running, stopping session |
| 202 | TD_DISCONNECT_RUNNING | TouchDIVER disconnected while running, stopping session |
| 203 | DONGLE_CONNECTION_ERROR | Error on Dongle during connection phase! |
| 204 | USB_CONNECTION_ERROR | | Can not use the device through Bluetooth while connected to a USB port |
| 300 | STOP_GENERIC_ERROR | Generic error occurred while stopping session |

@note The description of each status code might change between different weart app versions, use the status code to check instead of the description.

### StatusTrackingBP

If this blueprint is placed in the scene, it will fire an event for every received message about the weart app status.
There are two events that can be fired:

#### OnMiddlewareStatus

![](./statusTrackingBP.png)

#### OnDevicesStatus

![](./onDevicesStatus.png)

These two events are just signaling when one of the two data types is received. If you want to access them do it in EventTick. Then access the local data. This data gets updated automatically.

![](./middlewareStatusSearch.png)

Then split the struct to access the values.

![](./splitStruct.png)

The same can be done for ConnectedDeviceStatusLeft and ConnectedDeviceStatusRight.

![](./onDevicesStatus.png)

If you want to get access via public call, you can follow these steps:

1. Make sure you have StatusTrackerDisplay on level.
2. Make sure you have StatusTrackerBP on level.
3. Open it and add an Event Dispatcher.
4. In Tick Event, drag and drop it here, connect execution pins
5. Subscribe to event through finding status tracker on level.

![](./eventDispatcher.png)

![](./eventDispatcherPins.png)

![](./eventSubscribtion.png)

### WeArtThimbleSensorObject

If you want to use this feature, you will have to enable RawDataAutoStart in Project Settings -> WeArt

![](./rawdatasettings.png)

@note If inside the Project Settings, the RawDataAutoStart is checked, the calibration will be forced. If you want to use CalibrationUXBP or TrackingCalibrationBP, disable RawDataAutoStart.

This component can be added to any actor and by accessing it in EventTick, you can get the sensor data of the specified thimble. The component updates the values automatically.

![](./rawDataTest.png)

Make sure to set the Hand Side and Actuation Point.

![](./rawDataHandSide.png)

## Calibration UX Blueprint

The CalibrationUXBP offers an easy and precise way of calibrating the touch diver at the start of the experience.
You need to put your hands in the position specified and it will signal to the weart app that the calibration process is starting.

@note If inside the Project Settings, the RawDataAutoStart is checked, the calibration will be forced. If you want to use CalibrationUXBP, disable RawDataAutoStart.

You can drag and drop the following blueprint in the scene:

![](./CalibrationUXBP.png)

The blue hands represent the place in which we need to hold the hands for the calibration to start. This is how it should look in the scene:

![](./CalibrationUXDisplay.png)

@note The system will automatically detect the hands connected to the middleware and will present the corresponding hand destination. If for example there will be only the right hand connected. Only the right hand destination will be available. If both hands are connected to the middleware, both hands' destinations will be present.

## Actuation Panel

The actuation panel allows the user to see the variables of the effects that are currently applied for each haptic object, that includes the fingers and the palm. That gives six panels for TDPro (Thumb, Index, Middle, Annular, Pinky and Palm) for each hand, and for TD each hand will have three panels for (Thumb, Index and Middle). When the experience starts, the panels connect to the haptic objects found in the level and they change their name from "Finger" to the corresponding name of the actuation type.

Here we can see the actuation panel before starting the experience:

![](./actuationPanel.png)

And here it is when the experience is running:

![](./actuationPanelRunning.png)

Each panel provides information about:
* Force [0-1]
* Temperature[0-1]
* Texture
  * Texture Type
  * Velocity [0-1]
  * Texture Volume [0-100]

Here we can see that the values of the panels change based on the effect applied:

![](./actuationPanelValues.png)

This is the folder where we can find the actuation panel, if it is not already present in the level:

![](./actuationPanelFolder.png)

## Hand Offset Panel

If you want to adjust offset of the tracking device in runtime - you can use offset widget, here you can 
insert location and rotation to test, what fits you more. This is useful, if you use custom solutions for
tracking.

* Press ``` ` ``` sign
* Type ``` ToggleOffsetPanel ```

![](./ToggleOffsetCommand.png)

You will see offset panel appear on the left part of the display. You need to use the keyboard to handle inputting values.
Since combining mouse input and VR mode introduces bugs with widget interaction with using mouse as source, here are
steps how to use the panel:

![](./OffsetPanel.png)

1. Press tab until selection comes to first input field.
2. When first input field will be selected - you can start typing values with keyboard.
3. **Do not press ```enter```** to confirm the input, instead you will deselect the panel and save offsets to HandOffset data asset.
4. In case you deselected panel - just click with mouse inside play mode windows, and start from first step.
5. To go to next input field - press ```tab```, moving to previous one - ```shift + tab```.
6. Pressing ```tab``` or ```enter``` keys will save the offset.

To see the offset after you finished adjusting the hand - move to ```Blueprints/LevelSettings/HandOffset```, from there
you can copy offset values and use them in hand blueprints.

![](./OffsetLog.png)

## API

### Start/Stop Client

Once connected to the weart app, it's still not possible to receive tracking data and send haptic commands to the devices. In order to do so, it's important to start the weart app with the proper command.

From WeArtController GameInstance get Subsystem

~~~~~~~~~~~~~{.cpp}
UGameInstance* gameInstance = GetOuter()->GetWorld()->GetGameInstance();
UWeArtController* weArtController = gameInstance->GetSubsystem<UWeArtController>();
~~~~~~~~~~~~~

~~~~~~~~~~~~~{.cpp}
weArtController->PauseController(); 	//Start Weart App communication
weArtController->UnpauseController(); 	//Stop Weart App communication
~~~~~~~~~~~~~

![](./unpause.png)

### Start/Stop Calibration and events

From WeArtController GameInstance get Subsystem

~~~~~~~~~~~~~{.cpp}
UGameInstance* gameInstance = GetOuter()->GetWorld()->GetGameInstance();
UWeArtController* weArtController = gameInstance->GetSubsystem<UWeArtController>();
~~~~~~~~~~~~~

~~~~~~~~~~~~~{.cpp}
weArtController->StartCalibration(); 	//Start Weart App calibration
weArtController->StopCalibration(); 	//Stop Weart App calibration
~~~~~~~~~~~~~

Blueprint:

![](./calibration.png)

In order to enable the events to be called, drag this blueprint into the level.

![](./trackingCalibrationBP.png)

Inside this blueprint there are four events that are fired when the calibration starts, finishes, it succeeds or fails. They all return the side of the hand that called the event.

![](./calibrationEvents.png)

There is another event that fires when the calibration stops and it does not contain a hand side.

![](./calibrationStop.png)

### WeArtTouchEffect

The SDK contains a basic WeArtTouchEffect class to apply effects to the haptic device. The TouchEffect class contains the effects without any processing. For different use cases (e.g. values not directly set, but computed from other parameters), create a different effect class by implementing the WeArtEffect interface.

### Create Custom effect

**Blueprints**

Touch effect can be constructed using Construct Object From Class node. Touch effect is now UCLASS, so It supports engine's reflection.
You can see a list of accessed members by typing WeArt, when you drag out of constructed object pin.

![](./effectConstructing.png)

**C++**

Instantiate new effect:
~~~~~~~~~~~~~{.cpp}
IndexGraspEffect = NewObject<UWeArtTouchEffect>(this);
IndexGraspEffect->Init(FWeArtTemperature(), FWeArtForce(), FWeArtTexture());
~~~~~~~~~~~~~

Init used as constructor method, you must call It after allocating effect in memory.

**Blueprints**

Getters supporting splitting their output pin, so you can use effect's fields in blueprints.

![](./splitStruct.png)

To use constructed effect you can use these methods:

![](./hapticApply.png)

**C++**

Create and activate actuations

~~~~~~~~~~~~~{.cpp}
FWeArtForce force;
force.active = true; 	//active actuation
force.value = valueForce;	//set value
~~~~~~~~~~~~~

Set actuation to effect:

~~~~~~~~~~~~~{.cpp}
effect->Set(temperature, force, texture);
~~~~~~~~~~~~~

### Add effect

**Blueprints**

Use AddEffect node WeArtHapticObject.

![](./addEffect.png)


**C++**

Apply to your HapticObject (finger/thimble) effect:

~~~~~~~~~~~~~{.cpp}
hapticObject->AddEffect(effect);
~~~~~~~~~~~~~

### Update effect

**Blueprints**

Use *UpdateEffects* method of WeArtHapticObject to update and actuate haptics effects:

![](./updateEffects.png)


**C++**

Update effects to your HapticObject:

~~~~~~~~~~~~~{.cpp}
hapticObject->UpdateEffects();
~~~~~~~~~~~~~

### Remove effect

**Blueprints**

Use Remove Effect for removing specific effect, or Remove Last Effect

![](./removeEffect.png)

**C++**

To remove effect and restore actuation, get the same instance of effect and call “Remove” for the same HapticObject:

~~~~~~~~~~~~~{.cpp}
hapticObject->RemoveEffect(effect)
~~~~~~~~~~~~~

### Tracking 

After starting the weart app and performing the device calibration, it's possible to receive tracking data related to the TouchDIVER thimbles.

The player pawn contains two UWeArtHandController. One for each hand. Every UWeArtHandController contains three UWeArtThimbleTrackingObject. One for index, middle and thumb. From each UWeArtThimbleTrackingObject we can get the closure and abduction values. The values are from 0 to 1, 0 representing no closure or abduction and the 1 representing the maximum value.

Getting Closure and Abduction:

~~~~~~~~~~~~~{.cpp}
UWeArtThimbleTrackingObject* thumbTrackingObject;
thumbTrackingObject->GetClosure();
thumbTrackingObject->GetAbduction();
~~~~~~~~~~~~~

Here is a blueprint representation of getting the values, in this image the values are used for the animation of the hands.

![](./handAnimation.png)

### Adding new custom texture type

WeArtCommon.h contains an enum called TextureType. We can add a new member here and assign its number. Then inside the code and the editor we can set this member to WeArtTouchableObjects and inside Texture classes.

![](./exampleType.png)

After compiling the code, the member will be available in the editor:

![](./touchableExample.png)

## Level Content

### Blueprints

Commonly used blueprint classes stored in Blueprints folder, in ```Actors/Controllers``` you can find blueprints related to calibration and grasping functionality.

![](./componentsBP.png)

Hands are stored in HandBlueprints folder, Left and Right inherit from HapticHand_BP, If you want to make changes for both hands, you can implement them in HapticHand_BP.

![](./handBlueprints.png)

### Calibration

If you did not set calibration on start, you can do that manually by using WEART_BP located in Blueprints\Actors\Setup directory. 

![](./calibrationLevel.png)

* StatusTrackerDisplay - is the widget that show device status and properties.
* CalibrationUXBP - handles calibration process and shows the status of the device.
* StatusTrackingBP - actor that communicates between the weart app and StatusTrackerDisplay.

@note If you want to restart calibration you can press button F on keyboard to reopen CalibrationUX panel

When you restart calibration you are clearing calibration state. you can find it in WeArtPlayerController. There is delay for 2.5 seconds to start controller from weart app.
Reset method need to stop connection with weart app
Restart Connection send message to weart app start connection with hand
![](./RestartCalibration.png)

In WeArtPlayerController you can find Input action for restart calibration

@note If you want switch levels and don't want to lose connection with hand you need to use the same game mods like in previous level. If you want to store information about ux calibration state you need to set in project settings game instance to BP_GameInstanceUX. In BP_GameInstanceUX you have boolean value HideUXCalibration. This value is false but when you successful complete calibration in CalibrationUX this value will be set to true. When you restart calibration the value will be set to false.

# Collision and Physics Best Practices

## Component hierarchy

For collision presets you can choose between these:

![](./PhysicsActorPreset.png)

![](./BlockAllPreset.png)

![](./BloackAllDynamicPreset.png)

Our hand loves precise colliders, if you want to achieve as much as possible realistic haptic feedback, do not hesitate using complex, or convex with large hull count colliders.

Avoid using more than one constraint inside touchable object for physics interaction, wrong constraint can be used during interaction. 

@note Exception to multiple constraints can be using multiple constraints for reaching stability during interaction, like door hinges.

Avoid using multiple child static meshes for linear motion anchored object, It can lead to jittering during grasping.

# Migration and Update from previous SDK

Close the project, clean any reference from the previous WEART SDK plugin and copy the new one on the plugin folder, open the scene and in replace any blueprint with new components provided by the new SDK version.

# Troubleshoot

## Performance

If you're facing performance issues, try at first use profiling tools, like UnrealInsights, or GPU profiler.
Also, using console commands in VR mode can be useful, for detecting resource consumption and bottlenecks in scope of your project.

Commands:

* Press ``` ` ```, make sure you have ENG localization on keyboard
* Run command ```Stat FPS```
* Run command ```Stat UnitGraph```

Significant fps boost will be reducing GPU load, by changing anti-aliasing algorithm. By default in project, especially with maximum quality preset, will be used TSR. 
Setting anti-aliasing algorithm from TSR to FXAA gave the best fps boost. To do that, go to Project Settings -> Type in search tab "anti" -> Set FXAA

![](./antialiasing.png)

## Build Issues

Pay attention to what version of MSVC, dotnet and Unreal you use. Issues like vulnerable packages, undefined macros, related to components of the visual studio. 
* If you face these - downgrade dotnet version, using ```global.json``` file in root directory with your solution file - reference: https://learn.microsoft.com/en-us/dotnet/core/tools/global-json. 
* To check current dotnet version - run command, for example, in terminal of the visual studio ```dotnet --version``` , to list all dotnet sdks installed - ```dotnet --list-sdks``` .
* If you face issue with UE5.3 bug directly, related to possible null reference in Unreal's build tool (this is not a part of our SDK, but the Unreal itself) reference: https://forums.unrealengine.com/t/error-compiling-the-automation-tool-after-updating-visual-studio-today-unreal-5-3-2/1393088/5 . To fix that, follow the instruction on the screenshot from link.