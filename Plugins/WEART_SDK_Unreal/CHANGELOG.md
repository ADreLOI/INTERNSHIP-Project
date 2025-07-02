
# Change Log
All notable changes to this project will be documented in this file.

## [2.1.0]

### Added 
* Actuation panel that displays the effects applied on the haptic objects of the hands.
* Disable Dynamic Force flag in touchable object.
* Offset panel widget with realtime adjusting location and rotation values for hands.
* Offset Panel toggle with console command.
* Hand Offset saving to data asset.

### Updated

* Hand Calibration UX now can detect what hands are used and it is not necessary to manually set what hands are used
### Fixed

## [2.0.0]

### Added 

* TouchDIVER Pro PC Support: Added full compatibility for TouchDIVER Pro on PC platforms. 
* Device Switching in WeArtSettings: Introduced the ability to seamlessly switch between TouchDIVER and TouchDIVER Pro directly from the WeArtSettings menu.
* WeArtPhysicsInteraction Component: Added a new component enabling interaction with anchored objects for enhanced physics-based interactions.
* Gesture System: it is possible to create and track the hand gestures.
* Teleportation System — A teleportation feature that operates using gestures. You can configure a "Teleport Trajectory" gesture to display a teleportation laser that indicates the target location, showing whether teleportation to that spot is possible (it checks for obstacles and validates actors, where user can step on) and "Teleport Activation" gesture to teleport to position. The teleportation target can be set as either the name of a game object or a physical layer defined in the project settings.
* UI Interaction allows to press buttons and recognize slate core objects on touched widget.
* Gesture flag in plugin settings allows to control whether you want to use gesture system in the project, or not.

* Device Generation Flag in plugin settings allows to select, whether you have TouchDIVER and Middleware, or TouchDIVER Pro and Weart App
* TouchDIVER Pro actuation points: Annular, Pinky and Palm
* Weart App support
* Teleportation flag in HapticHand_BP
* Information Panel flag in HapticHand_BP
* WEART_BP actor to make setup easy
* Independent Haptic Object, for easy integration as attached child actor with overlap volume, to get haptic effect from touchable object.

* Actors/ Folder
* Actors/Core Folder
* Actors/Extras Folder
* Actors/Setup Folder
* Actors/UI Folder

### Updated

* Enhanced Gestures for TouchDIVER Pro: Added refined touch gestures tailored for smoother and more intuitive interactions with the TouchDIVER Pro interface.
* Redesigned Status Display: Introduced new messages and an updated layout for clearer, more user-friendly status information.
* Interactive Sample Level: Integrated a sample level featuring advanced physics-based interactions with anchored objects for more engaging user experiences.
* Improved Bunsen Effects: Upgraded visual and functional effects associated with Bunsen for greater impact and realism.
* High-Quality Texture Plates: Added texture plates with surface exploration for a more polished and immersive haptic feedback.
* Diverse Physics and Haptic Setups: Updated objects in the sample level with unique physics behaviors and haptic feedback configurations.
* Device Generation Display: The status panel now dynamically indicates the selected device generation (TouchDIVER or TouchDIVER Pro).

### Fixed

* Resolved Two-Hand Grasping Issues: Fixed problems with two-hand grasping to ensure seamless and accurate interactions.
* Corrected Default Hand Offset: Adjusted the default hand offset for proper alignment and improved usability.


### Moved

* Haptic Object and Independent Haptic Object to Actors/Core/
* StatusTrackingBP, TrackingCalibrationBP and WeArtGraspingEvents to Actors/Controllers/
* Teleportation Actors to Actors/Extras/ 
* WEART_BP to Actors/Setup/
* CalibrationUXBP, StatusTrackingDisplay and UIPanel to Actors/UI/

### Removed

* Components Folder
* SDK-Components Folder
* Scriptable Touchable Object

## [1.3.0]

### Added

* Surface Exploration flag in Touchable Object. Allows easier exploration of surfaces on touchable objects that have SurfaceExploration enabled
* Touch effect can be managed by blueprints
* TouchableObject's property now are editable via blueprints
* Grabbing object with multiple static mesh components

### Changed

* Haptics on DemoLevel


## [1.2.0]
 
### Added

* New physic hand and grasping system
* New BP and UI panel fo Middleware and device status 
* Added Tracking sensor raw data
* Expose public property closure thimble
* Add calibration procedure start/stop and listener
* Add hand grasping events
* New sample scene

### Changed
* Haptic/Touchable objects no longer require overlap events

## [1.1.0] 

### Added

* Expose public property closure thimble
* Add calibration procedure start/stop and listener
* Add hand grasping events
* Fix Blueprint behaviors Hand